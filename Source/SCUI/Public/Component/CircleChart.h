#pragma once

#include "CoreMinimal.h"
#include "Component/GraphBase.h"

#include "CircleChart.generated.h"

class SCircleChart;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnCircleHoverEvent, bool, bDataHovering,const FGraphDataCircle&, Data, int32, DataIndex, const FGeometry&, MyGeometry, const FPointerEvent&, MouseEvent);

UCLASS()
class SCUI_API UCircleChart : public UGraphBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		TEnumAsByte<ECircleChartType> ChartType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		TArray<FGraphDataCircle> Datas;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Thickness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		bool bShowDataText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (EditCondition = "bShowDataText"))
		TEnumAsByte<ECircleChartDataTextType> DataTextType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (EditCondition = "DataTextType == ECircleChartDataTextType::DataValue"))
		FString DataTextSuffix;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "0.1", ClampMax = "1.0"))
		float LineLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "0.0", ClampMax = "360"))
		float StartAngle;

	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
		FOnCircleHoverEvent OnDataHover;

	UFUNCTION(BlueprintCallable, Category = "Content")
		void SetChartType(ECircleChartType InChartType);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDatas(TArray<FGraphDataCircle> InDatas);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetThickness(float InThickness);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetShowDataText(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDataTextType(ECircleChartDataTextType InChartDataTextType);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDataTextSuffix(const FString& InDataTextSuffix);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLineLength(float Length);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetStartAngle(float Angle);

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
	TSharedPtr<SCircleChart> MyGraphWidget;
};