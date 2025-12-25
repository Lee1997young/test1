#pragma once

#include "CoreMinimal.h"
#include "SCoordGraph.h"
#include "SCUITypes.h"

class SCUI_API SLinearGraph : public SCoordGraph
{
public:
	void Construct(const FArguments& InArgs);
	void SetSplineType(bool bSpline);
	void SetSplineTangentScale(float InTangentScale);
	void SetSplinePointSizeScale(float InPointSizeScale);
	void SetShowLabelsData(bool bShow);
	void SetLabelsDataFont(const FSlateFontInfo& InFontInfo);
	void SetLabelsDataTextColorAndOpacity(const FSlateColor& InColorAndOpacity);

protected:
	int32 OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const override;

	virtual void HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	FSlateFontInfo GetLabelsDataFont() const;
	FSlateColor GetLabelsDataColor() const;
protected:
	bool bSplineType;
	float TangentScale;
	float PointSizeScale;
	bool bShowLabelsData;
	FSlateFontInfo LabelsDataFont;
	FSlateColor LabelsDataColor;
};