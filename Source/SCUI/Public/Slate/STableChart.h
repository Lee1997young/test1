#pragma once

#include "CoreMinimal.h"
#include "SGraphBase.h"
#include "SCUITypes.h"
#include "Widgets/Views/SListView.h"

class SCUI_API STableChart : public SGraphBase
{
public:
	void Construct(const FArguments& InArgs);

	virtual int32 OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const;

	void SetHeader(const TArray<FString>& InHeader);
	void SetDatas(const TArray<FGraphDataBasePtr>& InDatas);
	void SetShowBorder(bool bShow);
	void SetHeaderBackgroundBrush(const FSlateBrush& InBrush);
	void SetTableBackgroundColor(const FSlateColor& InColor);
	void SetOddRowBrush(const FSlateBrush& InBrush);
	void SetEvenRowBrush(const FSlateBrush& InBrush);
	void SetBorderType(ETableBorderType InBorderType);
	void SetRowType(ETableRowType InRowType);
	void SetHeaderTextAlign(ETextJustify::Type InTextAlign);
	void SetRowTextAlign(ETextJustify::Type InTextAlign);
	void SetHeaderRowStyle(const FHeaderRowStyle& InStyle);
	void SetTableRowStyle(const FTableRowStyle& InStyle);
	void SetScrollBarStyle(const FScrollBarStyle& InStyle);
protected:
	//virtual void HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	TSharedRef<ITableRow> OnGenerateWidgetForItem(FGraphDataTableRowPtr InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void ForceUpdate();

	EVisibility GetBorderVisible() const;
	const FSlateBrush* GetBorderImage() const;
	FSlateColor GetBorderBackgroundColor() const;
	FSlateColor GetRowColor(int32 InIndex) const;
	ETextJustify::Type GetHeaderTextAlign() const;
	ETextJustify::Type GetRowTextAlign() const;
protected:
	TArray<FString> TableHeader;
	TArray<FGraphDataTableRowPtr> TableListItems;
	TArray<FGraphDataBasePtr> OriginDatas;
	TSharedPtr< SListView<FGraphDataTableRowPtr>> TableListView;
	bool bShowBorder;
	FSlateBrush HeaderBackgroundBrush;
	FSlateColor TableBackgroundColor;
	FSlateBrush OddRowBrush;
	FSlateBrush EvenRowBrush;
	ETableRowType RowType;
	ETableBorderType BorderType;
	ETextJustify::Type HeaderTextAlign;
	ETextJustify::Type RowTextAlign;
	FHeaderRowStyle HeaderRowStyle;
	FTableRowStyle TableRowStyle;
	FScrollBarStyle ScrollBarStyle;
};