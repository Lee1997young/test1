#pragma once

#include "CoreMinimal.h"
#include "Component/GraphBase.h"

#include "Histogram.generated.h"

class SHistogram;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHistogramHoverEvent, bool, bDataHovering, const FGraphDataColor&, Data, int32, DataIndex, const FGeometry&, MyGeometry, const FPointerEvent&, MouseEvent);

UCLASS()
class SCUI_API UHistogram : public UGraphBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		TEnumAsByte<EOrientation> Orientation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		TArray<FString> Labels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		float MinValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		float MaxValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content", meta = (ClampMin = "2", ClampMax = "12"))
		int32 RowNum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		TArray<FGraphDataColor> Datas;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "-90", ClampMax = "90.0"))
		float LabelTextRot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		bool bDrawGradient;

	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
		FOnHistogramHoverEvent OnDataHover;

	UFUNCTION(BlueprintCallable, Category = "Content")
		void SetOrientation(EOrientation InOrientation);

	UFUNCTION(BlueprintCallable, Category = "Content")
		void SetLabels(const TArray<FString>& InLabels);

	UFUNCTION(BlueprintCallable, Category = "Content")
		void SetValue(float InMinValue, float InMaxValue, int32 InRowNum);

	UFUNCTION(BlueprintCallable, Category = "Content")
		void SetDatas(const TArray<FGraphDataColor>& InDatas);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabelTextRot(float InRot);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDrawGradient(bool bDraw);
public:

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

		//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	virtual void HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
private:
	TSharedPtr<SHistogram> MyGraphWidget;
};