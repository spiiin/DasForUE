// Part of DasRuntime.cpp: daScript type annotations for object handles, math values and transport views.
// Included in place; declaration order matters.
struct FObjectHandleAnnotation : das::ManagedStructureAnnotation<FDasObjectHandle, false>
{
    explicit FObjectHandleAnnotation(das::ModuleLibrary& Library)
        : ManagedStructureAnnotation("UObjectHandle", Library, "FDasObjectHandle")
    {
    }
    virtual bool isLocal() const override { return true; }
    virtual bool canCopy() const override { return true; }
    virtual bool canMove() const override { return true; }
    virtual bool hasNonTrivialCtor() const override { return false; }
    virtual bool isPod() const override { return true; }
    virtual bool canBePlacedInContainer() const override { return true; }
};

struct FClassHandleAnnotation : das::ManagedStructureAnnotation<FDasClassHandle, false>
{
    explicit FClassHandleAnnotation(das::ModuleLibrary& L)
        : ManagedStructureAnnotation("UEClassHandle", L, "FDasClassHandle")
    {
    }
    virtual das::TypeDeclPtr makeValueType() const override
    {
        return new das::TypeDecl(das::Type::tUInt64, das::LineInfo());
    }
    virtual das::SimNode* simulateClone(das::Context& C, const das::LineInfo& At, das::SimNode* Left,
                                        das::SimNode* Right) const override
    {
        return C.code->makeNode<das::SimNode_Set<FDasClassHandle>>(At, Left, Right);
    }
    virtual bool isRefType() const override { return false; }
    virtual bool isLocal() const override { return true; }
    virtual bool canCopy() const override { return true; }
    virtual bool canMove() const override { return true; }
    virtual bool hasNonTrivialCtor() const override { return false; }
    virtual bool isPod() const override { return true; }
    virtual bool canBePlacedInContainer() const override { return true; }
};
struct FDSLEnumAnnotation : das::ManagedStructureAnnotation<FDasEnumValue, false>
{
    FDSLEnumAnnotation(das::ModuleLibrary& L) : ManagedStructureAnnotation("UEEnumValue", L, "FDasEnumValue") {}
    virtual das::TypeDeclPtr makeValueType() const override
    {
        static_assert(sizeof(FDasEnumValue) == sizeof(uint64));
        return new das::TypeDecl(das::Type::tUInt64, das::LineInfo());
    }
    virtual das::SimNode* simulateClone(das::Context& C, const das::LineInfo& At, das::SimNode* Left,
                                        das::SimNode* Right) const override
    {
        return C.code->makeNode<das::SimNode_Set<FDasEnumValue>>(At, Left, Right);
    }
    virtual bool isRefType() const override { return false; }
    virtual bool isLocal() const override { return true; }
    virtual bool canCopy() const override { return true; }
    virtual bool canMove() const override { return true; }
    virtual bool hasNonTrivialCtor() const override { return false; }
    virtual bool isPod() const override { return true; }
};
struct FDSLNameAnnotation : das::ManagedStructureAnnotation<FDasNameValue, false>
{
    FDSLNameAnnotation(das::ModuleLibrary& L) : ManagedStructureAnnotation("UENameValue", L, "FDasNameValue") {}
    // Reference-to-value conversion during table cloning/calls copies the owned token.
    virtual das::TypeDeclPtr makeValueType() const override
    {
        static_assert(sizeof(FDasNameValue) == sizeof(uint64));
        return new das::TypeDecl(das::Type::tUInt64, das::LineInfo());
    }
    virtual das::SimNode* simulateClone(das::Context& C, const das::LineInfo& At, das::SimNode* Left,
                                        das::SimNode* Right) const override
    {
        return C.code->makeNode<das::SimNode_Set<FDasNameValue>>(At, Left, Right);
    }
    virtual bool isRefType() const override { return false; }
    virtual bool isLocal() const override { return true; }
    virtual bool canCopy() const override { return true; }
    virtual bool canMove() const override { return true; }
    virtual bool hasNonTrivialCtor() const override { return false; }
    virtual bool isPod() const override { return true; }
};
// Ticks of FDateTime / FTimespan, held by value like int64.
template <class T> struct TDasTicksAnnotation : das::ManagedStructureAnnotation<T, false>
{
    TDasTicksAnnotation(const char* Name, das::ModuleLibrary& L, const char* CppName)
        : das::ManagedStructureAnnotation<T, false>(Name, L, CppName)
    {
    }
    virtual das::TypeDeclPtr makeValueType() const override
    {
        static_assert(sizeof(T) == sizeof(int64));
        return new das::TypeDecl(das::Type::tInt64, das::LineInfo());
    }
    virtual das::SimNode* simulateClone(das::Context& C, const das::LineInfo& At, das::SimNode* Left,
                                        das::SimNode* Right) const override
    {
        return C.code->makeNode<das::SimNode_Set<T>>(At, Left, Right);
    }
    virtual bool isRefType() const override { return false; }
    virtual bool isLocal() const override { return true; }
    virtual bool canCopy() const override { return true; }
    virtual bool canMove() const override { return true; }
    virtual bool hasNonTrivialCtor() const override { return false; }
    virtual bool isPod() const override { return true; }
    virtual bool canBePlacedInContainer() const override { return true; }
};
struct FDSLTextAnnotation : das::ManagedStructureAnnotation<FDasTextValue, false>
{
    FDSLTextAnnotation(das::ModuleLibrary& L) : ManagedStructureAnnotation("UETextValue", L, "FDasTextValue") {}
    // Reference-to-value conversion during table cloning/calls copies the owned token.
    virtual das::TypeDeclPtr makeValueType() const override
    {
        static_assert(sizeof(FDasTextValue) == sizeof(uint64));
        return new das::TypeDecl(das::Type::tUInt64, das::LineInfo());
    }
    virtual das::SimNode* simulateClone(das::Context& C, const das::LineInfo& At, das::SimNode* Left,
                                        das::SimNode* Right) const override
    {
        return C.code->makeNode<das::SimNode_Set<FDasTextValue>>(At, Left, Right);
    }
    virtual bool isRefType() const override { return false; }
    virtual bool isLocal() const override { return true; }
    virtual bool canCopy() const override { return true; }
    virtual bool canMove() const override { return true; }
    virtual bool hasNonTrivialCtor() const override { return false; }
    virtual bool isPod() const override { return true; }
};
// Fields are registered with their C++ member names too (addField("x", "X")): generated AOT code reads the member.
template <class T> struct FMathValueAnnotation : das::ManagedStructureAnnotation<T, false>
{
    FMathValueAnnotation(const char* Name, das::ModuleLibrary& L, const char* Cpp)
        : das::ManagedStructureAnnotation<T, false>(Name, L, Cpp)
    {
    }
    virtual bool isLocal() const override { return true; }
    virtual bool canCopy() const override { return true; }
    virtual bool canMove() const override { return true; }
    virtual bool hasNonTrivialCtor() const override { return false; }
};
struct FDSLVector2DAnnotation : FMathValueAnnotation<FVector2D>
{
    FDSLVector2DAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UEVector2D", L, "FVector2D")
    {
        addField<DAS_BIND_MANAGED_FIELD(X)>("x", "X");
        addField<DAS_BIND_MANAGED_FIELD(Y)>("y", "Y");
    }
};
struct FDSLVectorAnnotation : FMathValueAnnotation<FVector>
{
    FDSLVectorAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UEVector", L, "FVector")
    {
        addField<DAS_BIND_MANAGED_FIELD(X)>("x", "X");
        addField<DAS_BIND_MANAGED_FIELD(Y)>("y", "Y");
        addField<DAS_BIND_MANAGED_FIELD(Z)>("z", "Z");
    }
};
struct FDSLRotatorAnnotation : FMathValueAnnotation<FRotator>
{
    FDSLRotatorAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UERotator", L, "FRotator")
    {
        addField<DAS_BIND_MANAGED_FIELD(Pitch)>("pitch", "Pitch");
        addField<DAS_BIND_MANAGED_FIELD(Yaw)>("yaw", "Yaw");
        addField<DAS_BIND_MANAGED_FIELD(Roll)>("roll", "Roll");
    }
};
struct FDSLQuatAnnotation : FMathValueAnnotation<FQuat>
{
    FDSLQuatAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UEQuat", L, "FQuat")
    {
        addField<DAS_BIND_MANAGED_FIELD(X)>("x", "X");
        addField<DAS_BIND_MANAGED_FIELD(Y)>("y", "Y");
        addField<DAS_BIND_MANAGED_FIELD(Z)>("z", "Z");
        addField<DAS_BIND_MANAGED_FIELD(W)>("w", "W");
    }
};
struct FDSLLinearColorAnnotation : FMathValueAnnotation<FLinearColor>
{
    FDSLLinearColorAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UELinearColor", L, "FLinearColor")
    {
        addField<DAS_BIND_MANAGED_FIELD(R)>("r", "R");
        addField<DAS_BIND_MANAGED_FIELD(G)>("g", "G");
        addField<DAS_BIND_MANAGED_FIELD(B)>("b", "B");
        addField<DAS_BIND_MANAGED_FIELD(A)>("a", "A");
    }
};
struct FDSLColorAnnotation : FMathValueAnnotation<FColor>
{
    FDSLColorAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UEColor", L, "FColor")
    {
        addField<DAS_BIND_MANAGED_FIELD(R)>("r", "R");
        addField<DAS_BIND_MANAGED_FIELD(G)>("g", "G");
        addField<DAS_BIND_MANAGED_FIELD(B)>("b", "B");
        addField<DAS_BIND_MANAGED_FIELD(A)>("a", "A");
    }
};
struct FTraceHitAnnotation : FMathValueAnnotation<FDasTraceHit>
{
    FTraceHitAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UETraceHit", L, "FDasTraceHit")
    {
        addField<DAS_BIND_MANAGED_FIELD(BlockingHit)>("blocking_hit", "BlockingHit");
        addField<DAS_BIND_MANAGED_FIELD(StartPenetrating)>("start_penetrating", "StartPenetrating");
        addField<DAS_BIND_MANAGED_FIELD(Distance)>("distance", "Distance");
        addField<DAS_BIND_MANAGED_FIELD(Time)>("time", "Time");
        addField<DAS_BIND_MANAGED_FIELD(Location)>("location", "Location");
        addField<DAS_BIND_MANAGED_FIELD(ImpactPoint)>("impact_point", "ImpactPoint");
        addField<DAS_BIND_MANAGED_FIELD(Normal)>("normal", "Normal");
        addField<DAS_BIND_MANAGED_FIELD(ImpactNormal)>("impact_normal", "ImpactNormal");
        addField<DAS_BIND_MANAGED_FIELD(FaceIndex)>("face_index", "FaceIndex");
        addField<DAS_BIND_MANAGED_FIELD(TraceStart)>("trace_start", "TraceStart");
        addField<DAS_BIND_MANAGED_FIELD(TraceEnd)>("trace_end", "TraceEnd");
        addField<DAS_BIND_MANAGED_FIELD(PenetrationDepth)>("penetration_depth", "PenetrationDepth");
        addField<DAS_BIND_MANAGED_FIELD(Item)>("item", "Item");
        addField<DAS_BIND_MANAGED_FIELD(MyItem)>("my_item", "MyItem");
        addField<DAS_BIND_MANAGED_FIELD(ElementIndex)>("element_index", "ElementIndex");
    }
};
struct FOverlapResultAnnotation : FMathValueAnnotation<FDasOverlapResult>
{
    FOverlapResultAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UEOverlapResult", L, "FDasOverlapResult")
    {
        addField<DAS_BIND_MANAGED_FIELD(BlockingHit)>("blocking_hit", "BlockingHit");
        addField<DAS_BIND_MANAGED_FIELD(ItemIndex)>("item_index", "ItemIndex");
        addField<DAS_BIND_MANAGED_FIELD(MyItemIndex)>("my_item_index", "MyItemIndex");
    }
};
struct FReflectedValueAnnotation : FMathValueAnnotation<FDasStructValue>
{
    FReflectedValueAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UEStructValue", L, "FDasStructValue") {}
    virtual bool isPod() const override { return true; }
};
FDasCallHandle BeginCallbackFrame(const char*, das::Context*);
void StartCallbackFrameTimer(const char*, float, const char*, bool, const FDasCallHandle&, das::Context*);
void NextCallbackFrameTimer(const char*, const char*, const FDasCallHandle&, das::Context*);
#include "DasSoftRuntime.inl"
#include "DasDSL.inl"
#include "DasGameplayServices.inl"
#include "DasWidgetLayoutsRuntime.inl"
#include "DasImageSoftBrushRuntime.inl"
#include "DasInputActionValue.inl"
#include "DasInputEventsRuntime.inl"
#include "DasGeometryRuntime.inl"
#include "DasWidgetEventsRuntime.inl"
#include "DasInputNavigation.inl"
#include "DasEventReplyCalls.inl"
#include "DasDragRuntime.inl"
#include "DasPaintRuntime.inl"
#include "DasWidgetAnimationRuntime.inl"
#include "DasHitResultTransport.inl"
#include "DasConstraintTransport.inl"
struct FHitResultValueAnnotation : FMathValueAnnotation<FDasHitResultValue>
{
    FHitResultValueAnnotation(das::ModuleLibrary& L) : FMathValueAnnotation("UEHitResult", L, "FDasHitResultValue") {}
    virtual bool isPod() const override { return true; }
};

#include "DasAsyncPhysicsVMExterns.inl"
