#include "Slate/STableChart.h"
#include "SCUIStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"

#define LOCTEXT_NAMESPACE "STableChart"

void STableChart::Construct(const FArguments& InArgs)
{
	SGraphBase::Construct(InArgs);

	HeaderRowStyle = FCoreStyle::Get().GetWidgetStyle<FHeaderRowStyle>("TableView.Header");
	HeaderRowStyle.BackgroundBrush.DrawAs = ESlateBrushDrawType::Box;
	HeaderRowStyle.ColumnStyle.NormalBrush.DrawAs = ESlateBrushDrawType::NoDrawType;

	TableRowStyle = FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
	ScrollBarStyle = FCoreStyle::Get().GetWidgetStyle<FScrollBarStyle>("ScrollBar");
}

int32 STableChart::OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const
{
	const FSlateRenderTransform& Transform = InParams.Geometry.GetAccumulatedRenderTransform();
	return InParams.Layer;
}

void STableChart::SetHeader(const TArray<FString>& InHeader)
{
	TableHeader = InHeader;
	HeaderRowStyle.BackgroundBrush = HeaderBackgroundBrush;
	TSharedRef<SHeaderRow> HeaderRow = SNew(SHeaderRow)
		.Style(&HeaderRowStyle);


	for (const FString& Item : TableHeader)
	{
		if (!Item.IsEmpty())
		{
			HeaderRow->AddColumn(
				SHeaderRow::Column(FName(*Item))
				.VAlignCell(VAlign_Center)
				.HeaderContent()
				[
					SNew(STextBlock)
					.Text(FText::FromString(Item))
					.Font(this, &STableChart::GetLabelFont)
					.ColorAndOpacity(this, &STableChart::GetLabelColor)
					.Justification(this, &STableChart::GetHeaderTextAlign)
				]
			);
		}
	}

	DataBox->SetContent(
		SAssignNew(TableListView, SListView<FGraphDataTableRowPtr>)
		.ItemHeight(24)
		.ListItemsSource(&TableListItems)
		.OnGenerateRow(this, &STableChart::OnGenerateWidgetForItem)
		.SelectionMode(ESelectionMode::None)
		.HeaderRow(HeaderRow)
		.ExternalScrollbar(SNew(SScrollBar).Style(&ScrollBarStyle))
	);
}

void STableChart::SetDatas(const TArray<FGraphDataBasePtr>& InDatas)
{
	TableListItems.Empty();
	for (int32 i = 0; i < InDatas.Num(); ++i)
	{
		FGraphDataTableRowPtr RowPtr = StaticCastSharedPtr<FGraphDataTableRow>(InDatas[i]);
		TableListItems.Add(FGraphDataTableRowPtr(RowPtr));
	}

	if (TableListView.IsValid())
	{
		TableListView->RequestListRefresh();
	}
}

void STableChart::SetShowBorder(bool bShow)
{
	bShowBorder = bShow;
}

void STableChart::SetHeaderBackgroundBrush(const FSlateBrush& InBrush)
{
	HeaderBackgroundBrush = InBrush;
	ForceUpdate();
}

void STableChart::SetTableBackgroundColor(const FSlateColor& InColor)
{
	TableBackgroundColor = InColor;
	ForceUpdate();
}

void STableChart::SetOddRowBrush(const FSlateBrush& InBrush)
{
	OddRowBrush = InBrush;
	ForceUpdate();
}

void STableChart::SetEvenRowBrush(const FSlateBrush& InBrush)
{
	EvenRowBrush = InBrush;
	ForceUpdate();
}

void STableChart::SetBorderType(ETableBorderType InBorderType)
{
	BorderType = InBorderType;
	ForceUpdate();
}

void STableChart::SetRowType(ETableRowType InRowType)
{
	RowType = InRowType;
	ForceUpdate();
}

void STableChart::SetHeaderTextAlign(ETextJustify::Type InTextAlign)
{
	HeaderTextAlign = InTextAlign;
	ForceUpdate();
}

void STableChart::SetRowTextAlign(ETextJustify::Type InTextAlign)
{
	RowTextAlign = InTextAlign;
	ForceUpdate();
}

void STableChart::SetHeaderRowStyle(const FHeaderRowStyle& InStyle)
{
	HeaderRowStyle = InStyle;
}

void STableChart::SetTableRowStyle(const FTableRowStyle& InStyle)
{
	TableRowStyle = InStyle;
}

void STableChart::SetScrollBarStyle(const FScrollBarStyle& InStyle)
{
	ScrollBarStyle = InStyle;
}

TSharedRef<ITableRow> STableChart::OnGenerateWidgetForItem(FGraphDataTableRowPtr InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<SHorizontalBox> HorBox = SNew(SHorizontalBox);
	for (int32 DataIndex = 0; DataIndex < InItem->RowDatas.Num(); ++DataIndex)
	{
		HorBox->AddSlot()
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(FText::FromName(*InItem->RowDatas[DataIndex]))
				.Font(this, &STableChart::GetDataFont)
				.ColorAndOpacity(this, &STableChart::GetDataColor)
				.Justification(this, &STableChart::GetRowTextAlign)
				//.ToolTipText(this, &FLiveLinkVirtualSubjectDetailCustomization::HandleSubjectItemToolTip, InItem)
			];
	}

	return SNew(STableRow<FGraphDataTableRowPtr>, OwnerTable)
			.Style(&TableRowStyle)
		[
			SNew(SBorder)
			.BorderImage(this, &STableChart::GetBorderImage)
			.BorderBackgroundColor(this, &STableChart::GetBorderBackgroundColor)
			.Padding(0)
			[
				SNew(SBorder)
				.BorderImage(this, &STableChart::GetBorderImage)
				.BorderBackgroundColor(this, &STableChart::GetRowColor, InItem->Index)
				[
					HorBox
				]
			]
		];
}

void STableChart::ForceUpdate()
{
	if (TableListView.IsValid())
	{
		TableListView->RequestListRefresh();
	}
}

EVisibility STableChart::GetBorderVisible() const
{
	return bShowBorder ? EVisibility::Visible : EVisibility::Hidden;
}

const FSlateBrush* STableChart::GetBorderImage() const
{
	return FCoreStyle::Get().GetBrush("GenericWhiteBox");
}

FSlateColor STableChart::GetBorderBackgroundColor() const
{
	return TableBackgroundColor;
}

FSlateColor STableChart::GetRowColor(int32 InIndex) const
{
	FSlateColor ReturnColor = FSlateColor(FLinearColor(0,0,0,0));
	bool bOdd = InIndex % 2 ? false : true;
	switch (RowType)
	{
	case ETableRowType::RowNone:
		break;
	case ETableRowType::RowOdd:
		ReturnColor = bOdd ? OddRowBrush.TintColor : ReturnColor;
		break;
	case ETableRowType::RowEven:
		ReturnColor = bOdd ? ReturnColor : EvenRowBrush.TintColor;
		break;
	case ETableRowType::RowAll:
		ReturnColor = bOdd ? OddRowBrush.TintColor : EvenRowBrush.TintColor;
		break;
	}

	return ReturnColor;
}

ETextJustify::Type STableChart::GetHeaderTextAlign() const
{
	return HeaderTextAlign;
}

ETextJustify::Type STableChart::GetRowTextAlign() const
{
	return RowTextAlign;
}

#undef LOCTEXT_NAMESPACE