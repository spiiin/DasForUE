#pragma once
#include "CoreMinimal.h"
#include "DasAuthoring.h"

// One table of value kinds (DasValueKinds.cpp). EDasTypeKind is the canonical kind; EDasScalarKind
// (actor field schema) and EDasMethodSignature (call contracts) keep their persisted numbering and are
// mapped from it here instead of in per-call-site string chains.
struct FDasValueKindRow
{
    EDasTypeKind Kind;
    const TCHAR* Name;
    // Actor field kind; containers use their carrier struct (callers check DasContainerCarrier).
    TOptional<EDasScalarKind> Field;
    // Method signature of a scalar parameter/result; containers travel as carrier structs.
    TOptional<EDasMethodSignature> Signature;
};
DASRUNTIME_API TConstArrayView<FDasValueKindRow> DasValueKinds();
DASRUNTIME_API TOptional<EDasScalarKind> DasFieldKind(EDasTypeKind Kind);
DASRUNTIME_API TOptional<EDasMethodSignature> DasSignatureKind(EDasTypeKind Kind);
// Inverse mappings; Vector/Rotator/Transform signatures are structs, Frame has no value kind.
DASRUNTIME_API EDasTypeKind DasTypeKindOf(EDasScalarKind Kind);
DASRUNTIME_API EDasTypeKind DasTypeKindOf(EDasMethodSignature Kind);
