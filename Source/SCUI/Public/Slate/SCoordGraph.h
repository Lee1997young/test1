#pragma once

#include "CoreMinimal.h"
#include "SGraphBase.h"
#include "SCUITypes.h"

class SCUI_API SCoordGraph : public SGraphBase
{
public:
	void Construct(const FArguments& InArgs);

	EOrientation GetOrientation();
	void SetOrientation(EOrientation InOrientation);

	void SetLabels(const TArray<FString>& InLabels);
	void SetValues(float InMinValue, float InMaxValue, int32 InRowNum);
	void SetHideLabels(const TArray<FString>& InLabels);

	void SetDrawColorBlock(bool bDraw);
	void SetDrawGradient(bool bGradient);
	void SetLableTextRot(float InRot);

protected:
	int32 OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const override;

	void CalcLongestLabelWidth();
	void CalcLongestValueWidth();

	FVector2D GetCoordOrigin(bool bNeedOffset = true) const;
	FVector2D GetCoordSize(bool bNeedOffset = true) const;
	bool GetPaintPos(const FString& InLabel, float InValue, FVector2D& PaintPos) const;

protected:
	//Data
	TArray<FString> Labels;
	float MinValue;
	float MaxValue;
	int32 RowNum;
	TArray<float> Values;
	bool bFloatValue;
	//Style
	EOrientation Orientation = EOrientation::Orient_Vertical;

	TArray<FString> HideLabels;
	float LableTextRot;
	bool bDrawColorBlock;
	bool bDrawGradient;

	FVector2D LongestLabelSize;
	FVector2D LongestValueSize;

private:
	const float TextAxisSpace = 0.2f;
};