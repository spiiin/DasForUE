// Native module ue_math: FMath (interpolation, easing, comparisons, ranges, angles, random values, geometry, noise)
// and the parts of FVector / FRotator / FQuat / FTransform / FVector2D that ue_foundation does not bind (constants,
// checks, conversions). Functions of a value type take it first, so scripts write v.equals(o), r.vector().
// Scalars are double in scripts; the engine's float parameters (delta time, alpha, ...) are converted here.
// Names that daScript math already has (lerp, clamp, min, max, abs, sqrt, trigonometry) are not bound again.
// Built on the public native binds API (DasNativeBinds.h).
#include "DasNativeBinds.h"
#include "Math/RotationMatrix.h"

namespace DasNativeMath
{
#define DAS_MATH_FN(Name, Params, Body)                                                                                \
    constexpr auto Name = +[] Params                                                                                   \
    {                                                                                                                  \
        return Body;                                                                                                   \
    };
// Scalar results are double in scripts, whatever the engine returns (some return float).
#define DAS_MATH_SCALAR(Name, Params, Body)                                                                            \
    constexpr auto Name = +[] Params                                                                                   \
    {                                                                                                                  \
        return double(Body);                                                                                           \
    };

// Interpolation: the frame step, as in the engine (delta time and speed are seconds and units per second).
DAS_MATH_SCALAR(FInterpTo, (double Current, double Target, double Dt, double Speed),
                double(FMath::FInterpTo(Current, Target, Dt, Speed)))
DAS_MATH_SCALAR(FInterpConstantTo, (double Current, double Target, double Dt, double Speed),
                double(FMath::FInterpConstantTo(Current, Target, Dt, Speed)))
DAS_MATH_FN(VInterpTo, (const FVector& Current, const FVector& Target, double Dt, double Speed),
            FMath::VInterpTo(Current, Target, float(Dt), float(Speed)))
DAS_MATH_FN(VInterpConstantTo, (const FVector& Current, const FVector& Target, double Dt, double Speed),
            FMath::VInterpConstantTo(Current, Target, float(Dt), float(Speed)))
DAS_MATH_FN(VInterpNormalRotationTo, (const FVector& Current, const FVector& Target, double Dt, double Degrees),
            FMath::VInterpNormalRotationTo(Current, Target, float(Dt), float(Degrees)))
DAS_MATH_FN(Vector2DInterpTo, (const FVector2D& Current, const FVector2D& Target, double Dt, double Speed),
            FMath::Vector2DInterpTo(Current, Target, float(Dt), float(Speed)))
DAS_MATH_FN(Vector2DInterpConstantTo, (const FVector2D& Current, const FVector2D& Target, double Dt, double Speed),
            FMath::Vector2DInterpConstantTo(Current, Target, float(Dt), float(Speed)))
DAS_MATH_FN(RInterpTo, (const FRotator& Current, const FRotator& Target, double Dt, double Speed),
            FMath::RInterpTo(Current, Target, float(Dt), float(Speed)))
DAS_MATH_FN(RInterpConstantTo, (const FRotator& Current, const FRotator& Target, double Dt, double Speed),
            FMath::RInterpConstantTo(Current, Target, float(Dt), float(Speed)))
DAS_MATH_FN(QInterpTo, (const FQuat& Current, const FQuat& Target, double Dt, double Speed),
            FMath::QInterpTo(Current, Target, float(Dt), float(Speed)))
DAS_MATH_FN(QInterpConstantTo, (const FQuat& Current, const FQuat& Target, double Dt, double Speed),
            FMath::QInterpConstantTo(Current, Target, float(Dt), float(Speed)))
DAS_MATH_FN(CInterpTo, (const FLinearColor& Current, const FLinearColor& Target, double Dt, double Speed),
            FMath::CInterpTo(Current, Target, float(Dt), float(Speed)))

// Easing between A and B by Alpha in 0..1.
DAS_MATH_SCALAR(EaseIn, (double A, double B, double Alpha, double Exp),
                FMath::InterpEaseIn(A, B, float(Alpha), float(Exp)))
DAS_MATH_SCALAR(EaseOut, (double A, double B, double Alpha, double Exp),
                FMath::InterpEaseOut(A, B, float(Alpha), float(Exp)))
DAS_MATH_SCALAR(EaseInOut, (double A, double B, double Alpha, double Exp),
                FMath::InterpEaseInOut(A, B, float(Alpha), float(Exp)))
DAS_MATH_SCALAR(SinusoidalIn, (double A, double B, double Alpha), FMath::InterpSinIn(A, B, float(Alpha)))
DAS_MATH_SCALAR(SinusoidalOut, (double A, double B, double Alpha), FMath::InterpSinOut(A, B, float(Alpha)))
DAS_MATH_SCALAR(SinusoidalInOut, (double A, double B, double Alpha), FMath::InterpSinInOut(A, B, float(Alpha)))
DAS_MATH_SCALAR(ExpoIn, (double A, double B, double Alpha), FMath::InterpExpoIn(A, B, float(Alpha)))
DAS_MATH_SCALAR(ExpoOut, (double A, double B, double Alpha), FMath::InterpExpoOut(A, B, float(Alpha)))
DAS_MATH_SCALAR(ExpoInOut, (double A, double B, double Alpha), FMath::InterpExpoInOut(A, B, float(Alpha)))
DAS_MATH_SCALAR(CircularIn, (double A, double B, double Alpha), FMath::InterpCircularIn(A, B, float(Alpha)))
DAS_MATH_SCALAR(CircularOut, (double A, double B, double Alpha), FMath::InterpCircularOut(A, B, float(Alpha)))
DAS_MATH_SCALAR(CircularInOut, (double A, double B, double Alpha), FMath::InterpCircularInOut(A, B, float(Alpha)))

// Comparisons and ranges.
DAS_MATH_FN(IsNearlyEqual, (double A, double B), FMath::IsNearlyEqual(A, B))
DAS_MATH_FN(IsNearlyEqualTolerance, (double A, double B, double Tolerance), FMath::IsNearlyEqual(A, B, Tolerance))
DAS_MATH_FN(IsNearlyZero, (double A), FMath::IsNearlyZero(A))
DAS_MATH_FN(IsNearlyZeroTolerance, (double A, double Tolerance), FMath::IsNearlyZero(A, Tolerance))
DAS_MATH_FN(IsWithin, (double V, double Min, double Max), FMath::IsWithin(V, Min, Max))
DAS_MATH_FN(IsWithinInclusive, (double V, double Min, double Max), FMath::IsWithinInclusive(V, Min, Max))
DAS_MATH_SCALAR(MappedClamped, (const FVector2D& In, const FVector2D& Out, double V),
                double(FMath::GetMappedRangeValueClamped(In, Out, V)))
DAS_MATH_SCALAR(MappedUnclamped, (const FVector2D& In, const FVector2D& Out, double V),
                double(FMath::GetMappedRangeValueUnclamped(In, Out, V)))
// As UKismetMathLibrary::NormalizeToRange: an empty range gives 0 below it and 1 from it on.
DAS_MATH_SCALAR(NormalizeToRange, (double V, double Min, double Max),
                Min == Max ? (V < Min ? 0.0 : 1.0) : (V - Min) / (Max - Min))

// Angles (degrees unless named radians) and rounding.
DAS_MATH_SCALAR(FindDeltaAngleDegrees, (double A, double B), double(FMath::FindDeltaAngleDegrees(A, B)))
DAS_MATH_SCALAR(UnwindDegrees, (double A), FMath::UnwindDegrees(A))
DAS_MATH_SCALAR(ClampAngle, (double A, double Min, double Max), FMath::ClampAngle(A, Min, Max))
DAS_MATH_SCALAR(DegreesToRadians, (double A), FMath::DegreesToRadians(A))
DAS_MATH_SCALAR(RadiansToDegrees, (double A), FMath::RadiansToDegrees(A))
DAS_MATH_SCALAR(GridSnap, (double Location, double Grid), FMath::GridSnap(Location, Grid))
DAS_MATH_SCALAR(RoundFromZero, (double A), FMath::RoundFromZero(A))
DAS_MATH_SCALAR(MakePulsatingValue, (double Time, double PulsesPerSecond, double Phase),
                double(FMath::MakePulsatingValue(Time, float(PulsesPerSecond), float(Phase))))

// Random values (the engine's global stream).
DAS_MATH_FN(RandRangeInt, (int32 Min, int32 Max), FMath::RandRange(Min, Max))
DAS_MATH_SCALAR(RandRangeDouble, (double Min, double Max), FMath::FRandRange(Min, Max))
DAS_MATH_SCALAR(FRand, (), double(FMath::FRand()))
DAS_MATH_FN(RandBool, (), FMath::RandBool())
DAS_MATH_FN(VRand, (), FMath::VRand())
DAS_MATH_FN(VRandCone, (const FVector& Dir, double HalfAngleRadians), FMath::VRandCone(Dir, float(HalfAngleRadians)))
DAS_MATH_FN(RandPointInCircle, (double Radius), FMath::RandPointInCircle(float(Radius)))
DAS_MATH_FN(RandomPointInBoundingBox, (const FVector& Center, const FVector& HalfSize),
            Center + FMath::RandPointInBox(FBox(-HalfSize, HalfSize)))
// As UKismetMathLibrary::RandomRotator.
DAS_MATH_FN(RandomRotator, (bool bRoll),
            FRotator(FMath::FRand() * 360.0, FMath::FRand() * 360.0, bRoll ? FMath::FRand() * 360.0 : 0.0))

// Geometry.
DAS_MATH_FN(ClosestPointOnLine, (const FVector& Start, const FVector& End, const FVector& Point),
            FMath::ClosestPointOnLine(Start, End, Point))
DAS_MATH_FN(ClosestPointOnInfiniteLine, (const FVector& Start, const FVector& End, const FVector& Point),
            FMath::ClosestPointOnInfiniteLine(Start, End, Point))
DAS_MATH_SCALAR(PointDistToSegment, (const FVector& Point, const FVector& Start, const FVector& End),
                double(FMath::PointDistToSegment(Point, Start, End)))
DAS_MATH_FN(LinePlaneIntersection, (const FVector& P1, const FVector& P2, const FVector& Origin, const FVector& Normal),
            FMath::LinePlaneIntersection(P1, P2, Origin, Normal))
DAS_MATH_FN(LineSphereIntersection,
            (const FVector& Start, const FVector& Dir, double Length, const FVector& Origin, double Radius),
            FMath::LineSphereIntersection(Start, Dir, Length, Origin, Radius))

// Noise in -1..1.
DAS_MATH_SCALAR(PerlinNoise1D, (double V), double(FMath::PerlinNoise1D(float(V))))
DAS_MATH_SCALAR(PerlinNoise2D, (const FVector2D& V), double(FMath::PerlinNoise2D(V)))
DAS_MATH_SCALAR(PerlinNoise3D, (const FVector& V), double(FMath::PerlinNoise3D(V)))

// FVector.
DAS_MATH_FN(ZeroVector, (), FVector::ZeroVector)
DAS_MATH_FN(OneVector, (), FVector::OneVector)
DAS_MATH_FN(UpVector, (), FVector::UpVector)
DAS_MATH_FN(DownVector, (), FVector::DownVector)
DAS_MATH_FN(ForwardVector, (), FVector::ForwardVector)
DAS_MATH_FN(BackwardVector, (), FVector::BackwardVector)
DAS_MATH_FN(RightVector, (), FVector::RightVector)
DAS_MATH_FN(LeftVector, (), FVector::LeftVector)
DAS_MATH_FN(VectorEquals, (const FVector& A, const FVector& B), A.Equals(B))
DAS_MATH_FN(VectorEqualsTolerance, (const FVector& A, const FVector& B, double Tolerance), A.Equals(B, Tolerance))
DAS_MATH_FN(VectorIsZero, (const FVector& V), V.IsZero())
DAS_MATH_FN(VectorIsNearlyZero, (const FVector& V, double Tolerance), V.IsNearlyZero(Tolerance))
DAS_MATH_FN(VectorIsNormalized, (const FVector& V), V.IsNormalized())
DAS_MATH_FN(VectorIsUnit, (const FVector& V, double Tolerance), V.IsUnit(Tolerance))
DAS_MATH_FN(VectorContainsNaN, (const FVector& V), V.ContainsNaN())
DAS_MATH_FN(GetAbs, (const FVector& V), V.GetAbs())
DAS_MATH_SCALAR(GetMax, (const FVector& V), V.GetMax())
DAS_MATH_SCALAR(GetMin, (const FVector& V), V.GetMin())
DAS_MATH_SCALAR(GetAbsMax, (const FVector& V), V.GetAbsMax())
DAS_MATH_FN(ComponentMin, (const FVector& A, const FVector& B), A.ComponentMin(B))
DAS_MATH_FN(ComponentMax, (const FVector& A, const FVector& B), A.ComponentMax(B))
DAS_MATH_FN(GetSignVector, (const FVector& V), V.GetSignVector())
DAS_MATH_FN(Reciprocal, (const FVector& V), V.Reciprocal())
DAS_MATH_SCALAR(Size2D, (const FVector& V), V.Size2D())
DAS_MATH_SCALAR(SizeSquared2D, (const FVector& V), V.SizeSquared2D())
DAS_MATH_SCALAR(Dist2D, (const FVector& A, const FVector& B), FVector::Dist2D(A, B))
DAS_MATH_SCALAR(DistSquared, (const FVector& A, const FVector& B), FVector::DistSquared(A, B))
DAS_MATH_FN(GetSafeNormal2D, (const FVector& V), V.GetSafeNormal2D())
DAS_MATH_FN(GetClampedToSize, (const FVector& V, double Min, double Max), V.GetClampedToSize(Min, Max))
DAS_MATH_FN(GetClampedToMaxSize, (const FVector& V, double Max), V.GetClampedToMaxSize(Max))
DAS_MATH_FN(VectorGridSnap, (const FVector& V, double Grid), V.GridSnap(Grid))
DAS_MATH_FN(RotateAngleAxis, (const FVector& V, double Degrees, const FVector& Axis), V.RotateAngleAxis(Degrees, Axis))
DAS_MATH_FN(MirrorByVector, (const FVector& V, const FVector& Normal), V.MirrorByVector(Normal))
DAS_MATH_SCALAR(HeadingAngle, (const FVector& V), V.HeadingAngle())
DAS_MATH_SCALAR(CosineAngle2D, (const FVector& A, const FVector& B), A.CosineAngle2D(B))
DAS_MATH_FN(VectorRotation, (const FVector& V), V.Rotation())
DAS_MATH_FN(ToOrientationQuat, (const FVector& V), V.ToOrientationQuat())
DAS_MATH_FN(PointsAreNear, (const FVector& A, const FVector& B, double Distance),
            FVector::PointsAreNear(A, B, Distance))

// FRotator.
DAS_MATH_FN(ZeroRotator, (), FRotator::ZeroRotator)
DAS_MATH_FN(RotatorEquals, (const FRotator& A, const FRotator& B), A.Equals(B))
DAS_MATH_FN(RotatorEqualsTolerance, (const FRotator& A, const FRotator& B, double Tolerance), A.Equals(B, Tolerance))
DAS_MATH_FN(RotatorIsZero, (const FRotator& R), R.IsZero())
DAS_MATH_FN(RotatorIsNearlyZero, (const FRotator& R, double Tolerance), R.IsNearlyZero(Tolerance))
DAS_MATH_FN(GetInverse, (const FRotator& R), R.GetInverse())
DAS_MATH_FN(GetNormalized, (const FRotator& R), R.GetNormalized())
DAS_MATH_FN(GetDenormalized, (const FRotator& R), R.GetDenormalized())
DAS_MATH_FN(GetClamped, (const FRotator& R), R.Clamp())
DAS_MATH_SCALAR(NormalizeAxis, (double Angle), FRotator::NormalizeAxis(Angle))
DAS_MATH_SCALAR(ClampAxis, (double Angle), FRotator::ClampAxis(Angle))
DAS_MATH_FN(RotatorVector, (const FRotator& R), R.Vector())
DAS_MATH_FN(RotatorQuaternion, (const FRotator& R), R.Quaternion())
DAS_MATH_FN(RotatorEuler, (const FRotator& R), R.Euler())
DAS_MATH_SCALAR(GetManhattanDistance, (const FRotator& A, const FRotator& B), A.GetManhattanDistance(B))
DAS_MATH_FN(MakeFromX, (const FVector& X), FRotationMatrix::MakeFromX(X).Rotator())
DAS_MATH_FN(MakeFromY, (const FVector& Y), FRotationMatrix::MakeFromY(Y).Rotator())
DAS_MATH_FN(MakeFromZ, (const FVector& Z), FRotationMatrix::MakeFromZ(Z).Rotator())
DAS_MATH_FN(MakeFromXY, (const FVector& X, const FVector& Y), FRotationMatrix::MakeFromXY(X, Y).Rotator())
DAS_MATH_FN(MakeFromXZ, (const FVector& X, const FVector& Z), FRotationMatrix::MakeFromXZ(X, Z).Rotator())
DAS_MATH_FN(MakeFromYX, (const FVector& Y, const FVector& X), FRotationMatrix::MakeFromYX(Y, X).Rotator())
DAS_MATH_FN(MakeFromYZ, (const FVector& Y, const FVector& Z), FRotationMatrix::MakeFromYZ(Y, Z).Rotator())
DAS_MATH_FN(MakeFromZX, (const FVector& Z, const FVector& X), FRotationMatrix::MakeFromZX(Z, X).Rotator())
DAS_MATH_FN(MakeFromZY, (const FVector& Z, const FVector& Y), FRotationMatrix::MakeFromZY(Z, Y).Rotator())

// FQuat.
DAS_MATH_FN(QuatEquals, (const FQuat& A, const FQuat& B), A.Equals(B))
DAS_MATH_FN(QuatEqualsTolerance, (const FQuat& A, const FQuat& B, double Tolerance), A.Equals(B, Tolerance))
DAS_MATH_FN(QuatIsIdentity, (const FQuat& Q), Q.IsIdentity())
DAS_MATH_SCALAR(GetAngle, (const FQuat& Q), Q.GetAngle())
DAS_MATH_SCALAR(AngularDistance, (const FQuat& A, const FQuat& B), A.AngularDistance(B))
DAS_MATH_FN(QuatEuler, (const FQuat& Q), Q.Euler())
DAS_MATH_FN(MakeFromEuler, (const FVector& Euler), FQuat::MakeFromEuler(Euler))
DAS_MATH_FN(FindBetweenVectors, (const FVector& A, const FVector& B), FQuat::FindBetweenVectors(A, B))
DAS_MATH_FN(FindBetweenNormals, (const FVector& A, const FVector& B), FQuat::FindBetweenNormals(A, B))
DAS_MATH_FN(QuatForward, (const FQuat& Q), Q.GetForwardVector())
DAS_MATH_FN(QuatRight, (const FQuat& Q), Q.GetRightVector())
DAS_MATH_FN(QuatUp, (const FQuat& Q), Q.GetUpVector())
DAS_MATH_FN(GetAxisX, (const FQuat& Q), Q.GetAxisX())
DAS_MATH_FN(GetAxisY, (const FQuat& Q), Q.GetAxisY())
DAS_MATH_FN(GetAxisZ, (const FQuat& Q), Q.GetAxisZ())
DAS_MATH_FN(GetRotationAxis, (const FQuat& Q), Q.GetRotationAxis())
DAS_MATH_FN(QuatVector, (const FQuat& Q), Q.Vector())
DAS_MATH_FN(SlerpFullPath, (const FQuat& A, const FQuat& B, double Alpha), FQuat::SlerpFullPath(A, B, Alpha))
DAS_MATH_FN(FastLerp, (const FQuat& A, const FQuat& B, double Alpha), FQuat::FastLerp(A, B, Alpha))

// FTransform.
DAS_MATH_FN(TransformEquals, (const FTransform& A, const FTransform& B), A.Equals(B))
DAS_MATH_FN(TransformEqualsTolerance, (const FTransform& A, const FTransform& B, double Tolerance),
            A.Equals(B, Tolerance))
DAS_MATH_FN(EqualsNoScale, (const FTransform& A, const FTransform& B, double Tolerance), A.EqualsNoScale(B, Tolerance))
DAS_MATH_FN(GetRelativeTransform, (const FTransform& T, const FTransform& Other), T.GetRelativeTransform(Other))
DAS_MATH_SCALAR(GetMaximumAxisScale, (const FTransform& T), T.GetMaximumAxisScale())
DAS_MATH_SCALAR(GetMinimumAxisScale, (const FTransform& T), T.GetMinimumAxisScale())
DAS_MATH_FN(TransformPositionNoScale, (const FTransform& T, const FVector& V), T.TransformPositionNoScale(V))
DAS_MATH_FN(InverseTransformPositionNoScale, (const FTransform& T, const FVector& V),
            T.InverseTransformPositionNoScale(V))
DAS_MATH_FN(InverseTransformRotation, (const FTransform& T, const FQuat& Q), T.InverseTransformRotation(Q))
DAS_MATH_SCALAR(GetDeterminant, (const FTransform& T), T.GetDeterminant())
DAS_MATH_FN(TransformRotator, (const FTransform& T), T.Rotator())
DAS_MATH_FN(TransformContainsNaN, (const FTransform& T), T.ContainsNaN())
DAS_MATH_FN(TransformGetLocation, (const FTransform& T), T.GetLocation())
DAS_MATH_FN(GetScale3D, (const FTransform& T), T.GetScale3D())
DAS_MATH_FN(GetRelativeTransformReverse, (const FTransform& T, const FTransform& Other),
            T.GetRelativeTransformReverse(Other))
DAS_MATH_FN(TransformVectorNoScale, (const FTransform& T, const FVector& V), T.TransformVectorNoScale(V))
DAS_MATH_FN(InverseTransformVectorNoScale, (const FTransform& T, const FVector& V),
            T.InverseTransformVectorNoScale(V))
DAS_MATH_FN(TransformIsRotationNormalized, (const FTransform& T), T.IsRotationNormalized())
DAS_MATH_FN(SubtractTranslations, (const FTransform& A, const FTransform& B), FTransform::SubtractTranslations(A, B))
DAS_MATH_FN(TranslationEquals, (const FTransform& A, const FTransform& B), A.TranslationEquals(B))
DAS_MATH_FN(TranslationEqualsTolerance, (const FTransform& A, const FTransform& B, double Tolerance),
            A.TranslationEquals(B, Tolerance))

// FMath: the rest of the AngelScript set. Boxes and planes are passed as their corners and as a point and a normal
// (FBox and FPlane are not script types).
DAS_MATH_SCALAR(CubicInterp, (double P0, double T0, double P1, double T1, double Alpha),
                FMath::CubicInterp(P0, T0, P1, T1, Alpha))
DAS_MATH_FN(CubicInterpVector,
            (const FVector& P0, const FVector& T0, const FVector& P1, const FVector& T1, double Alpha),
            FMath::CubicInterp(P0, T0, P1, T1, Alpha))
DAS_MATH_FN(CubicInterpDerivative,
            (const FVector& P0, const FVector& T0, const FVector& P1, const FVector& T1, double Alpha),
            FMath::CubicInterpDerivative(P0, T0, P1, T1, Alpha))
DAS_MATH_FN(CubicInterpDerivativeRotator,
            (const FRotator& P0, const FRotator& T0, const FRotator& P1, const FRotator& T1, double Alpha),
            FMath::CubicInterpDerivative(P0, T0, P1, T1, Alpha))
DAS_MATH_SCALAR(FastAsin, (double V), FMath::FastAsin(V))
DAS_MATH_SCALAR(FindDeltaAngleRadians, (double A, double B), FMath::FindDeltaAngleRadians(A, B))
DAS_MATH_SCALAR(UnwindRadians, (double A), FMath::UnwindRadians(A))
DAS_MATH_SCALAR(InvSqrt, (double V), FMath::InvSqrt(V))
DAS_MATH_SCALAR(InvSqrtEst, (double V), FMath::InvSqrtEst(V))
DAS_MATH_FN(IsPowerOfTwo, (int32 V), FMath::IsPowerOfTwo(V))
DAS_MATH_SCALAR(LerpStable, (double A, double B, double Alpha), FMath::LerpStable(A, B, Alpha))
DAS_MATH_SCALAR(LogX, (double Base, double V), FMath::LogX(Base, V))
DAS_MATH_SCALAR(Max3, (double A, double B, double C), FMath::Max3(A, B, C))
DAS_MATH_SCALAR(Min3, (double A, double B, double C), FMath::Min3(A, B, C))
DAS_MATH_FN(Rand, (), FMath::Rand())
DAS_MATH_FN(RandHelper, (int32 Max), FMath::RandHelper(Max))
DAS_MATH_SCALAR(RoundToDouble, (double V), FMath::RoundToDouble(V))
DAS_MATH_SCALAR(RoundToFloat, (double V), FMath::RoundToFloat(V))
DAS_MATH_SCALAR(TruncToDouble, (double V), FMath::TruncToDouble(V))
DAS_MATH_SCALAR(TruncToFloat, (double V), FMath::TruncToFloat(V))
DAS_MATH_FN(TruncToInt, (double V), FMath::TruncToInt32(V))
DAS_MATH_FN(LineBoxIntersection,
            (const FVector& BoxMin, const FVector& BoxMax, const FVector& Start, const FVector& End),
            FMath::LineBoxIntersection(FBox(BoxMin, BoxMax), Start, End, End - Start))
DAS_MATH_FN(RayPlaneIntersection,
            (const FVector& Origin, const FVector& Direction, const FVector& PlanePoint, const FVector& PlaneNormal),
            FMath::RayPlaneIntersection(Origin, Direction, FPlane(PlanePoint, PlaneNormal)))
DAS_MATH_FN(SphereAABBIntersection,
            (const FVector& Center, double RadiusSquared, const FVector& BoxMin, const FVector& BoxMax),
            FMath::SphereAABBIntersection(Center, RadiusSquared, FBox(BoxMin, BoxMax)))

// FVector: the rest of the AngelScript set.
DAS_MATH_FN(AllComponentsEqual, (const FVector& V), V.AllComponentsEqual())
DAS_MATH_FN(AllComponentsEqualTolerance, (const FVector& V, double Tolerance), V.AllComponentsEqual(Tolerance))
DAS_MATH_FN(BoundToBox, (const FVector& V, const FVector& Min, const FVector& Max), V.BoundToBox(Min, Max))
DAS_MATH_FN(BoundToCube, (const FVector& V, double Radius), V.BoundToCube(Radius))
DAS_MATH_FN(Coincident, (const FVector& A, const FVector& B), FVector::Coincident(A, B))
DAS_MATH_FN(CoincidentThreshold, (const FVector& A, const FVector& B, double Threshold),
            FVector::Coincident(A, B, Threshold))
DAS_MATH_SCALAR(DistSquared2D, (const FVector& A, const FVector& B), FVector::DistSquared2D(A, B))
DAS_MATH_SCALAR(DistSquaredXY, (const FVector& A, const FVector& B), FVector::DistSquaredXY(A, B))
DAS_MATH_SCALAR(DistXY, (const FVector& A, const FVector& B), FVector::DistXY(A, B))
DAS_MATH_FN(GetClampedToMaxSize2D, (const FVector& V, double Max), V.GetClampedToMaxSize2D(Max))
DAS_MATH_FN(GetClampedToSize2D, (const FVector& V, double Min, double Max), V.GetClampedToSize2D(Min, Max))
DAS_MATH_FN(Orthogonal, (const FVector& A, const FVector& B), FVector::Orthogonal(A, B))
DAS_MATH_FN(OrthogonalThreshold, (const FVector& A, const FVector& B, double Threshold),
            FVector::Orthogonal(A, B, Threshold))
DAS_MATH_FN(Parallel, (const FVector& A, const FVector& B), FVector::Parallel(A, B))
DAS_MATH_FN(ParallelThreshold, (const FVector& A, const FVector& B, double Threshold),
            FVector::Parallel(A, B, Threshold))
DAS_MATH_FN(PointsAreSame, (const FVector& A, const FVector& B), FVector::PointsAreSame(A, B))
DAS_MATH_FN(VectorProjection, (const FVector& V), V.Projection())
DAS_MATH_FN(RotateAngleAxisRad, (const FVector& V, double Radians, const FVector& Axis),
            V.RotateAngleAxisRad(Radians, Axis))
DAS_MATH_FN(ToOrientationRotator, (const FVector& V), V.ToOrientationRotator())
DAS_MATH_FN(VectorPlaneProject, (const FVector& V, const FVector& Normal), FVector::VectorPlaneProject(V, Normal))

// FVector2D: the rest of the AngelScript set.
DAS_MATH_FN(Vector2DContainsNaN, (const FVector2D& V), V.ContainsNaN())
DAS_MATH_SCALAR(Vector2DDistSquared, (const FVector2D& A, const FVector2D& B), FVector2D::DistSquared(A, B))
DAS_MATH_FN(Vector2DGetAbs, (const FVector2D& V), V.GetAbs())
DAS_MATH_SCALAR(Vector2DGetAbsMax, (const FVector2D& V), V.GetAbsMax())
DAS_MATH_SCALAR(Vector2DGetMax, (const FVector2D& V), V.GetMax())
DAS_MATH_SCALAR(Vector2DGetMin, (const FVector2D& V), V.GetMin())
DAS_MATH_FN(Vector2DGetSignVector, (const FVector2D& V), V.GetSignVector())
DAS_MATH_FN(Vector2DIsZero, (const FVector2D& V), V.IsZero())
DAS_MATH_FN(Vector2DIsNearlyZero, (const FVector2D& V), V.IsNearlyZero())
DAS_MATH_FN(Vector2DIsNearlyZeroTolerance, (const FVector2D& V, double Tolerance), V.IsNearlyZero(Tolerance))

// FRotator and FQuat: the rest of the AngelScript set.
DAS_MATH_FN(RotatorContainsNaN, (const FRotator& R), R.ContainsNaN())
DAS_MATH_FN(RotatorMakeFromEuler, (const FVector& Euler), FRotator::MakeFromEuler(Euler))
DAS_MATH_FN(QuatContainsNaN, (const FQuat& Q), Q.ContainsNaN())
DAS_MATH_SCALAR(QuatSizeSquared, (const FQuat& Q), Q.SizeSquared())
DAS_MATH_SCALAR(GetTwistAngle, (const FQuat& Q, const FVector& Axis), Q.GetTwistAngle(Axis))
DAS_MATH_SCALAR(ErrorAutoNormalize, (const FQuat& A, const FQuat& B), FQuat::ErrorAutoNormalize(A, B))
DAS_MATH_FN(FindBetween, (const FVector& A, const FVector& B), FQuat::FindBetween(A, B))
DAS_MATH_FN(MakeFromRotator, (const FRotator& R), FQuat::MakeFromRotator(R))
DAS_MATH_FN(FastBilerp,
            (const FQuat& P00, const FQuat& P10, const FQuat& P01, const FQuat& P11, double FracX, double FracY),
            FQuat::FastBilerp(P00, P10, P01, P11, FracX, FracY))
DAS_MATH_FN(SlerpNotNormalized, (const FQuat& A, const FQuat& B, double Alpha), FQuat::Slerp_NotNormalized(A, B, Alpha))
DAS_MATH_FN(SlerpFullPathNotNormalized, (const FQuat& A, const FQuat& B, double Alpha),
            FQuat::SlerpFullPath_NotNormalized(A, B, Alpha))
DAS_MATH_FN(Squad, (const FQuat& A, const FQuat& TangentA, const FQuat& B, const FQuat& TangentB, double Alpha),
            FQuat::Squad(A, TangentA, B, TangentB, Alpha))
DAS_MATH_FN(SquadFullPath,
            (const FQuat& A, const FQuat& TangentA, const FQuat& B, const FQuat& TangentB, double Alpha),
            FQuat::SquadFullPath(A, TangentA, B, TangentB, Alpha))

// FLinearColor (float components, as in the engine).
DAS_MATH_FN(ColorEquals, (const FLinearColor& A, const FLinearColor& B), A.Equals(B))
DAS_MATH_FN(ColorEqualsTolerance, (const FLinearColor& A, const FLinearColor& B, double Tolerance),
            A.Equals(B, float(Tolerance)))
DAS_MATH_FN(ColorGetClamped, (const FLinearColor& C), C.GetClamped())
DAS_MATH_FN(ColorGetClampedRange, (const FLinearColor& C, double Min, double Max),
            C.GetClamped(float(Min), float(Max)))
DAS_MATH_FN(HSVToLinearRGB, (const FLinearColor& C), C.HSVToLinearRGB())
DAS_MATH_FN(LinearRGBToHSV, (const FLinearColor& C), C.LinearRGBToHSV())
DAS_MATH_FN(IsAlmostBlack, (const FLinearColor& C), C.IsAlmostBlack())
DAS_MATH_FN(MakeFromColorTemperature, (double Kelvin), FLinearColor::MakeFromColorTemperature(float(Kelvin)))
DAS_MATH_FN(MakeFromHSV8, (uint8 H, uint8 S, uint8 V), FLinearColor::MakeFromHSV8(H, S, V))
DAS_MATH_FN(MakeRandomColor, (), FLinearColor::MakeRandomColor())
DAS_MATH_FN(ToFColor, (const FLinearColor& C, bool bSRGB), C.ToFColor(bSRGB))
#undef DAS_MATH_FN
#undef DAS_MATH_SCALAR
FTransform RemoveScaling(const FTransform& T)
{
    FTransform Result = T;
    Result.RemoveScaling();
    return Result;
}
FTransform Blend(const FTransform& A, const FTransform& B, double Alpha)
{
    FTransform Result;
    Result.Blend(A, B, float(Alpha));
    return Result;
}

// FVector2D.
FVector2D ZeroVector2D()
{
    return FVector2D::ZeroVector;
}
bool Vector2DEquals(const FVector2D& A, const FVector2D& B, double Tolerance)
{
    return A.Equals(B, Tolerance);
}
FVector2D Vector2DGetSafeNormal(const FVector2D& V)
{
    return V.GetSafeNormal();
}
FVector2D GetRotated(const FVector2D& V, double Degrees)
{
    return V.GetRotated(Degrees);
}
// As FVector::GetClampedToMaxSize (FVector2D has none).
FVector2D Vector2DGetClampedToMaxSize(const FVector2D& V, double Max)
{
    if (Max < UE_KINDA_SMALL_NUMBER)
        return FVector2D::ZeroVector;
    const double SizeSquared = V.SizeSquared();
    return SizeSquared > Max * Max ? V * (Max * FMath::InvSqrt(SizeSquared)) : V;
}
// HexColor: 0xAARRGGBB; sRGB colors are converted to linear, others reinterpreted (as AngelScript's MakeFromHex).
FLinearColor MakeFromHex(int64 Hex, bool bSRGB)
{
    const FColor Color(uint32(Hex & 0xFFFFFFFF));
    return bSRGB ? FLinearColor(Color) : Color.ReinterpretAsLinear();
}
FLinearColor MakeFromHexSRGB(int64 Hex)
{
    return MakeFromHex(Hex, true);
}
// FTransform in place: the script passes a variable (`var t : UETransform&`, DasNativeBinds.h references).
void SetLocation(FTransform& T, const FVector& V)
{
    T.SetLocation(V);
}
void SetTranslation(FTransform& T, const FVector& V)
{
    T.SetTranslation(V);
}
void SetRotation(FTransform& T, const FQuat& Q)
{
    T.SetRotation(Q);
}
void SetScale3D(FTransform& T, const FVector& V)
{
    T.SetScale3D(V);
}
void SetTranslationAndScale3D(FTransform& T, const FVector& Translation, const FVector& Scale)
{
    T.SetTranslationAndScale3D(Translation, Scale);
}
void AddToTranslation(FTransform& T, const FVector& V)
{
    T.AddToTranslation(V);
}
void Accumulate(FTransform& T, const FTransform& Source)
{
    T.Accumulate(Source);
}
void BlendWith(FTransform& T, const FTransform& Other, double Alpha)
{
    T.BlendWith(Other, float(Alpha));
}
void ConcatenateRotation(FTransform& T, const FQuat& Q)
{
    T.ConcatenateRotation(Q);
}
void NormalizeRotation(FTransform& T)
{
    T.NormalizeRotation();
}
void ScaleTranslation(FTransform& T, const FVector& Scale)
{
    T.ScaleTranslation(Scale);
}
void ScaleTranslationScalar(FTransform& T, double Scale)
{
    T.ScaleTranslation(Scale);
}
void SetToRelativeTransform(FTransform& T, const FTransform& Other)
{
    T.SetToRelativeTransform(Other);
}

// Output parameters.
void ToAxisAndAngle(const FQuat& Q, FVector& Axis, double& Angle)
{
    Q.ToAxisAndAngle(Axis, Angle);
}
void ToDirectionAndLength(const FVector& V, FVector& Direction, double& Length)
{
    V.ToDirectionAndLength(Direction, Length);
}
void ToDirectionAndLength2D(const FVector2D& V, FVector2D& Direction, double& Length)
{
    V.ToDirectionAndLength(Direction, Length);
}
void ToSwingTwist(const FQuat& Q, const FVector& TwistAxis, FQuat& Swing, FQuat& Twist)
{
    Q.ToSwingTwist(TwistAxis, Swing, Twist);
}
void FindBestAxisVectors(const FVector& V, FVector& Axis1, FVector& Axis2)
{
    V.FindBestAxisVectors(Axis1, Axis2);
}
void CalcTangents(const FQuat& Previous, const FQuat& P, const FQuat& Next, double Tension, FQuat& Tangent)
{
    FQuat::CalcTangents(Previous, P, Next, Tension, Tangent);
}
void GetWindingAndRemainder(const FRotator& R, FRotator& Winding, FRotator& Remainder)
{
    R.GetWindingAndRemainder(Winding, Remainder);
}
bool SegmentIntersection2D(const FVector& StartA, const FVector& EndA, const FVector& StartB, const FVector& EndB,
                           FVector& Point)
{
    return FMath::SegmentIntersection2D(StartA, EndA, StartB, EndB, Point);
}
// The box as its corners (FBox is not a script type); the hit time is double in scripts.
bool LineExtentBoxIntersection(const FVector& BoxMin, const FVector& BoxMax, const FVector& Start, const FVector& End,
                               const FVector& Extent, FVector& HitLocation, FVector& HitNormal, double& HitTime)
{
    float Time = 0;
    const bool bHit = FMath::LineExtentBoxIntersection(FBox(BoxMin, BoxMax), Start, End, Extent, HitLocation,
                                                       HitNormal, Time);
    HitTime = Time;
    return bHit;
}
// C++ integer division truncates toward zero; dividing by zero (or INT_MIN by -1) is a script error.
template <class T> T IntegerDivisionTrunc(T Value, T DivideBy)
{
    if (DivideBy == 0 || (Value == TNumericLimits<T>::Min() && DivideBy == -1))
    {
        DasNativeError(DivideBy == 0 ? TEXT("division by zero") : TEXT("integer overflow"));
        return 0;
    }
    return Value / DivideBy;
}
}

DAS_NATIVE_BINDS(UEMath, "ue_math")
{
    using namespace DasNativeMath;
    // FMath: interpolation.
    Binds.Function<FInterpTo>(TEXT("f_interp_to"));
    Binds.Function<FInterpConstantTo>(TEXT("f_interp_constant_to"));
    Binds.Function<VInterpTo>(TEXT("v_interp_to"));
    Binds.Function<VInterpConstantTo>(TEXT("v_interp_constant_to"));
    Binds.Function<VInterpNormalRotationTo>(TEXT("v_interp_normal_rotation_to"));
    Binds.Function<Vector2DInterpTo>(TEXT("vector2d_interp_to"));
    Binds.Function<Vector2DInterpConstantTo>(TEXT("vector2d_interp_constant_to"));
    Binds.Function<RInterpTo>(TEXT("r_interp_to"));
    Binds.Function<RInterpConstantTo>(TEXT("r_interp_constant_to"));
    Binds.Function<QInterpTo>(TEXT("q_interp_to"));
    Binds.Function<QInterpConstantTo>(TEXT("q_interp_constant_to"));
    Binds.Function<CInterpTo>(TEXT("c_interp_to"));
    // Easing.
    Binds.Function<EaseIn>(TEXT("ease_in"));
    Binds.Function<EaseOut>(TEXT("ease_out"));
    Binds.Function<EaseInOut>(TEXT("ease_in_out"));
    Binds.Function<SinusoidalIn>(TEXT("sinusoidal_in"));
    Binds.Function<SinusoidalOut>(TEXT("sinusoidal_out"));
    Binds.Function<SinusoidalInOut>(TEXT("sinusoidal_in_out"));
    Binds.Function<ExpoIn>(TEXT("expo_in"));
    Binds.Function<ExpoOut>(TEXT("expo_out"));
    Binds.Function<ExpoInOut>(TEXT("expo_in_out"));
    Binds.Function<CircularIn>(TEXT("circular_in"));
    Binds.Function<CircularOut>(TEXT("circular_out"));
    Binds.Function<CircularInOut>(TEXT("circular_in_out"));
    // Comparisons, ranges, angles, rounding.
    Binds.Function<IsNearlyEqual>(TEXT("is_nearly_equal"));
    Binds.Function<IsNearlyEqualTolerance>(TEXT("is_nearly_equal"));
    Binds.Function<IsNearlyZero>(TEXT("is_nearly_zero"));
    Binds.Function<IsNearlyZeroTolerance>(TEXT("is_nearly_zero"));
    Binds.Function<IsWithin>(TEXT("is_within"));
    Binds.Function<IsWithinInclusive>(TEXT("is_within_inclusive"));
    Binds.Function<MappedClamped>(TEXT("get_mapped_range_value_clamped"));
    Binds.Function<MappedUnclamped>(TEXT("get_mapped_range_value_unclamped"));
    Binds.Function<NormalizeToRange>(TEXT("normalize_to_range"));
    Binds.Function<FindDeltaAngleDegrees>(TEXT("find_delta_angle_degrees"));
    Binds.Function<UnwindDegrees>(TEXT("unwind_degrees"));
    Binds.Function<ClampAngle>(TEXT("clamp_angle"));
    Binds.Function<DegreesToRadians>(TEXT("degrees_to_radians"));
    Binds.Function<RadiansToDegrees>(TEXT("radians_to_degrees"));
    Binds.Function<GridSnap>(TEXT("grid_snap"));
    Binds.Function<RoundFromZero>(TEXT("round_from_zero"));
    Binds.Function<MakePulsatingValue>(TEXT("make_pulsating_value"));
    // Random values.
    Binds.Function<RandRangeInt>(TEXT("rand_range"));
    Binds.Function<RandRangeDouble>(TEXT("rand_range"));
    Binds.Function<FRand>(TEXT("frand"));
    Binds.Function<RandBool>(TEXT("rand_bool"));
    Binds.Function<VRand>(TEXT("v_rand"));
    Binds.Function<VRandCone>(TEXT("v_rand_cone"));
    Binds.Function<RandPointInCircle>(TEXT("rand_point_in_circle"));
    Binds.Function<RandomPointInBoundingBox>(TEXT("random_point_in_bounding_box"));
    Binds.Function<RandomRotator>(TEXT("random_rotator"));
    // Geometry and noise.
    Binds.Function<ClosestPointOnLine>(TEXT("closest_point_on_line"));
    Binds.Function<ClosestPointOnInfiniteLine>(TEXT("closest_point_on_infinite_line"));
    Binds.Function<PointDistToSegment>(TEXT("point_dist_to_segment"));
    Binds.Function<LinePlaneIntersection>(TEXT("line_plane_intersection"));
    Binds.Function<LineSphereIntersection>(TEXT("line_sphere_intersection"));
    Binds.Function<PerlinNoise1D>(TEXT("perlin_noise_1d"));
    Binds.Function<PerlinNoise2D>(TEXT("perlin_noise_2d"));
    Binds.Function<PerlinNoise3D>(TEXT("perlin_noise_3d"));

    // FVector.
    Binds.Function<ZeroVector>(TEXT("zero_vector"));
    Binds.Function<OneVector>(TEXT("one_vector"));
    Binds.Function<UpVector>(TEXT("up_vector"));
    Binds.Function<DownVector>(TEXT("down_vector"));
    Binds.Function<ForwardVector>(TEXT("forward_vector"));
    Binds.Function<BackwardVector>(TEXT("backward_vector"));
    Binds.Function<RightVector>(TEXT("right_vector"));
    Binds.Function<LeftVector>(TEXT("left_vector"));
    Binds.Function<VectorEquals>(TEXT("equals"));
    Binds.Function<VectorEqualsTolerance>(TEXT("equals"));
    Binds.Function<VectorIsZero>(TEXT("is_zero"));
    Binds.Function<VectorIsNearlyZero>(TEXT("is_nearly_zero"));
    Binds.Function<VectorIsNormalized>(TEXT("is_normalized"));
    Binds.Function<VectorIsUnit>(TEXT("is_unit"));
    Binds.Function<VectorContainsNaN>(TEXT("contains_nan"));
    Binds.Function<GetAbs>(TEXT("get_abs"));
    Binds.Function<GetMax>(TEXT("get_max"));
    Binds.Function<GetMin>(TEXT("get_min"));
    Binds.Function<GetAbsMax>(TEXT("get_abs_max"));
    Binds.Function<ComponentMin>(TEXT("component_min"));
    Binds.Function<ComponentMax>(TEXT("component_max"));
    Binds.Function<GetSignVector>(TEXT("get_sign_vector"));
    Binds.Function<Reciprocal>(TEXT("reciprocal"));
    Binds.Function<Size2D>(TEXT("size_2d"));
    Binds.Function<SizeSquared2D>(TEXT("size_squared_2d"));
    Binds.Function<Dist2D>(TEXT("dist_2d"));
    Binds.Function<DistSquared>(TEXT("dist_squared"));
    Binds.Function<GetSafeNormal2D>(TEXT("get_safe_normal_2d"));
    Binds.Function<GetClampedToSize>(TEXT("get_clamped_to_size"));
    Binds.Function<GetClampedToMaxSize>(TEXT("get_clamped_to_max_size"));
    Binds.Function<VectorGridSnap>(TEXT("grid_snap"));
    Binds.Function<RotateAngleAxis>(TEXT("rotate_angle_axis"));
    Binds.Function<MirrorByVector>(TEXT("mirror_by_vector"));
    Binds.Function<HeadingAngle>(TEXT("heading_angle"));
    Binds.Function<CosineAngle2D>(TEXT("cosine_angle_2d"));
    Binds.Function<VectorRotation>(TEXT("rotation"));
    Binds.Function<ToOrientationQuat>(TEXT("to_orientation_quat"));
    Binds.Function<PointsAreNear>(TEXT("points_are_near"));

    // FRotator.
    Binds.Function<ZeroRotator>(TEXT("zero_rotator"));
    Binds.Function<RotatorEquals>(TEXT("equals"));
    Binds.Function<RotatorEqualsTolerance>(TEXT("equals"));
    Binds.Function<RotatorIsZero>(TEXT("is_zero"));
    Binds.Function<RotatorIsNearlyZero>(TEXT("is_nearly_zero"));
    Binds.Function<GetInverse>(TEXT("get_inverse"));
    Binds.Function<GetNormalized>(TEXT("get_normalized"));
    Binds.Function<GetDenormalized>(TEXT("get_denormalized"));
    Binds.Function<GetClamped>(TEXT("get_clamped"));
    Binds.Function<NormalizeAxis>(TEXT("normalize_axis"));
    Binds.Function<ClampAxis>(TEXT("clamp_axis"));
    Binds.Function<RotatorVector>(TEXT("vector"));
    Binds.Function<RotatorQuaternion>(TEXT("quaternion"));
    Binds.Function<RotatorEuler>(TEXT("euler"));
    Binds.Function<GetManhattanDistance>(TEXT("get_manhattan_distance"));
    Binds.Function<MakeFromX>(TEXT("make_from_x"));
    Binds.Function<MakeFromY>(TEXT("make_from_y"));
    Binds.Function<MakeFromZ>(TEXT("make_from_z"));
    Binds.Function<MakeFromXY>(TEXT("make_from_xy"));
    Binds.Function<MakeFromXZ>(TEXT("make_from_xz"));
    Binds.Function<MakeFromYX>(TEXT("make_from_yx"));
    Binds.Function<MakeFromYZ>(TEXT("make_from_yz"));
    Binds.Function<MakeFromZX>(TEXT("make_from_zx"));
    Binds.Function<MakeFromZY>(TEXT("make_from_zy"));

    // FQuat.
    Binds.Function<QuatEquals>(TEXT("equals"));
    Binds.Function<QuatEqualsTolerance>(TEXT("equals"));
    Binds.Function<QuatIsIdentity>(TEXT("is_identity"));
    Binds.Function<GetAngle>(TEXT("get_angle"));
    Binds.Function<AngularDistance>(TEXT("angular_distance"));
    Binds.Function<QuatEuler>(TEXT("euler"));
    Binds.Function<MakeFromEuler>(TEXT("make_from_euler"));
    Binds.Function<FindBetweenVectors>(TEXT("find_between_vectors"));
    Binds.Function<FindBetweenNormals>(TEXT("find_between_normals"));
    Binds.Function<QuatForward>(TEXT("get_forward_vector"));
    Binds.Function<QuatRight>(TEXT("get_right_vector"));
    Binds.Function<QuatUp>(TEXT("get_up_vector"));
    Binds.Function<GetAxisX>(TEXT("get_axis_x"));
    Binds.Function<GetAxisY>(TEXT("get_axis_y"));
    Binds.Function<GetAxisZ>(TEXT("get_axis_z"));
    Binds.Function<GetRotationAxis>(TEXT("get_rotation_axis"));
    Binds.Function<QuatVector>(TEXT("vector"));
    Binds.Function<SlerpFullPath>(TEXT("slerp_full_path"));
    Binds.Function<FastLerp>(TEXT("fast_lerp"));

    // FTransform.
    Binds.Function<TransformEquals>(TEXT("equals"));
    Binds.Function<TransformEqualsTolerance>(TEXT("equals"));
    Binds.Function<EqualsNoScale>(TEXT("equals_no_scale"));
    Binds.Function<GetRelativeTransform>(TEXT("get_relative_transform"));
    Binds.Function<&Blend>(TEXT("blend"));
    Binds.Function<GetMaximumAxisScale>(TEXT("get_maximum_axis_scale"));
    Binds.Function<GetMinimumAxisScale>(TEXT("get_minimum_axis_scale"));
    Binds.Function<&RemoveScaling>(TEXT("remove_scaling"));
    Binds.Function<TransformPositionNoScale>(TEXT("transform_position_no_scale"));
    Binds.Function<InverseTransformPositionNoScale>(TEXT("inverse_transform_position_no_scale"));
    Binds.Function<InverseTransformRotation>(TEXT("inverse_transform_rotation"));
    Binds.Function<GetDeterminant>(TEXT("get_determinant"));
    Binds.Function<TransformRotator>(TEXT("rotator"));

    // FVector2D.
    Binds.Function<&ZeroVector2D>(TEXT("zero_vector2d"));
    Binds.Function<&Vector2DEquals>(TEXT("equals"));
    Binds.Function<&Vector2DGetSafeNormal>(TEXT("get_safe_normal"));
    Binds.Function<&GetRotated>(TEXT("get_rotated"));

    // The rest of AngelScript's FMath, FVector, FVector2D, FRotator, FQuat, FLinearColor and FTransform binds
    // that need no new bind features (Docs/Dev/Architecture/NativeBindsCoverage.md).
    Binds.Function<CubicInterp>(TEXT("cubic_interp"));
    Binds.Function<CubicInterpVector>(TEXT("cubic_interp"));
    Binds.Function<CubicInterpDerivative>(TEXT("cubic_interp_derivative"));
    Binds.Function<CubicInterpDerivativeRotator>(TEXT("cubic_interp_derivative"));
    Binds.Function<FastAsin>(TEXT("fast_asin"));
    Binds.Function<FindDeltaAngleRadians>(TEXT("find_delta_angle_radians"));
    Binds.Function<UnwindRadians>(TEXT("unwind_radians"));
    Binds.Function<InvSqrt>(TEXT("inv_sqrt"));
    Binds.Function<InvSqrtEst>(TEXT("inv_sqrt_est"));
    Binds.Function<IsPowerOfTwo>(TEXT("is_power_of_two"));
    Binds.Function<&IntegerDivisionTrunc<int32>>(TEXT("integer_division_trunc"));
    Binds.Function<&IntegerDivisionTrunc<int64>>(TEXT("integer_division_trunc"));
    Binds.Function<LerpStable>(TEXT("lerp_stable"));
    Binds.Function<LogX>(TEXT("log_x"));
    Binds.Function<Max3>(TEXT("max3"));
    Binds.Function<Min3>(TEXT("min3"));
    Binds.Function<Rand>(TEXT("rand"));
    Binds.Function<RandHelper>(TEXT("rand_helper"));
    Binds.Function<RoundToDouble>(TEXT("round_to_double"));
    Binds.Function<RoundToFloat>(TEXT("round_to_float"));
    Binds.Function<TruncToDouble>(TEXT("trunc_to_double"));
    Binds.Function<TruncToFloat>(TEXT("trunc_to_float"));
    Binds.Function<TruncToInt>(TEXT("trunc_to_int"));
    Binds.Function<LineBoxIntersection>(TEXT("line_box_intersection"));
    Binds.Function<RayPlaneIntersection>(TEXT("ray_plane_intersection"));
    Binds.Function<SphereAABBIntersection>(TEXT("sphere_aabb_intersection"));

    Binds.Function<AllComponentsEqual>(TEXT("all_components_equal"));
    Binds.Function<AllComponentsEqualTolerance>(TEXT("all_components_equal"));
    Binds.Function<BoundToBox>(TEXT("bound_to_box"));
    Binds.Function<BoundToBox>(TEXT("component_clamp"));
    Binds.Function<BoundToCube>(TEXT("bound_to_cube"));
    Binds.Function<Coincident>(TEXT("coincident"));
    Binds.Function<CoincidentThreshold>(TEXT("coincident"));
    Binds.Function<DistSquared2D>(TEXT("dist_squared_2d"));
    Binds.Function<DistSquaredXY>(TEXT("dist_squared_xy"));
    Binds.Function<DistXY>(TEXT("dist_xy"));
    Binds.Function<GetClampedToMaxSize2D>(TEXT("get_clamped_to_max_size_2d"));
    Binds.Function<GetClampedToSize2D>(TEXT("get_clamped_to_size_2d"));
    Binds.Function<Orthogonal>(TEXT("orthogonal"));
    Binds.Function<OrthogonalThreshold>(TEXT("orthogonal"));
    Binds.Function<Parallel>(TEXT("parallel"));
    Binds.Function<ParallelThreshold>(TEXT("parallel"));
    Binds.Function<PointsAreSame>(TEXT("points_are_same"));
    Binds.Function<VectorProjection>(TEXT("projection"));
    Binds.Function<RotateAngleAxisRad>(TEXT("rotate_angle_axis_rad"));
    Binds.Function<ToOrientationRotator>(TEXT("to_orientation_rotator"));
    Binds.Function<VectorPlaneProject>(TEXT("vector_plane_project"));

    Binds.Function<Vector2DContainsNaN>(TEXT("contains_nan"));
    Binds.Function<Vector2DDistSquared>(TEXT("dist_squared"));
    Binds.Function<Vector2DGetAbs>(TEXT("get_abs"));
    Binds.Function<Vector2DGetAbsMax>(TEXT("get_abs_max"));
    Binds.Function<&Vector2DGetClampedToMaxSize>(TEXT("get_clamped_to_max_size"));
    Binds.Function<Vector2DGetMax>(TEXT("get_max"));
    Binds.Function<Vector2DGetMin>(TEXT("get_min"));
    Binds.Function<Vector2DGetSignVector>(TEXT("get_sign_vector"));
    Binds.Function<Vector2DIsZero>(TEXT("is_zero"));
    Binds.Function<Vector2DIsNearlyZero>(TEXT("is_nearly_zero"));
    Binds.Function<Vector2DIsNearlyZeroTolerance>(TEXT("is_nearly_zero"));

    Binds.Function<RotatorContainsNaN>(TEXT("contains_nan"));
    // FQuat has make_from_euler; the result type alone cannot tell overloads apart.
    Binds.Function<RotatorMakeFromEuler>(TEXT("rotator_make_from_euler"));
    Binds.Function<QuatContainsNaN>(TEXT("contains_nan"));
    Binds.Function<QuatSizeSquared>(TEXT("size_squared"));
    Binds.Function<GetTwistAngle>(TEXT("get_twist_angle"));
    Binds.Function<ErrorAutoNormalize>(TEXT("error_auto_normalize"));
    Binds.Function<FindBetween>(TEXT("find_between"));
    Binds.Function<MakeFromRotator>(TEXT("make_from_rotator"));
    Binds.Function<FastBilerp>(TEXT("fast_bilerp"));
    Binds.Function<SlerpNotNormalized>(TEXT("slerp_not_normalized"));
    Binds.Function<SlerpFullPathNotNormalized>(TEXT("slerp_full_path_not_normalized"));
    Binds.Function<Squad>(TEXT("squad"));
    Binds.Function<SquadFullPath>(TEXT("squad_full_path"));

    Binds.Function<ColorEquals>(TEXT("equals"));
    Binds.Function<ColorEqualsTolerance>(TEXT("equals"));
    Binds.Function<ColorGetClamped>(TEXT("get_clamped"));
    Binds.Function<ColorGetClampedRange>(TEXT("get_clamped"));
    Binds.Function<HSVToLinearRGB>(TEXT("hsv_to_linear_rgb"));
    Binds.Function<LinearRGBToHSV>(TEXT("linear_rgb_to_hsv"));
    Binds.Function<IsAlmostBlack>(TEXT("is_almost_black"));
    Binds.Function<MakeFromColorTemperature>(TEXT("make_from_color_temperature"));
    Binds.Function<MakeFromHSV8>(TEXT("make_from_hsv8"));
    Binds.Function<&MakeFromHex>(TEXT("make_from_hex"));
    Binds.Function<&MakeFromHexSRGB>(TEXT("make_from_hex"));
    Binds.Function<MakeRandomColor>(TEXT("make_random_color"));
    Binds.Function<ToFColor>(TEXT("to_fcolor"));

    Binds.Function<TransformContainsNaN>(TEXT("contains_nan"));
    Binds.Function<TransformGetLocation>(TEXT("get_location"));
    Binds.Function<GetScale3D>(TEXT("get_scale3d"));
    Binds.Function<GetRelativeTransformReverse>(TEXT("get_relative_transform_reverse"));
    Binds.Function<TransformVectorNoScale>(TEXT("transform_vector_no_scale"));
    Binds.Function<InverseTransformVectorNoScale>(TEXT("inverse_transform_vector_no_scale"));
    Binds.Function<TransformIsRotationNormalized>(TEXT("is_rotation_normalized"));
    Binds.Function<SubtractTranslations>(TEXT("subtract_translations"));
    Binds.Function<TranslationEquals>(TEXT("translation_equals"));
    Binds.Function<TranslationEqualsTolerance>(TEXT("translation_equals"));

    // Coverage part 3: FTransform in place and output parameters (references, DasNativeBinds.h).
    Binds.Function<&SetLocation>(TEXT("set_location"));
    Binds.Function<&SetTranslation>(TEXT("set_translation"));
    Binds.Function<&SetRotation>(TEXT("set_rotation"));
    Binds.Function<&SetScale3D>(TEXT("set_scale3d"));
    Binds.Function<&SetTranslationAndScale3D>(TEXT("set_translation_and_scale3d"));
    Binds.Function<&AddToTranslation>(TEXT("add_to_translation"));
    Binds.Function<&Accumulate>(TEXT("accumulate"));
    Binds.Function<&BlendWith>(TEXT("blend_with"));
    Binds.Function<&ConcatenateRotation>(TEXT("concatenate_rotation"));
    Binds.Function<&NormalizeRotation>(TEXT("normalize_rotation"));
    Binds.Function<&ScaleTranslation>(TEXT("scale_translation"));
    Binds.Function<&ScaleTranslationScalar>(TEXT("scale_translation"));
    Binds.Function<&SetToRelativeTransform>(TEXT("set_to_relative_transform"));
    Binds.Function<&ToAxisAndAngle>(TEXT("to_axis_and_angle"));
    Binds.Function<&ToDirectionAndLength>(TEXT("to_direction_and_length"));
    Binds.Function<&ToDirectionAndLength2D>(TEXT("to_direction_and_length"));
    Binds.Function<&ToSwingTwist>(TEXT("to_swing_twist"));
    Binds.Function<&FindBestAxisVectors>(TEXT("find_best_axis_vectors"));
    Binds.Function<&CalcTangents>(TEXT("calc_tangents"));
    Binds.Function<&GetWindingAndRemainder>(TEXT("get_winding_and_remainder"));
    Binds.Function<&SegmentIntersection2D>(TEXT("segment_intersection_2d"));
    Binds.Function<&LineExtentBoxIntersection>(TEXT("line_extent_box_intersection"));
}
