#pragma once
#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Slate/SGraphBase.h"
#include "SCUITypes.h"
#include "GraphBase.generated.h"

UCLASS(ABSTRACT)
class SCUI_API UGraphBase : public UWidget
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		FSlateFontInfo LabelFont;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		FSlateColor LabelTextColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		FSlateFontInfo ValueFont;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		FSlateColor ValueTextColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		FSlateFontInfo DataFont;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		FSlateColor DataTextColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
		FSlateColor GridColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (ClampMin = "0.2", ClampMax = "2.0"))
		float SizeScale;

	UPROPERTY(EditAnywhere, Category = "Interface")
		TSubclassOf<UUserWidget> WidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabelTextColorAndOpacity(FSlateColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabelTextOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetValueTextColorAndOpacity(FSlateColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetValueTextOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDataTextColorAndOpacity(FSlateColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetDataTextOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetGridColorAndOpacity(FSlateColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetGridOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetLabelFont(FSlateFontInfo InFontInfo);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetSizeScale(float InSizeScale);

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayAnim(float Duration = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void SetAnimProgress(float InProgress = 1.f);

	UFUNCTION(BlueprintPure, Category = "Animation")
		float GetAnimProgress();
public:

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	virtual void HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

protected:
	TSharedPtr<SGraphBase> GraphWidget;
	TArray<TSharedPtr<FGraphDataBase>> DataPtrs;
};