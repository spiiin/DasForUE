#pragma once
#include "DasTypeAdapters.h"
struct FDasDeclaredLiteral;
class FJsonValue;
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

// Call transports a view may own: a call-frame slot (ue_frame_out_<suffix>) and a method bridge
// (__ue_method_arg_<suffix>, __ue_arg_input_value). Without one the value crosses as a struct snapshot.
enum class EDasCallBridge : uint8
{
    FrameSlot,
    MethodBridge
};

// daScript view of an adapted value type: recognising the view, crossing the boundary and making the view
// type for properties of the type (Docs/Dev/Design/TypeAdapters.md).
enum class EDasScriptDefaultUse : uint8
{
    Property,
    StructField,
    Parameter
};
class IDasScriptTypeFacet
{
public:
    virtual ~IDasScriptTypeFacet() = default;
    virtual bool MatchesScriptType(const das::TypeDeclPtr& Type) const = 0;
    virtual das::ExpressionPtr Pack(const das::LineInfo& At, das::ExpressionPtr Value) const = 0;
    virtual das::ExpressionPtr Unpack(const das::LineInfo& At, const das::TypeDeclPtr& Type,
                                      das::ExpressionPtr Handle) const = 0;
    virtual bool MatchesCallType(const das::TypeDeclPtr& Type) const { return MatchesScriptType(Type); }
    virtual bool IsNativeType(const das::TypeDeclPtr&) const { return false; }
    virtual FString ResolveTypePath(const das::TypeDeclPtr& Type, const FString&, const FString&) const
    {
        return GetTypePath(Type);
    }
    virtual das::ExpressionPtr PackForPath(const das::LineInfo& At, const das::TypeDeclPtr& Type,
                                           das::ExpressionPtr Value, const FString&) const
    {
        return PackTyped(At, Type, Value);
    }
    virtual das::ExpressionPtr UnpackForPath(const das::LineInfo& At, const das::TypeDeclPtr& Type,
                                             das::ExpressionPtr Value, const FString&) const
    {
        return Unpack(At, Type, Value);
    }
    virtual bool ParseDefault(EDasScriptDefaultUse, const das::TypeDeclPtr&, das::ExpressionPtr, das::ModuleGroup&,
                              das::Module*, FDasDeclaredLiteral&) const
    {
        return false;
    }
    virtual das::ExpressionPtr DefaultExpression(const FDasDeclaredLiteral&, const das::LineInfo&) const
    {
        return nullptr;
    }
    virtual TSharedPtr<FJsonValue> DefaultJson(const das::TypeDeclPtr&, das::ExpressionPtr, das::ModuleGroup&,
                                               das::Module*) const
    {
        return nullptr;
    }
    virtual int UpcastDistance(const das::TypeDeclPtr&, const das::TypeDeclPtr&) const { return -1; }
    virtual das::ExpressionPtr Upcast(const das::LineInfo&, const das::TypeDeclPtr&, const das::TypeDeclPtr&,
                                      das::ExpressionPtr) const
    {
        return nullptr;
    }
    virtual FString GetTypePath(const das::TypeDeclPtr&) const { return {}; }
    virtual das::ExpressionPtr PackTyped(const das::LineInfo& At, const das::TypeDeclPtr& Type,
                                         das::ExpressionPtr Value) const
    {
        return Pack(At, Value);
    }
    virtual das::TypeDeclPtr MakeViewForPath(const FString&, das::ModuleGroup& Group, das::Module* Module,
                                             const das::LineInfo& At) const
    {
        return MakeView(Group, Module, At);
    }
    virtual EDasScriptViewUse GetViewUses() const = 0;
    // View type in the module group, or nullptr when its module is not imported (see GetViewModule).
    virtual das::TypeDeclPtr MakeView(das::ModuleGroup& Group, das::Module* Module, const das::LineInfo& At) const = 0;
    virtual const TCHAR* GetViewModule() const = 0;
    // Scalar (double) member of the view usable in place for this use.
    virtual bool HasMember(const das::string& Name, EDasMemberUse Use) const { return false; }
    // Suffix of the view's own transport of this kind, or nullptr.
    virtual const char* GetCallBridge(EDasCallBridge Bridge) const { return nullptr; }
    // daScript handle annotation of a handle view, or nullptr.
    virtual const char* GetHandleName() const { return nullptr; }
};
