#pragma once

#include "CoreMinimal.h"
#include "SCoordGraph.h"
#include "SCUITypes.h"

class SCUI_API SAdvHistogram : public SCoordGraph
{
public:
	void Construct(const FArguments& InArgs);
	void SetColors(const TArray<FLinearColor>& InColors);
protected:
	int32 OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const override;

protected:
	virtual void HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

protected:
	TArray<FLinearColor> Colors;
};