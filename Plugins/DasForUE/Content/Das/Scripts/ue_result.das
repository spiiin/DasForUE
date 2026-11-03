/*
Adapted from dasSDL3/dassdl3/sdl3_try.das.
MIT License

Copyright (c) 2026 dasSDL3 contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
// Result propagation follows dasSDL3's sdl_try inference-pass design.
// No exceptions or native protected calls; ordinary return/defer semantics apply.
options gen2
module ue_result private
require daslib/result public
require ue_object_types public
require daslib/ast
require daslib/ast_boost
require daslib/templates_boost

// Type-inference signature only. The infer pass expands direct uses and rejects
// all remaining calls/addresses before optimization or execution.
[unused_argument(value)]
def public ue_try(value : $Result<auto(T); auto(E)>) : T {
    var inferred : T
    return <- inferred
}

def is_ue_try_function(fn : Function?) : bool {
    if (fn == null) { return false }
    let origin = fn.fromGeneric
    return origin != null && origin.name == "ue_try" && origin._module.name == "ue_result"
}

def is_ue_result(rt : TypeDeclPtr) : bool {
    if (rt == null || rt.baseType != Type.tTuple || length(rt.argTypes) != 3 || length(rt.argNames) != 3) { return false }
    return (rt.argNames[0] == "_is_ok" && rt.argNames[1] == "_value" && rt.argNames[2] == "_error"
        && rt.argTypes[0].baseType == Type.tBool)
}

class UETryVisitor : AstVisitor {
    changed : bool
    @do_not_delete func : Function?
    returns : array<TypeDeclPtr>
    in_finally : int
    def override preVisitExprBlockFinal(blk : ExprBlock?) { in_finally++ }
    def override visitExprBlockFinal(blk : ExprBlock?) { in_finally-- }
    def override preVisitFunction(var fn : FunctionPtr) {
        func = fn
        clear(returns)
        var rt = clone_type(fn.result)
        emplace(returns, rt)
    }
    def override preVisitExprBlock(var blk : ExprBlock?) {
        if (blk.blockFlags.isClosure) {
            var rt = clone_type(blk.returnType)
            emplace(returns, rt)
        }
    }
    def override visitExprBlock(var blk : ExprBlock?) : ExpressionPtr {
        var expanded : array<ExpressionPtr>
        for (stmt in blk.list) {
            var target = stmt
            if (stmt is ExprLet && length((stmt as ExprLet).variables) == 1) {
                target = (stmt as ExprLet).variables[0].init
            } elif (stmt is ExprCopy && (stmt as ExprCopy).left is ExprVar) {
                target = (stmt as ExprCopy).right
            } elif (stmt is ExprMove && (stmt as ExprMove).left is ExprVar) {
                target = (stmt as ExprMove).right
            }
            if (target != null && target is ExprCall && is_ue_try_function((target as ExprCall).func)) {
                let call = target as ExprCall
                let tmp = make_unique_private_name("__ue_try", call.at)
                if (empty(returns) || !is_ue_result(returns[length(returns) - 1]) || in_finally != 0) {
                    macro_sticky_error(compiling_program(), call.at, "ue_try requires an enclosing Result-returning function or block")
                    var copy = stmt
                    expanded |> emplace(copy)
                    continue
                }
                let rt = returns[length(returns) - 1]
                let output = rt.argTypes[1]
                var prefix = qmacro_block() {
                    var $i(tmp) <- $e(call.arguments[0])
                    if (!$i(tmp)._is_ok) {
                        return result::err($i(tmp)._error, type<$t(output)>)
                    }
                }
                // Copy scalar/string/pointer results, consume move-only results.
                ((prefix as ExprBlock).list[0] as ExprLet).variables[0].flags.init_via_move = !call.arguments[0]._type.canCopy
                prefix |> force_at(call.at)
                for (item in (prefix as ExprBlock).list) {
                    var copy = item
                    expanded |> emplace(copy)
                }
                if (stmt is ExprLet) {
                    (stmt as ExprLet).variables[0].init = qmacro($i(tmp)._value)
                    var copy = stmt
                    expanded |> emplace(copy)
                } elif (stmt is ExprCopy) {
                    (stmt as ExprCopy).right = qmacro($i(tmp)._value)
                    var copy = stmt
                    expanded |> emplace(copy)
                } elif (stmt is ExprMove) {
                    (stmt as ExprMove).right = qmacro($i(tmp)._value)
                    var copy = stmt
                    expanded |> emplace(copy)
                }
                changed = true
                func.not_inferred()
            } else {
                var copy = stmt
                expanded |> emplace(copy)
            }
        }
        clear(blk.list)
        for (item in expanded) { blk.list |> emplace(item) }
        if (blk.blockFlags.isClosure) { pop(returns) }
        return <- blk
    }
}
class UETryUnexpanded : AstVisitor {
    def override visitExprCall(var call : ExprCall?) : ExpressionPtr {
        if (is_ue_try_function(call.func)) {
            macro_sticky_error(compiling_program(), call.at, "ue_try must be a standalone statement, the sole initializer of one let/var, or the full RHS of assignment to a variable; it is forbidden in finally/defer")
        }
        return <- call
    }
    def override visitExprAddr(var expr : ExprAddr?) : ExpressionPtr {
        if (is_ue_try_function(expr.func)) {
            macro_sticky_error(compiling_program(), expr.at, "ue_try is a macro; taking its function address is forbidden")
        }
        return <- expr
    }
}
[infer_macro]
class UETryPass : AstPassMacro {
    def override apply(prog : ProgramPtr; mod : Module?) : bool {
        var visitor = new UETryVisitor()
        make_visitor(*visitor) $(adapter) { visit_module(prog, adapter, mod) }
        let changed = visitor.changed
        unsafe { delete visitor }
        var remaining = new UETryUnexpanded()
        make_visitor(*remaining) $(adapter) { visit_module(prog, adapter, mod) }
        unsafe { delete remaining }
        return changed
    }
}

// Guards return Results so callers can compose validation without changing
// ordinary false/None payloads into errors implicitly.
def public ue_require(condition : bool; error : auto(E)) : Result<bool; E -const> {
    return condition ? ok(true, type<E>) : err(error, type<bool>)
}

def public ue_require_valid(value : auto(T); error : auto(E)) : Result<T -const; E -const> {
    if (!ue_is_valid(value)) { return err(error, type<T>) }
    return ok(value, type<E>)
}
