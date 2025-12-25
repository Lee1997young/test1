#include "GraphHelper.h"

bool FGraphHelper::IsNumeric(float InValue)
{
	return (InValue - int32(InValue) == 0) ? true : false;
}

bool FGraphHelper::HasFloatValue(float MinValue, float MaxValue, int32 SectionNum)
{
	if ((MaxValue - MinValue) <= 1.0f) return true;
	return (int32)(MaxValue - MinValue) % (SectionNum - 1) != 0 ? true : false;
}

FString FGraphHelper::FloatToString(float InValue, int32 Decimal)
{
	if (Decimal < 1)
	{
		return FString::Printf(TEXT("%d"), (int32)InValue);
	}

	if (FMath::Fmod(InValue, 1.0) == 0.0)
	{
		return FString::Printf(TEXT("%d"), (int32)InValue);
	}
	else
	{
		return Decimal == 1 ? FString::Printf(TEXT("%.1f"), InValue) : FString::Printf(TEXT("%.01f"), InValue);
	}
	
	
}

FString FGraphHelper::ValueToString(float InValue, bool bFloat/* = false*/)
{
	return bFloat ? FString::Printf(TEXT("%.01f"), InValue) : FString::Printf(TEXT("%d"), (int32)InValue);
}

float FGraphHelper::EvaluateBezier(const FVector2D* ControlPoints, int32 NumPoints, TArray<FVector2D>& OutPoints)
{
	check(ControlPoints);
	check(NumPoints >= 2);

	// var q is the change in t between successive evaluations.
	const float q = 1.f / (NumPoints - 1); // q is dependent on the number of GAPS = POINTS-1

	// recreate the names used in the derivation
	const FVector2D& P0 = ControlPoints[0];
	const FVector2D& P1 = ControlPoints[1];
	const FVector2D& P2 = ControlPoints[2];
	const FVector2D& P3 = ControlPoints[3];

	// coefficients of the cubic polynomial that we're FDing -
	const FVector2D a = P0;
	const FVector2D b = 3 * (P1 - P0);
	const FVector2D c = 3 * (P2 - 2 * P1 + P0);
	const FVector2D d = P3 - 3 * P2 + 3 * P1 - P0;

	// initial values of the poly and the 3 diffs -
	FVector2D S = a;						// the poly value
	FVector2D U = b * q + c * q*q + d * q*q*q;	// 1st order diff (quadratic)
	FVector2D V = 2 * c*q*q + 6 * d*q*q*q;	// 2nd order diff (linear)
	FVector2D W = 6 * d*q*q*q;				// 3rd order diff (constant)

	// Path length.
	float Length = 0.f;

	FVector2D OldPos = P0;
	OutPoints.Add(P0);	// first point on the curve is always P0.

	for (int32 i = 1; i < NumPoints; ++i)
	{
		// calculate the next value and update the deltas
		S += U;			// update poly value
		U += V;			// update 1st order diff value
		V += W;			// update 2st order diff value
		// 3rd order diff is constant => no update needed.

		// Update Length.
		Length += FMath::Sqrt(FVector2D::DistSquared(S, OldPos));
		OldPos = S;

		OutPoints.Add(S);
	}

	// Return path length as experienced in sequence (linear interpolation between points).
	return Length;
}

bool FGraphHelper::IsInsideSector(float TargetAngle, float StartAngle, float EndAngle)
{
	return (TargetAngle > StartAngle && TargetAngle < EndAngle);
}
