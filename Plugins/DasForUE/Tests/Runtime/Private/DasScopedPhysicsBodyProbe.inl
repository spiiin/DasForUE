#include "DasScopedPhysicsBodyProbe.h"
namespace
{
class FScopedBodyProbe final : public IDasScopedPhysicsBodyProbe
{
    uint64 Previous = 0;

public:
    FDasPhysicsBodyBindings A, B;
    explicit FScopedBodyProbe(UObject* Owner) : A(Owner), B(Owner) {}
    virtual bool VerifyIsolation(const FDasAsyncPhysicsStep& Step, FString& Error) override
    {
        FDasPhysicsBodyScope SA(A, Step), SB(B, Step);
        const auto TA = SA.Acquire(0, Error);
        if (!TA)
            return false;
        const auto TB = SB.Acquire(0, Error);
        if (!TB)
            return false;
        if (TA == TB || SA.AddForce(TB, FVector(1, 0, 0), false, Error) ||
            SB.AddForce(TA, FVector(1, 0, 0), false, Error) || SA.AddForce(0, FVector(1, 0, 0), false, Error) ||
            (Previous && SA.AddForce(Previous, FVector(1, 0, 0), false, Error)))
        {
            Error = TEXT("foreign/null/expired native body capability unexpectedly accepted");
            return false;
        }
        Previous = TA;
        Error.Reset();
        return true; // no commit; no force is applied
    }
};
}
TUniquePtr<IDasScopedPhysicsBodyProbe>
PrepareDasScopedPhysicsBodyProbeForTests(UObject* Owner, UPrimitiveComponent* Body, FString& Error)
{
    auto P = MakeUnique<FScopedBodyProbe>(Owner);
    if (!P->A.Bind(0, Body, Error) || !P->B.Bind(0, Body, Error))
        return nullptr;
    return P;
}
