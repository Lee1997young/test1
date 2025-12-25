#pragma once

#include "CoreMinimal.h"
#include "SGraphBase.h"
#include "SCUITypes.h"

class SCUI_API SCircleChart : public SGraphBase
{
public:
	void Construct(const FArguments& InArgs);

	virtual int32 OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const;
	ECircleChartType GetChartType();
	void SetChartType(ECircleChartType InChartType);

	void SetDatas(const TArray<TSharedPtr<FGraphDataBase>>& InDatas);

	void SetThickness(float InThickness);
	void SetShowDataText(bool bShow);
	void SetDataTextType(ECircleChartDataTextType InDataTextType);
	void SetDataTextSuffix(const FString& InDataTextSuffix);
	void SetLineLength(float Length);
	void SetStartAngle(float Angle);

protected:
	virtual void HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

protected:
	ECircleChartType ChartType;

	//Data
	int32 DataNum; //Data Num
	float DataTotal;//Data Value Total
	const int32 TrisTotalNum = 1000;

	float Thickness;
	bool bShowDataText;
	ECircleChartDataTextType DataTextType;
	FString DataTextSuffix;

	float LineLength; //0.1 - 1.0
	float StartAngle; //0 - 360
};