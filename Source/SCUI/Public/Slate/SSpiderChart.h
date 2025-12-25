#pragma once

#include "CoreMinimal.h"
#include "SCoordGraph.h"
#include "SCUITypes.h"

class SCUI_API SSpiderChart : public SCoordGraph
{
public:
	void Construct(const FArguments& InArgs);

	int32 OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const override;
	
	virtual void HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
private:
	FVector2D GetPaintLoc(int32 Index, float InRadius, const FVector2D& InCenter) const;
	int32 DrawGridlines(const FOnPaintHandlerParams& InParams) const;
	int32 DrawLabelText(const FOnPaintHandlerParams& InParams) const;
	int32 DrawDatas(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const;
};