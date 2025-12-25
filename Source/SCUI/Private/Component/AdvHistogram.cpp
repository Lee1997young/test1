#include "Component/AdvHistogram.h"
#include "Slate/SAdvHistogram.h"
#include "SCUIColorSet.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Misc/CoreMisc.h"

#define LOCTEXT_NAMESPACE "SCUI"

UAdvHistogram::UAdvHistogram(const FObjectInitializer& ObjectInitializer)
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

	for (int32 i = 0; i < 3; i++)
	{
		Colors.Add(FLinearColor(FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f)));
	}

	for (const FString& Label : Labels)
	{
		TArray<float> DataValues;
		for (int32 Index = 0; Index < 3; Index++)
		{
			DataValues.Add(FMath::RandRange(MinValue, MaxValue));
		}
		Datas.Add(FGraphDataGroup(Label, DataValues));
	}
}


void UAdvHistogram::SetOrientation(EOrientation InOrientation)
{
	Orientation = InOrientation;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetOrientation(InOrientation);
	}
}

void UAdvHistogram::SetLabels(const TArray<FString>& InLabels)
{
	Labels = InLabels;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLabels(InLabels);
	}
}

void UAdvHistogram::SetValue(float InMinValue, float InMaxValue, int32 InRowNum)
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

void UAdvHistogram::SetColors(const TArray<FLinearColor>& InColors)
{
	Colors = InColors;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetColors(InColors);
	}
}

void UAdvHistogram::SetDatas(const TArray<FGraphDataGroup>& InDatas)
{
	Datas = InDatas;
	if (MyGraphWidget.IsValid())
	{
		DataPtrs.Empty();
		for (FGraphDataGroup& Data : Datas)
		{
			DataPtrs.Add(MakeShareable(new FGraphDataGroup(Data)));
		}
		MyGraphWidget->SetDatas(DataPtrs);
	}
}

void UAdvHistogram::SetLabelTextRot(float InRot)
{
	LabelTextRot = FMath::Clamp(InRot, 0.0f, 90.0f);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLableTextRot(InRot);
	}
}

void UAdvHistogram::SetDrawGradient(bool bDraw)
{
	bDrawGradient = bDraw;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetDrawGradient(bDraw);
	}
}

TSharedRef<SWidget> UAdvHistogram::RebuildWidget()
{
	GraphWidget = SNew(SAdvHistogram)
		.OnDataHover(BIND_UOBJECT_DELEGATE(FOnDataHover, HandleOnDataHover));
	MyGraphWidget = StaticCastSharedPtr<SAdvHistogram>(GraphWidget);
	return GraphWidget.ToSharedRef();
}

void UAdvHistogram::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget.Reset();
	}
}

void UAdvHistogram::HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FGraphDataColor Data;
	if (bHovering)
	{
		FGraphDataColorPtr HoverDataPtr = StaticCastSharedPtr<FGraphDataColor>(DataPtr);
		Data = *HoverDataPtr.Get();
	}	
	OnDataHover.Broadcast(bHovering, Data, DataIndex, MyGeometry, MouseEvent);
}

void UAdvHistogram::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyGraphWidget->SetSizeScale(SizeScale);
	MyGraphWidget->SetLableTextRot(LabelTextRot);
	MyGraphWidget->SetDrawGradient(bDrawGradient);

	MyGraphWidget->SetOrientation(Orientation);
	MyGraphWidget->SetLabels(Labels);
	MyGraphWidget->SetValues(MinValue, MaxValue, RowNum);
	MyGraphWidget->SetColors(Colors);
	DataPtrs.Empty();
	for (FGraphDataGroup& Data : Datas)
	{
		DataPtrs.Add(MakeShareable(new FGraphDataGroup(Data)));
	}
	MyGraphWidget->SetDatas(DataPtrs);
}

#undef LOCTEXT_NAMESPACE