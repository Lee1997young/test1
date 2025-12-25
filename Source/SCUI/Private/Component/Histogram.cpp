#include "Component/Histogram.h"
#include "Slate/SHistogram.h"
#include "SCUIColorSet.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Misc/CoreMisc.h"

#define LOCTEXT_NAMESPACE "SCUI"

UHistogram::UHistogram(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Orientation(Orient_Horizontal)
{

	//Init Data
	Labels = { TEXT("a"), TEXT("b"), TEXT("c"), TEXT("d"), TEXT("e"), TEXT("f"), TEXT("g") };
	MinValue = 0.f;
	MaxValue = 100.f;
	RowNum = 3;
	//LabelTextRot = 0.f;
	bDrawGradient = true;

	for (const FString& Label : Labels)
	{
		Datas.Add(FGraphDataColor(
			Label,
			(int32)FMath::RandRange(MinValue, MaxValue),
			FLinearColor(FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f))));
	}
}


void UHistogram::SetOrientation(EOrientation InOrientation)
{
	Orientation = InOrientation;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetOrientation(InOrientation);
	}
}

void UHistogram::SetLabels(const TArray<FString>& InLabels)
{
	Labels = InLabels;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLabels(InLabels);
	}
}

void UHistogram::SetValue(float InMinValue, float InMaxValue, int32 InRowNum)
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

void UHistogram::SetDatas(const TArray<FGraphDataColor>& InDatas)
{
	Datas = InDatas;
	if (MyGraphWidget.IsValid())
	{
		DataPtrs.Empty();
		for (FGraphDataColor& Data : Datas)
		{
			DataPtrs.Add(MakeShareable(new FGraphDataColor(Data)));
		}
		MyGraphWidget->SetDatas(DataPtrs);
	}
}

void UHistogram::SetLabelTextRot(float InRot)
{
	LabelTextRot = FMath::Clamp(InRot, 0.0f, 90.0f);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLableTextRot(InRot);
	}
}

void UHistogram::SetDrawGradient(bool bDraw)
{
	bDrawGradient = bDraw;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetDrawGradient(bDraw);
	}
}

TSharedRef<SWidget> UHistogram::RebuildWidget()
{
	GraphWidget = SNew(SHistogram)
		.OnDataHover(BIND_UOBJECT_DELEGATE(FOnDataHover, HandleOnDataHover));
	MyGraphWidget = StaticCastSharedPtr<SHistogram>(GraphWidget);
	return GraphWidget.ToSharedRef();
}

void UHistogram::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget.Reset();
	}
}

void UHistogram::HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FGraphDataColor Data;
	if (bHovering)
	{
		FGraphDataColorPtr HoverDataPtr = StaticCastSharedPtr<FGraphDataColor>(DataPtr);
		Data = *HoverDataPtr.Get();
	}	
	OnDataHover.Broadcast(bHovering, Data, DataIndex, MyGeometry, MouseEvent);
}

void UHistogram::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyGraphWidget->SetSizeScale(SizeScale);
	MyGraphWidget->SetLableTextRot(LabelTextRot);
	MyGraphWidget->SetDrawGradient(bDrawGradient);

	MyGraphWidget->SetOrientation(Orientation);
	MyGraphWidget->SetLabels(Labels);
	MyGraphWidget->SetValues(MinValue, MaxValue, RowNum);

	DataPtrs.Empty();
	for (FGraphDataColor& Data : Datas)
	{
		DataPtrs.Add(MakeShareable(new FGraphDataColor(Data)));
	}
	MyGraphWidget->SetDatas(DataPtrs);
}

#undef LOCTEXT_NAMESPACE