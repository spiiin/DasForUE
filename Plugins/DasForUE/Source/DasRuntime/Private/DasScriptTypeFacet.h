#pragma once
#include "DasTypeAdapters.h"
// Requires the daScript AST headers, included by DasRuntime.cpp with its packing and macro guards.

// Where lowering uses the view of an adapted struct. The math structs differ: Vector2D has no
// authored-reference view and InputActionValue reaches native properties through its opaque source type.
enum class EDasScriptViewUse : uint8
{
    None = 0,
    NativeProperty = 1,
    SelfProperty = 2,
    AuthoredReference = 4
};
ENUM_CLASS_FLAGS(EDasScriptViewUse)
// Where scalar members of a view (x/y/z, pitch/yaw/roll) are accessed in place. Vector2D members are
// mutable through authored references but not as self leaves.
enum class EDasMemberUse : uint8
{
    None = 0,
    SelfLeaf = 1,
    AuthoredMutation = 2
};
ENUM_CLASS_FLAGS(EDasMemberUse)

// daScript view of an adapted value type: recognising the view, crossing the boundary and making the view
// type for properties of the type (Docs/Dev/Design/TypeAdapters.md).
class IDasScriptTypeFacet
{
public:
    virtual ~IDasScriptTypeFacet() = default;
    virtual bool MatchesScriptType(const das::TypeDeclPtr& Type) const = 0;
    virtual das::ExpressionPtr Pack(const das::LineInfo& At, das::ExpressionPtr Value) const = 0;
    virtual das::ExpressionPtr Unpack(const das::LineInfo& At, const das::TypeDeclPtr& Type,
                                      das::ExpressionPtr Handle) const = 0;
    virtual EDasScriptViewUse GetViewUses() const = 0;
    // View type in the module group, or nullptr when its module is not imported (see GetViewModule).
    virtual das::TypeDeclPtr MakeView(das::ModuleGroup& Group, das::Module* Module, const das::LineInfo& At) const = 0;
    virtual const TCHAR* GetViewModule() const = 0;
    // Scalar (double) member of the view usable in place for this use.
    virtual bool HasMember(const das::string& Name, EDasMemberUse Use) const { return false; }
};
