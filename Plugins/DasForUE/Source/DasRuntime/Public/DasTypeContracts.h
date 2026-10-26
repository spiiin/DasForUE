#pragma once
#include "DasTypeAdapters.h"

// These are gates at named compiler/runtime boundaries, not promises that an entire declaration
// or arbitrary source expression will compile. Nominal resolution and whole-signature checks remain explicit.
enum class EDasTypeUse : uint8
{
    AuthoredInput,
    AuthoredOut,
    AuthoredRef,
    AuthoredReturn,
    InterfaceInput,
    InterfaceOut,
    InterfaceRef,
    InterfaceReturn,
    DelegateInput,
    DelegateOut,
    DelegateRef,
    DelegateReturn,
    MulticastInput,
    MulticastRef,
    QueuedEvent,
    Timer,
    PropertyDefault,
    StructMemberDefault,
    ParameterDefault,
    ComputedDefaultLeaf,
    SynchronousCallback,
    NativeCall
};
struct FDasTypeUseInfo
{
    EDasTypeUse Use;
    const TCHAR* Name;
    const TCHAR* Boundary;
};
enum class EDasTypeSupport : uint8
{
    Supported,
    Unsupported,
    RequiresContext
};
struct FDasTypeUseResult
{
    EDasTypeSupport State;
    FString Reason;
    bool IsSupported() const { return State == EDasTypeSupport::Supported; }
};
DASRUNTIME_API TConstArrayView<FDasTypeUseInfo> DasTypeUses();
DASRUNTIME_API FDasTypeUseResult QueryDasTypeUse(const FDasDeclaredType& Type, EDasTypeUse Use);
DASRUNTIME_API bool ValidateDasTypeUse(const FDasDeclaredType& Type, EDasTypeUse Use, FString& Error);
// The existing lowering ABI, shared by authoring and support diagnostics. Containers resolve their
// canonical carrier on the game thread; this is not an AST/source-type inference operation.
DASRUNTIME_API EDasMethodSignature DasAuthoredTypeSignature(const FDasDeclaredType& Type);
enum class EDasSignatureUse : uint8
{
    Method,
    Interface,
    Delegate,
    Multicast
};
DASRUNTIME_API bool ValidateDasSignatureContract(const FDasDeclaredMethod& Method, EDasSignatureUse Use,
                                                 FString& Error);
// Declaration shape validation deliberately permits not-yet-materialized authored class identities.
DASRUNTIME_API bool ValidateDasDeclarationType(const FDasDeclaredType& Type, const TSet<FString>& Classes,
                                               bool AllowVoid, FString& Error);
// Value checks follow the type gate. Empty means absent, not an explicit null/zero initializer.
DASRUNTIME_API bool ValidateDasAuthoredDefault(const FDasDeclaredType& Type, const FDasDeclaredLiteral& Value,
                                               EDasTypeUse Use, FString& Error);
// Context-free NativeCall queries return RequiresContext. Only this concrete gate can admit an
// opaque transport, retaining the adapter's exact function, direction and depth restrictions.
DASRUNTIME_API bool ValidateDasNativeCallContract(const FProperty* Property, bool Input, bool Output, int32 Depth,
                                                  FString& Error);

DASRUNTIME_API bool ValidateDasLoweredSignatureType(EDasMethodSignature Kind, const FString& Path, EDasTypeUse Use,
                                                    FString& Error);

// Type-role admission for specialized synchronous callback frames. The caller must still validate
// the complete callback contract and establish the transport's live/detached lifetime scope.
DASRUNTIME_API bool DasSynchronousCallbackRole(const FString& Path, EDasNativeTypeRole Role);

DASRUNTIME_API bool ValidateDasParameterDefaults(const FDasDeclaredMethod& Method, bool AllowDefaults, FString& Error);
