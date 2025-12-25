#include "Component/CircleChart.h"
#include "Slate/SCircleChart.h"
#include "SCUIColorSet.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Misc/CoreMisc.h"

#define LOCTEXT_NAMESPACE "SCUI"

UCircleChart::UCircleChart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ChartType(ECircleChartType::Pie)
	, DataTextType(ECircleChartDataTextType::DataPercent)
{
	//Init Data
	TArray<FString> Labels = { TEXT("a"), TEXT("b"), TEXT("c") };
	int32 Value = 3;
	for (const FString& Label : Labels)
	{
		Datas.Add(FGraphDataCircle(
			Label,
			(Value++),
			FLinearColor(FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f)),
			1.0f)
		);
	}

	Thickness = 1.f;
	bShowDataText = true;
	LineLength = 0.2f;
	StartAngle = 0.f;
}

void UCircleChart::SetChartType(ECircleChartType InChartType)
{
	ChartType = InChartType;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetChartType(InChartType);
	}
}

void UCircleChart::SetDatas(TArray<FGraphDataCircle> InDatas)
{
	for (FGraphDataCircle& Data : InDatas)
	{
		if (Data.Value < 0) { Data.Value = 0; }
	}

	Datas = InDatas;
	if (MyGraphWidget.IsValid())
	{
		DataPtrs.Empty();
		for (FGraphDataCircle& Data : Datas)
		{
			DataPtrs.Add(MakeShareable(new FGraphDataCircle(Data)));
		}
		MyGraphWidget->SetDatas(DataPtrs);
	}
}

void UCircleChart::SetThickness(float InThickness)
{
	Thickness = InThickness;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetThickness(Thickness);
	}
}

void UCircleChart::SetShowDataText(bool bShow)
{
	bShowDataText = bShow;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetShowDataText(bShow);
	}
}

void UCircleChart::SetDataTextType(ECircleChartDataTextType InChartDataTextType)
{
	DataTextType = InChartDataTextType;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetDataTextType(InChartDataTextType);
	}
}

void UCircleChart::SetDataTextSuffix(const FString& InDataTextSuffix)
{
	DataTextSuffix = InDataTextSuffix;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetDataTextSuffix(InDataTextSuffix);
	}
}

void UCircleChart::SetLineLength(float Length)
{
	LineLength = FMath::Clamp(Length, 0.1f, 1.f);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLineLength(LineLength);
	}
}

void UCircleChart::SetStartAngle(float Angle)
{
	StartAngle = FMath::Clamp(Angle, 0.f, 360.f);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetStartAngle(Angle);
	}
}

TSharedRef<SWidget> UCircleChart::RebuildWidget()
{
	GraphWidget = SNew(SCircleChart)
		.OnDataHover(BIND_UOBJECT_DELEGATE(FOnDataHover, HandleOnDataHover));
	MyGraphWidget = StaticCastSharedPtr<SCircleChart>(GraphWidget);
	return GraphWidget.ToSharedRef();
}

void UCircleChart::HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FGraphDataCircle Data;
	if (bHovering)
	{
		FGraphDataCirclePtr HoverDataPtr = StaticCastSharedPtr<FGraphDataCircle>(DataPtr);
		Data = *HoverDataPtr.Get();
	}
	OnDataHover.Broadcast(bHovering, Data, DataIndex, MyGeometry, MouseEvent);
}

void UCircleChart::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	MyGraphWidget->SetThickness(Thickness);
	MyGraphWidget->SetShowDataText(bShowDataText);
	MyGraphWidget->SetDataTextType(DataTextType);
	MyGraphWidget->SetDataTextSuffix(DataTextSuffix);
	MyGraphWidget->SetLineLength(LineLength);
	MyGraphWidget->SetStartAngle(StartAngle);
	MyGraphWidget->SetChartType(ChartType);

	DataPtrs.Empty();
	for (FGraphDataCircle& Data : Datas)
	{
		if (Data.Value < 0) { Data.Value = 0; }
		DataPtrs.Add(MakeShareable(new FGraphDataCircle(Data)));
	}
	MyGraphWidget->SetDatas(DataPtrs);
}

void UCircleChart::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget.Reset();
	}
}

#undef LOCTEXT_NAMESPACE