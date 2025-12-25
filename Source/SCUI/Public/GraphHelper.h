#pragma once
#include "CoreMinimal.h"

class FGraphHelper
{
public:
	static bool IsNumeric(float InValue);

	//According to the segmentation between the maximum value and the minimum value, 
	//we can judge whether there is a decimal part
	static bool HasFloatValue(float MinValue, float MaxValue, int32 SectionNum);

	static FString FloatToString(float InValue, int32 Decimal = 2);

	//Keep one decimal place
	static FString ValueToString(float InValue, bool bFloat = false);

	/**
	 * Generates a list of sample points on a Bezier curve defined by 2 points.
	 *
	 * @param ControlPoints	Array of 4 FVectors (vert1, controlpoint1, controlpoint2, vert2).
	 * @param NumPoints Number of samples.
	 * @param OutPoints Receives the output samples.
	 * @return The path length.
	 */
	static float EvaluateBezier(const FVector2D* ControlPoints, int32 NumPoints, TArray<FVector2D>& OutPoints);

	static bool IsInsideSector(float TargetAngle, float StartAngle, float EndAngle);
};