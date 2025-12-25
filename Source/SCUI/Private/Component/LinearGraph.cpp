#include "Component/LinearGraph.h"
#include "Slate/SLinearGraph.h"
#include "SCUIColorSet.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Misc/CoreMisc.h"

#define LOCTEXT_NAMESPACE "SCUI"

ULinearGraph::ULinearGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Init Data
	Labels = { TEXT("a"), TEXT("b"), TEXT("c"), TEXT("d"), TEXT("e"), TEXT("f"), TEXT("g") };
	MinValue = 0.f;
	MaxValue = 100.f;
	RowNum = 3;
	//LabelTextRot = 0.f;
	bSplineType = false;
	SplineTangentScale = 0.2f;
	SplinePointSizeScale = 0.5f;
	bDrawColorBlock = true;
	bDrawGradient = true;
	bShowLabelsData = false;

	LabelsDataTextColorAndOpacity = FLinearColor::White;
	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
		LabelsDataFont = FSlateFontInfo(RobotoFontObj.Object, 16, FName("Bold"));
	}

	TArray<FGraphData> Datas;
	for (const FString& Label : Labels)
	{
		Datas.Add(FGraphData(Label, FMath::RandRange(MinValue, MaxValue)));
	}
	DataLines.Add(FGraphDataLine(FLinearColor(FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f)) , Datas));

}

void ULinearGraph::SetLabels(const TArray<FString>& InLabels)
{
	Labels = InLabels;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLabels(InLabels);
	}
}

void ULinearGraph::SetValue(float InMinValue, float InMaxValue, int32 InRowNum)
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

void ULinearGraph::SetDatas(const TArray<FGraphDataLine>& InDataLines)
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

void ULinearGraph::SetHideLabels(const TArray<FString>& InLabels)
{
	HideLabels = InLabels;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetHideLabels(InLabels);
	}
}

void ULinearGraph::SetLabelTextRot(float InRot)
{
	LabelTextRot = FMath::Clamp(InRot, 0.0f, 90.0f);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLableTextRot(InRot);
	}
}

void ULinearGraph::SetSplineType(bool bSpline)
{ 
	bSplineType = bSpline;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetSplineType(bSpline);
	}
}

void ULinearGraph::SetSplineTangentScale(float InTangentScale)
{
	SplineTangentScale = InTangentScale;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetSplineTangentScale(InTangentScale);
	}
}

void ULinearGraph::SetSplinePointSizeScale(float InSplinePointSizeScale)
{
	SplinePointSizeScale = InSplinePointSizeScale;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetSplinePointSizeScale(InSplinePointSizeScale);
	}
}

void ULinearGraph::SetDrawColorBlock(bool bDraw)
{
	bDrawColorBlock = bDraw;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetDrawColorBlock(bDraw);
	}
}

void ULinearGraph::SetDrawGradient(bool bDraw)
{
	bDrawGradient = bDraw;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetDrawGradient(bDraw);
	}
}

void ULinearGraph::SetLabelsDataFont(FSlateFontInfo InFontInfo)
{
	LabelsDataFont = InFontInfo;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLabelsDataFont(LabelsDataFont);
	}
}

void ULinearGraph::SetLabelsDataTextColorAndOpacity(FSlateColor InColorAndOpacity)
{
	LabelsDataTextColorAndOpacity = InColorAndOpacity;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetLabelsDataTextColorAndOpacity(InColorAndOpacity);
	}
}

void ULinearGraph::SetLabelsDataTextOpacity(float InOpacity)
{
	FLinearColor CurrentColor = LabelsDataTextColorAndOpacity.GetSpecifiedColor();
	CurrentColor.A = InOpacity;

	SetLabelsDataTextColorAndOpacity(FSlateColor(CurrentColor));
}

void ULinearGraph::SetShowLabelsData(bool bShow)
{
	bShowLabelsData = bShow;
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget->SetShowLabelsData(bShow);
	}
}

TSharedRef<SWidget> ULinearGraph::RebuildWidget()
{
	GraphWidget = SNew(SLinearGraph)
		.OnDataHover(BIND_UOBJECT_DELEGATE(FOnDataHover, HandleOnDataHover));
	MyGraphWidget = StaticCastSharedPtr<SLinearGraph>(GraphWidget);
	return GraphWidget.ToSharedRef();
}

void ULinearGraph::HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FGraphData Data;
	if (bHovering)
	{
		FGraphDataPtr HoverDataPtr = StaticCastSharedPtr<FGraphData>(DataPtr);
		Data = *HoverDataPtr.Get();
	}
	OnDataHover.Broadcast(bHovering, Data, DataIndex, MyGeometry, MouseEvent);
}

void ULinearGraph::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyGraphWidget->SetSplineType(bSplineType);
	MyGraphWidget->SetSplineTangentScale(SplineTangentScale);
	MyGraphWidget->SetSplinePointSizeScale(SplinePointSizeScale);
	MyGraphWidget->SetDrawColorBlock(bDrawColorBlock);
	MyGraphWidget->SetDrawGradient(bDrawGradient);
	MyGraphWidget->SetShowLabelsData(bShowLabelsData);
	MyGraphWidget->SetLabelsDataFont(LabelsDataFont);
	MyGraphWidget->SetLabelsDataTextColorAndOpacity(LabelsDataTextColorAndOpacity);
	MyGraphWidget->SetLableTextRot(LabelTextRot);
	MyGraphWidget->SetHideLabels(HideLabels);
	MyGraphWidget->SetLabels(Labels);
	MyGraphWidget->SetValues(MinValue, MaxValue, RowNum);

	DataPtrs.Empty();
	for (FGraphDataLine& Data : DataLines)
	{
		DataPtrs.Add(MakeShareable(new FGraphDataLine(Data)));
	}
	MyGraphWidget->SetDatas(DataPtrs);

}

void ULinearGraph::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (MyGraphWidget.IsValid())
	{
		MyGraphWidget.Reset();
	}
}

#undef LOCTEXT_NAMESPACE