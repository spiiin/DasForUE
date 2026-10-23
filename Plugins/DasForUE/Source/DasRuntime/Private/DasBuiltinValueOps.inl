// Included in DasRuntime.cpp. Operators reuse native UE value semantics and the common extern guard.
template <class T> const T& BuiltinValid(const T& V, das::Context* C)
{
    if (V.ContainsNaN())
        C->throw_error("built-in math requires finite values");
    if constexpr (std::is_same_v<T, FTransform>)
        if (!V.IsRotationNormalized())
            C->throw_error("transform rotation must be normalized");
    return V;
}
double BuiltinFinite(double V, das::Context* C)
{
    if (!FMath::IsFinite(V))
        C->throw_error("built-in math requires finite values");
    return V;
}
double BuiltinTolerance(double V, das::Context* C)
{
    if (BuiltinFinite(V, C) < 0)
        C->throw_error("tolerance must be non-negative");
    return V;
}
template <class T> T BuiltinAdd(const T& A, const T& B, das::Context* C)
{
    return BuiltinValid(BuiltinValid(A, C) + BuiltinValid(B, C), C);
}
template <class T> T BuiltinSubtract(const T& A, const T& B, das::Context* C)
{
    return BuiltinValid(BuiltinValid(A, C) - BuiltinValid(B, C), C);
}
template <class T> T BuiltinNegate(const T& A, das::Context* C)
{
    if constexpr (std::is_same_v<T, FRotator>)
        return BuiltinValid(BuiltinValid(A, C) * -1.0, C);
    else
        return BuiltinValid(-BuiltinValid(A, C), C);
}
template <class T> T BuiltinMultiply(const T& A, double B, das::Context* C)
{
    return BuiltinValid(BuiltinValid(A, C) * BuiltinFinite(B, C), C);
}
template <class T> T BuiltinMultiplyLeft(double A, const T& B, das::Context* C)
{
    return BuiltinMultiply(B, A, C);
}
template <class T> T BuiltinDivide(const T& A, double B, das::Context* C)
{
    BuiltinFinite(B, C);
    if (B == 0)
        C->throw_error("vector division by zero");
    return BuiltinValid(BuiltinValid(A, C) / B, C);
}
template <class T> bool BuiltinEqual(const T& A, const T& B, das::Context* C)
{
    return BuiltinValid(A, C) == BuiltinValid(B, C);
}
template <class T> bool BuiltinNotEqual(const T& A, const T& B, das::Context* C)
{
    return !BuiltinEqual(A, B, C);
}
template <class T> bool BuiltinNearly(const T& A, const T& B, double Tolerance, das::Context* C)
{
    return BuiltinValid(A, C).Equals(BuiltinValid(B, C), BuiltinTolerance(Tolerance, C));
}
template <class T> void BuiltinAddAssign(T& A, const T& B, das::Context* C)
{
    A = BuiltinAdd(A, B, C);
}
template <class T> void BuiltinSubtractAssign(T& A, const T& B, das::Context* C)
{
    A = BuiltinSubtract(A, B, C);
}
template <class T> void BuiltinMultiplyAssign(T& A, double B, das::Context* C)
{
    A = BuiltinMultiply(A, B, C);
}
template <class T> void BuiltinDivideAssign(T& A, double B, das::Context* C)
{
    A = BuiltinDivide(A, B, C);
}
template <class T> double BuiltinLength(const T& A, das::Context* C)
{
    return BuiltinFinite(BuiltinValid(A, C).Size(), C);
}
template <class T> double BuiltinLengthSquared(const T& A, das::Context* C)
{
    return BuiltinFinite(BuiltinValid(A, C).SizeSquared(), C);
}
template <class T> double BuiltinDot(const T& A, const T& B, das::Context* C)
{
    return BuiltinFinite(T::DotProduct(BuiltinValid(A, C), BuiltinValid(B, C)), C);
}
template <class T> double BuiltinDistance(const T& A, const T& B, das::Context* C)
{
    return BuiltinLength(BuiltinSubtract(A, B, C), C);
}
template <class T> T BuiltinNormalized(const T& A, double ToleranceSquared, das::Context* C)
{
    if (BuiltinTolerance(ToleranceSquared, C) == 0)
        C->throw_error("normalization tolerance squared must be positive");
    BuiltinLengthSquared(A, C);
    return BuiltinValid(A.GetSafeNormal(ToleranceSquared), C);
}
FVector BuiltinCross(const FVector& A, const FVector& B, das::Context* C)
{
    return BuiltinValid(FVector::CrossProduct(BuiltinValid(A, C), BuiltinValid(B, C)), C);
}
double BuiltinCross2D(const FVector2D& A, const FVector2D& B, das::Context* C)
{
    return BuiltinFinite(FVector2D::CrossProduct(BuiltinValid(A, C), BuiltinValid(B, C)), C);
}
FRotator BuiltinRotationNormalized(const FRotator& R, das::Context* C)
{
    return BuiltinValid(BuiltinValid(R, C).GetNormalized(), C);
}
FRotator BuiltinComposeRotation(const FRotator& First, const FRotator& Then, das::Context* C)
{
    return BuiltinValid((BuiltinValid(Then, C).Quaternion() * BuiltinValid(First, C).Quaternion()).Rotator(), C);
}
FVector BuiltinRotate(const FRotator& R, const FVector& V, das::Context* C)
{
    return BuiltinValid(BuiltinValid(R, C).RotateVector(BuiltinValid(V, C)), C);
}
FVector BuiltinUnrotate(const FRotator& R, const FVector& V, das::Context* C)
{
    return BuiltinValid(BuiltinValid(R, C).UnrotateVector(BuiltinValid(V, C)), C);
}
FTransform BuiltinComposeTransform(const FTransform& A, const FTransform& B, das::Context* C)
{
    return BuiltinValid(BuiltinValid(A, C) * BuiltinValid(B, C), C);
}
void BuiltinComposeTransformAssign(FTransform& A, const FTransform& B, das::Context* C)
{
    A = BuiltinComposeTransform(A, B, C);
}
void BuiltinInvertible(const FTransform& T, das::Context* C)
{
    const FVector S = BuiltinValid(T, C).GetScale3D();
    if (FMath::Abs(S.X) <= UE_SMALL_NUMBER || FMath::Abs(S.Y) <= UE_SMALL_NUMBER || FMath::Abs(S.Z) <= UE_SMALL_NUMBER)
        C->throw_error("inverse transform requires scale magnitude greater than UE_SMALL_NUMBER on every axis");
}
FTransform BuiltinInverse(const FTransform& T, das::Context* C)
{
    BuiltinInvertible(T, C);
    return BuiltinValid(T.Inverse(), C);
}
FTransform BuiltinRelative(const FTransform& T, const FTransform& Parent, das::Context* C)
{
    BuiltinInvertible(Parent, C);
    return BuiltinValid(BuiltinValid(T, C).GetRelativeTransform(Parent), C);
}
FVector BuiltinTransformVector(const FTransform& T, const FVector& V, das::Context* C)
{
    return BuiltinValid(BuiltinValid(T, C).TransformVector(BuiltinValid(V, C)), C);
}
FVector BuiltinTransformDirection(const FTransform& T, const FVector& V, das::Context* C)
{
    return BuiltinValid(BuiltinValid(T, C).TransformVectorNoScale(BuiltinValid(V, C)), C);
}
FVector BuiltinInversePosition(const FTransform& T, const FVector& V, das::Context* C)
{
    BuiltinInvertible(T, C);
    return BuiltinValid(T.InverseTransformPosition(BuiltinValid(V, C)), C);
}
FVector BuiltinInverseVector(const FTransform& T, const FVector& V, das::Context* C)
{
    BuiltinInvertible(T, C);
    return BuiltinValid(T.InverseTransformVector(BuiltinValid(V, C)), C);
}
bool BuiltinNamesNotEqual(FDasNameValue A, FDasNameValue B, das::Context* C)
{
    return !DSLNamesEqual(A, B, C);
}

// Quaternion conversions preserve Unreal's double-precision rotation convention.
// Never silently normalize a caller's quaternion or reinterpret Euler components.
const FQuat& BuiltinRotationQuat(const FQuat& Q, das::Context* C)
{
    BuiltinValid(Q, C);
    if (!Q.IsNormalized())
        C->throw_error("quaternion rotation must be normalized");
    return Q;
}
FQuat BuiltinQuatFromRotator(const FRotator& R, das::Context* C)
{
    return BuiltinRotationQuat(BuiltinValid(R, C).Quaternion(), C);
}
FRotator BuiltinRotatorFromQuat(const FQuat& Q, das::Context* C)
{
    return BuiltinValid(BuiltinRotationQuat(Q, C).Rotator(), C);
}
FQuat BuiltinQuat(double X, double Y, double Z, double W, das::Context* C)
{
    return BuiltinRotationQuat(FQuat(X, Y, Z, W), C);
}
FQuat BuiltinQuatIdentity()
{
    return FQuat::Identity;
}
