#include "Component/GraphBase.h"
#include "Slate/SGraphBase.h"
#include "SCUIColorSet.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Misc/CoreMisc.h"
#include "..\..\Public\Component\GraphBase.h"

#define LOCTEXT_NAMESPACE "SCUI"

UGraphBase::UGraphBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LabelTextColorAndOpacity = FLinearColor::White;
	ValueTextColorAndOpacity = FLinearColor::White;
	DataTextColorAndOpacity = FLinearColor::White;
	GridColorAndOpacity = FLinearColor::Gray;
	SizeScale = 1.f;
	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
		LabelFont = ValueFont = DataFont = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Bold"));
	}
}

void UGraphBase::SetLabelTextColorAndOpacity(FSlateColor InColorAndOpacity)
{
	LabelTextColorAndOpacity = InColorAndOpacity;
	if (GraphWidget.IsValid())
	{
		GraphWidget->SetLabelTextColorAndOpacity(InColorAndOpacity);
	}
}

void UGraphBase::SetLabelTextOpacity(float InOpacity)
{
	FLinearColor CurrentColor = LabelTextColorAndOpacity.GetSpecifiedColor();
	CurrentColor.A = InOpacity;

	SetLabelTextColorAndOpacity(FSlateColor(CurrentColor));
}

void UGraphBase::SetValueTextColorAndOpacity(FSlateColor InColorAndOpacity)
{
	ValueTextColorAndOpacity = InColorAndOpacity;
	if (GraphWidget.IsValid())
	{
		GraphWidget->SetValueTextColorAndOpacity(InColorAndOpacity);
	}
}

void UGraphBase::SetValueTextOpacity(float InOpacity)
{
	FLinearColor CurrentColor = ValueTextColorAndOpacity.GetSpecifiedColor();
	CurrentColor.A = InOpacity;

	SetValueTextColorAndOpacity(FSlateColor(CurrentColor));
}

void UGraphBase::SetDataTextColorAndOpacity(FSlateColor InColorAndOpacity)
{
	DataTextColorAndOpacity = InColorAndOpacity;
	if (GraphWidget.IsValid())
	{
		GraphWidget->SetDataTextColorAndOpacity(InColorAndOpacity);
	}
}

void UGraphBase::SetDataTextOpacity(float InOpacity)
{
	FLinearColor CurrentColor = DataTextColorAndOpacity.GetSpecifiedColor();
	CurrentColor.A = InOpacity;

	SetDataTextColorAndOpacity(FSlateColor(CurrentColor));
}

void UGraphBase::SetGridColorAndOpacity(FSlateColor InColorAndOpacity)
{
	GridColorAndOpacity = InColorAndOpacity;
	if (GraphWidget.IsValid())
	{
		GraphWidget->SetGridColorAndOpacity(InColorAndOpacity);
	}
}

void UGraphBase::SetGridOpacity(float InOpacity)
{
	FLinearColor CurrentColor = GridColorAndOpacity.GetSpecifiedColor();
	CurrentColor.A = InOpacity;

	SetGridColorAndOpacity(FSlateColor(CurrentColor));
}

void UGraphBase::SetLabelFont(FSlateFontInfo InFontInfo)
{
	LabelFont = InFontInfo;
	if (GraphWidget.IsValid())
	{
		GraphWidget->SetLabelFont(LabelFont);
	}
}

void UGraphBase::SetSizeScale(float InSizeScale)
{
	SizeScale = FMath::Clamp(InSizeScale, 0.1f, 2.0f);
	if (GraphWidget.IsValid())
	{
		GraphWidget->SetSizeScale(InSizeScale);
	}
}

void UGraphBase::PlayAnim(float Duration /*= 2.0f*/)
{
	if (GraphWidget.IsValid())
	{
		GraphWidget->PlayAnim(Duration);
	}
}

void UGraphBase::SetAnimProgress(float InProgress)
{
	if (GraphWidget.IsValid())
	{
		GraphWidget->SetAnimProgress(InProgress);
	}
}

float UGraphBase::GetAnimProgress()
{
	if (GraphWidget.IsValid())
	{
		return GraphWidget->GetAnimProgress();
	}
	return 0.f;
}

void UGraphBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	GraphWidget->SetLabelFont(LabelFont);
	GraphWidget->SetLabelTextColorAndOpacity(LabelTextColorAndOpacity);

	GraphWidget->SetValueFont(ValueFont);
	GraphWidget->SetValueTextColorAndOpacity(ValueTextColorAndOpacity);

	GraphWidget->SetDataFont(DataFont);
	GraphWidget->SetDataTextColorAndOpacity(DataTextColorAndOpacity);

	GraphWidget->SetGridColorAndOpacity(GridColorAndOpacity);
	GraphWidget->SetSizeScale(SizeScale);
}

void UGraphBase::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (GraphWidget.IsValid())
	{
		GraphWidget.Reset();
	}
	DataPtrs.Reset();
}

#if WITH_EDITOR
const FText UGraphBase::GetPaletteCategory()
{
	return LOCTEXT("SCUI", "SCUI");
}
#endif

TSharedRef<SWidget> UGraphBase::RebuildWidget()
{
	GraphWidget = SNew(SGraphBase)
		.OnDataHover(BIND_UOBJECT_DELEGATE(FOnDataHover, HandleOnDataHover));
	return GraphWidget.ToSharedRef();
}

void UGraphBase::HandleOnDataHover(bool bHovering, FGraphDataBasePtr DataPtr, int32 DataIndex, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//OnDataHover.Broadcast(*DataPtr.Get());
}

#undef LOCTEXT_NAMESPACE

