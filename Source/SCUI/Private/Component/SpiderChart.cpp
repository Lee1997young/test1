#include "Component/SpiderChart.h"
#include "Slate/SSpiderChart.h"
#include "SCUIColorSet.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Misc/CoreMisc.h"

#define LOCTEXT_NAMESPACE "SCUI"

USpiderChart::USpiderChart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//初始化数据
	Labels = { TEXT("a"), TEXT("b"), TEXT("c"), TEXT("d"), TEXT("e"), TEXT("f")};
	MinValue = 0.f;
	MaxValue = 100.f;
	RowNum = 3;

	TArray<FGraphData> Datas;
	for (const FString& Label : Labels)
	{
		Datas.Add(FGraphData(TEXT(""), FMath::RandRange(MinValue, MaxValue)));
	}
	DataLines.Add(FGraphDataLine(FLinearColor(FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f)), Datas));

	bDrawColorBlock = true;
}


void USpiderChart::SetLabels(const TArray<FString>& InLabels)
{
	Labels = InLabels;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLabels(InLabels);
	}
}

void USpiderChart::SetValue(float InMinValue, float InMaxValue, int32 InRowNum)
{
	MinValue = InMinValue;
	MaxValue = InMaxValue;

	InRowNum = InRowNum < 2 ? 2 : InRowNum;
	InRowNum = InRowNum > 12 ? 12 : InRowNum;
	RowNum = InRowNum;

	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetValues(MinValue, MaxValue, RowNum);
	}
}

void USpiderChart::SetDatas(const TArray<FGraphDataLine>& InDataLines)
{
	DataLines = InDataLines;
	if (MyGraphWidget.IsValid())
	{
		DataPtrs.Empty();
		for (FGraphDataLine& Data : DataLines)
		{
			DataPtrs.Add(MakeShareable(new FGraphDataLine(Data)));
		}
		MyGraphWidget->SetDatas(DataPtrs);
	}
}

void USpiderChart::SetDrawColorBlock(bool bDraw)
{
	bDrawColorBlock = bDraw;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetDrawColorBlock(bDraw);
	}
}

TSharedRef<SWidget> USpiderChart::RebuildWidget()
{
	GraphWidget = SNew(SSpiderChart)
		.OnDataHover(BIND_UOBJECT_DELEGATE(FOnDataHover, HandleOnDataHover));
	MyGraphWidget = StaticCastSharedPtr<SSpiderChart>(GraphWidget);
	return MyGraphWidget.ToSharedRef();
}

void USpiderChart::HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FGraphData Data;
	if (bHovering)
	{
		FGraphDataPtr HoverDataPtr = StaticCastSharedPtr<FGraphData>(DataPtr);
		Data = *HoverDataPtr.Get();
	}
	OnDataHover.Broadcast(bHovering, Data, DataIndex, MyGeometry, MouseEvent);
}

void USpiderChart::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyGraphWidget->SetDrawColorBlock(bDrawColorBlock);
	MyGraphWidget->SetLabels(Labels);
	MyGraphWidget->SetValues(MinValue, MaxValue, RowNum);

	DataPtrs.Empty();
	for (FGraphDataLine& Data : DataLines)
	{
		DataPtrs.Add(MakeShareable(new FGraphDataLine(Data)));
	}
	MyGraphWidget->SetDatas(DataPtrs);
}

void USpiderChart::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget.Reset();
	}
}

#undef LOCTEXT_NAMESPACE