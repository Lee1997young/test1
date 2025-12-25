#pragma once

#include "CoreMinimal.h"
#include "Component/GraphBase.h"

#include "LinearGraph.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnLinearHoverEvent, bool, bDataHovering, const FGraphData&, Data, int32, LineIndex, const FGeometry&, MyGeometry, const FPointerEvent&, MouseEvent);

class SLinearGraph;

UCLASS()
class SCUI_API ULinearGraph : public UGraphBase
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		TArray<FString> HideLabels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "-90", ClampMax = "90.0"))
		float LabelTextRot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		bool bSplineType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (EditCondition = "bSplineType", ClampMin = "0.1", ClampMax = "1.0"))
		float SplineTangentScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (EditCondition = "bSplineType", ClampMin = "0.0", ClampMax = "1.0"))
		float SplinePointSizeScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		bool bDrawColorBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (EditCondition = "bDrawColorBlock"))
		bool bDrawGradient;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		bool bShowLabelsData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		FSlateFontInfo LabelsDataFont;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		FSlateColor LabelsDataTextColorAndOpacity;

	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
		FOnLinearHoverEvent OnDataHover;

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabels(const TArray<FString>& InLabels);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetValue(float InMinValue, float InMaxValue,  int32 InRowNum);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDatas(const TArray<FGraphDataLine>& InDataLines);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetHideLabels(const TArray<FString>& InHideLabels);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabelTextRot(float InRot);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetSplineType(bool bSpline);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetSplineTangentScale(float InTangentScale);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetSplinePointSizeScale(float InSplinePointSizeScale);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDrawColorBlock(bool bDraw);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDrawGradient(bool bDraw);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabelsDataFont(FSlateFontInfo InFontInfo);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabelsDataTextColorAndOpacity(FSlateColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabelsDataTextOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetShowLabelsData(bool bShow);


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
	TSharedPtr<SLinearGraph> MyGraphWidget;
};