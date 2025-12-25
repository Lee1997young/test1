#pragma once

#include "CoreMinimal.h"
#include "Component/GraphBase.h"
#include "Framework/Text/TextLayout.h"
#include "TableChart.generated.h"

class STableChart;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnTableHoverEvent, bool, bDataHovering,const FGraphDataTable&, Data, int32, DataIndex, const FGeometry&, MyGeometry, const FPointerEvent&, MouseEvent);

UCLASS()
class SCUI_API UTableChart : public UGraphBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		TArray<FString> Header;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
		TArray<FGraphDataTableRow> Rows;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Thickness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		TEnumAsByte<ETableBorderType> BorderType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		TEnumAsByte<ETableRowType> RowType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		TEnumAsByte<ETextJustify::Type> HeaderTextAlign;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		TEnumAsByte<ETextJustify::Type> RowTextAlign;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		FSlateBrush HeaderBackgroundBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		FSlateColor TableBackgroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		FSlateBrush OddRowBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance"/*, AdvancedDisplay*/)
		FSlateBrush EvenRowBrush;

	//UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	//	FOnTableHoverEvent OnDataHover;

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetHeader(TArray<FString> InHeader);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDatas(TArray<FGraphDataTableRow> InDatas);

	UFUNCTION(BlueprintCallable, Category = "Content")
		void SetBorderType(ETableBorderType InBorderType);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetRowType(ETableRowType InRowType);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetHeaderTextAlign(ETextJustify::Type InTextAlign);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetRowTextAlign(ETextJustify::Type InTextAlign);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetTableBackgroundColor(FLinearColor InColor);

	//UFUNCTION(BlueprintCallable, Category = "Appearance")
	//	void SetStripeColor(FLinearColor InColor);
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

	//virtual void HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
private:
	TSharedPtr<STableChart> MyGraphWidget;
};