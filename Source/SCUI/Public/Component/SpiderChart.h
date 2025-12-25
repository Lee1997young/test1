#pragma once

#include "CoreMinimal.h"
#include "Component/GraphBase.h"

#include "SpiderChart.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnSpiderHoverEvent, bool, bDataHovering, const FGraphData&, Data, int32, LineIndex, const FGeometry&, MyGeometry, const FPointerEvent&, MouseEvent);

class SSpiderChart;

UCLASS()
class SCUI_API USpiderChart : public UGraphBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		TArray<FString> Labels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		float MinValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		float MaxValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content", meta = (ClampMin = "2", ClampMax = "12"))
		int32 RowNum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		TArray<FGraphDataLine> DataLines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		bool bDrawColorBlock;

	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
		FOnSpiderHoverEvent OnDataHover;

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabels(const TArray<FString>& InLabels);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetValue(float InMinValue, float InMaxValue, int32 InRowNum);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDatas(const TArray<FGraphDataLine>& InDataLines);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDrawColorBlock(bool bDraw);

public:
	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	virtual void HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
private:
	TSharedPtr<SSpiderChart> MyGraphWidget;
};