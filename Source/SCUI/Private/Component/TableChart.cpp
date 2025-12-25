#include "Component/TableChart.h"
#include "Slate/STableChart.h"
#include "SCUIColorSet.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Misc/CoreMisc.h"

#define LOCTEXT_NAMESPACE "SCUI"

UTableChart::UTableChart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, BorderType(ETableBorderType::BorderNone)
	, RowType(ETableRowType::RowNone)
{
	//Init Data
	Header = { TEXT("#"), TEXT("name"), TEXT("age") };
	for (int32 RowIndex = 0; RowIndex < 3; ++RowIndex)
	{
		TArray<FString> RowDatas;
		RowDatas.Add(FString::FromInt(RowIndex));
		RowDatas.Add(TEXT("name"));
		RowDatas.Add(FString::FromInt(RowIndex));
		
		Rows.Add(FGraphDataTableRow(
			FLinearColor(FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f)),
			RowDatas)
		);
	}

	HeaderBackgroundBrush = FSlateBrush();
	HeaderBackgroundBrush.TintColor = FSlateColor(FLinearColor::Black);
	TableBackgroundColor = FLinearColor::Black;

	LabelTextColorAndOpacity = FLinearColor::White;
	DataTextColorAndOpacity = FLinearColor::White;
	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
		LabelFont = FSlateFontInfo(RobotoFontObj.Object, 20, FName("Bold"));
		DataFont = FSlateFontInfo(RobotoFontObj.Object, 16, FName("Bold"));
	}
}

void UTableChart::SetHeader(TArray<FString> InHeader)
{
	Header = InHeader;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetHeader(InHeader);
	}
}

void UTableChart::SetDatas(TArray<FGraphDataTableRow> InDatas)
{
	Rows = InDatas;
	if (MyGraphWidget.IsValid())
	{
		DataPtrs.Empty();
		for (int32 RowIndex = 0; RowIndex < Rows.Num(); ++RowIndex)
		{
			DataPtrs.Add(MakeShareable(new FGraphDataTableRow(RowIndex, Rows[RowIndex].RowColor, Rows[RowIndex].RowDatas)));
		}
		MyGraphWidget->SetDatas(DataPtrs);
	}
}

void UTableChart::SetBorderType(ETableBorderType InBorderType)
{
	BorderType = InBorderType;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetBorderType(BorderType);
	}
}

void UTableChart::SetRowType(ETableRowType InStripeType)
{
	RowType = InStripeType;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetRowType(RowType);
	}
}

void UTableChart::SetHeaderTextAlign(ETextJustify::Type InTextAlign)
{
	HeaderTextAlign = InTextAlign;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetHeaderTextAlign(HeaderTextAlign);
	}
}

void UTableChart::SetRowTextAlign(ETextJustify::Type InTextAlign)
{
	RowTextAlign = InTextAlign;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetRowTextAlign(RowTextAlign);
	}
}

void UTableChart::SetTableBackgroundColor(FLinearColor InColor)
{
	TableBackgroundColor = InColor;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetTableBackgroundColor(InColor);
	}
}

TSharedRef<SWidget> UTableChart::RebuildWidget()
{
	GraphWidget = SNew(STableChart)
		.GraphPadding(FMargin(0))
		.OnDataHover(BIND_UOBJECT_DELEGATE(FOnDataHover, HandleOnDataHover));
	MyGraphWidget = StaticCastSharedPtr<STableChart>(GraphWidget);
	return GraphWidget.ToSharedRef();
}

//void UTableChart::HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	FGraphDataCircle Data;
//	if (bHovering)
//	{
//		FGraphDataCirclePtr HoverDataPtr = StaticCastSharedPtr<FGraphDataCircle>(DataPtr);
//		Data = *HoverDataPtr.Get();
//	}
//	OnDataHover.Broadcast(bHovering, Data, DataIndex, MyGeometry, MouseEvent);
//}

void UTableChart::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	MyGraphWidget->SetHeaderBackgroundBrush(HeaderBackgroundBrush);
	MyGraphWidget->SetTableBackgroundColor(TableBackgroundColor);
	MyGraphWidget->SetOddRowBrush(OddRowBrush);
	MyGraphWidget->SetEvenRowBrush(EvenRowBrush);
	MyGraphWidget->SetBorderType(BorderType);
	MyGraphWidget->SetRowType(RowType);
	MyGraphWidget->SetHeaderTextAlign(HeaderTextAlign);
	MyGraphWidget->SetRowTextAlign(RowTextAlign);
	
	DataPtrs.Empty();
	for (int32 RowIndex = 0; RowIndex < Rows.Num(); ++RowIndex)
	{ 
		DataPtrs.Add(MakeShareable(new FGraphDataTableRow(RowIndex, Rows[RowIndex].RowColor, Rows[RowIndex].RowDatas)));
	}
	MyGraphWidget->SetDatas(DataPtrs);
	MyGraphWidget->SetHeader(Header);
}

void UTableChart::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget.Reset();
	}
}

#undef LOCTEXT_NAMESPACE