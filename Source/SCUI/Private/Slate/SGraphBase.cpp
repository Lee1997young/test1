#include "Slate/SGraphBase.h"
#include "SCUIStyle.h"
#include "SCUIMacro.h"
#include "Widgets/Layout/SBorder.h"
#include "..\..\Public\Slate\SGraphBase.h"

#define LOCTEXT_NAMESPACE "SGraphBase"

void SGraphBase::Construct(const FArguments& InArgs)
{
	GraphPadding = InArgs._GraphPadding;
	ChildSlot
	[
		SNew(SBorder)
		.Padding(GraphPadding)
		.BorderBackgroundColor(FLinearColor(0.f,0.f,0.f,0.f))
		[
			SAssignNew(DataBox, SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
		]
	];

	OnDataHover = InArgs._OnDataHover;
}

FReply SGraphBase::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Unhandled();
}

FReply SGraphBase::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Unhandled();
}

FReply SGraphBase::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	MousePosition = DataBox->GetTickSpaceGeometry().AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	HoverDataCheck(MyGeometry, MouseEvent);
	return SCompoundWidget::OnMouseMove(MyGeometry, MouseEvent);
}

void SGraphBase::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
}

void SGraphBase::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseLeave(MouseEvent);
}

int32 SGraphBase::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	WidgetStyle = InWidgetStyle;
	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	LayerId++;
	FOnPaintHandlerParams Params(DataBox->GetPaintSpaceGeometry(), MyCullingRect, OutDrawElements, LayerId, bParentEnabled && IsEnabled());
	
	//float InAnimAlpha = IsAnimPlaying ? (TimeInterval / AnimDuration) : AnimProgress;
	OnPaintAnim(Params, IsAnimPlaying, AnimProgress);

	return LayerId;
}

void SGraphBase::SetDatas(const TArray<TSharedPtr<FGraphDataBase>>& InDatas)
{
	DataPtrs = InDatas;
}

void SGraphBase::SetLabelFont(const FSlateFontInfo& InFontInfo)
{
	LabelFont = InFontInfo;
}

void SGraphBase::SetLabelTextColorAndOpacity(const FSlateColor& InColorAndOpacity)
{
	LabelColor = InColorAndOpacity;
}

void SGraphBase::SetValueFont(const FSlateFontInfo& InFontInfo)
{
	ValueFont = InFontInfo;
}

void SGraphBase::SetValueTextColorAndOpacity(const FSlateColor& InColorAndOpacity)
{
	ValueColor = InColorAndOpacity;
}

void SGraphBase::SetDataFont(const FSlateFontInfo& InFontInfo)
{
	DataFont = InFontInfo;
}

void SGraphBase::SetDataTextColorAndOpacity(const FSlateColor& InColorAndOpacity)
{
	DataColor = InColorAndOpacity;
}

void SGraphBase::SetGridColorAndOpacity(const FSlateColor& InColorAndOpacity)
{
	GridColor = InColorAndOpacity;
}

void SGraphBase::SetSizeScale(float InSizeScale)
{
	SizeScale = InSizeScale;
}

FSlateFontInfo SGraphBase::GetLabelFont() const
{
	return LabelFont;
}

FSlateColor SGraphBase::GetLabelColor() const
{
	return LabelColor;
}

FSlateFontInfo SGraphBase::GetValueFont() const
{
	return ValueFont;
}

FSlateColor SGraphBase::GetValueColor() const
{
	return ValueColor;
}

FSlateFontInfo SGraphBase::GetDataFont() const
{
	return DataFont;
}

FSlateColor SGraphBase::GetDataColor() const
{
	return DataColor;
}

FSlateColor SGraphBase::GetGridColor() const
{
	return GridColor;
}

float SGraphBase::GetWidgetOpacity() const
{
	return FMath::Clamp(WidgetStyle.GetColorAndOpacityTint().A, 0.f, 1.f);
}

int32 SGraphBase::OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const
{
	return InParams.Layer;
}

void SGraphBase::DrawLine(const FOnPaintHandlerParams& InParams, const FVector2D& InPoint1, const FVector2D& InPoint2, const FLinearColor& InLinearColor /*= FLinearColor::White*/, float InWidth /*= 1.0f*/) const
{
	TArray<FVector2D> LinePoints{ InPoint1 , InPoint2 };

	FSlateDrawElement::MakeLines
	(
		InParams.OutDrawElements,
		InParams.Layer,
		InParams.Geometry.ToPaintGeometry(),
		LinePoints,
		ESlateDrawEffect::None,
		InLinearColor * GetWidgetOpacity(),
		true,
		InWidth
	);
}

int32 SGraphBase::DrawGradient(const FOnPaintHandlerParams& InParams, const FQuadPoints& QuadPoints, const FQuadPoints& QuadPointsUV, const FSlateResourceHandle& InHandle, const FLinearColor& InColor) const
{
	FColor VertColor = InColor.ToFColor(true);
	VertColor.A *= 0.6 * GetWidgetOpacity();

	TArray<FSlateVertex> Verts;
	Verts.Reserve(4);

	AddSlateVertex(Verts, FVector2f(InParams.Geometry.LocalToAbsolute(QuadPoints.LeftTop)), QuadPointsUV.LeftTop);
	AddSlateVertex(Verts, FVector2f(InParams.Geometry.LocalToAbsolute(QuadPoints.LeftBottom)), QuadPointsUV.LeftBottom);
	AddSlateVertex(Verts, FVector2f(InParams.Geometry.LocalToAbsolute(QuadPoints.RightBottom)), QuadPointsUV.RightBottom);
	AddSlateVertex(Verts, FVector2f(InParams.Geometry.LocalToAbsolute(QuadPoints.RightTop)), QuadPointsUV.RightTop);

	TArray<SlateIndex> Indexes = { 0,1,2, 0,2,3 };

	FSlateDrawElement::MakeCustomVerts
	(
		InParams.OutDrawElements,
		InParams.Layer,
		InHandle,
		Verts,
		Indexes,
		nullptr,
		0,
		0
	);
	return InParams.Layer;
}

void SGraphBase::HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
}

void SGraphBase::PlayAnim(float Duration)
{
	AnimDuration = Duration;
	RequestActiveTimer();
}

void SGraphBase::SetAnimProgress(float InAnimProgress)
{
	AnimProgress = FMath::Clamp(InAnimProgress, 0.f, 1.f);
	Invalidate(EInvalidateWidget::Paint);
}

float SGraphBase::GetAnimProgress() const
{
	return AnimProgress;
}

void SGraphBase::RequestActiveTimer()
{
	CancelActiveTimer();
	LastTime = FSlateApplication::Get().GetCurrentTime();
	IsAnimPlaying = true;
	TimeInterval = 0.f;
	ActiveTimerHandle = RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SGraphBase::ActiveTick));
}

EActiveTimerReturnType SGraphBase::ActiveTick(double InCurrentTime, float InDeltaTime)
{
	TimeInterval = InCurrentTime - LastTime;
	if (TimeInterval <= AnimDuration)
	{
		AnimProgress = TimeInterval / AnimDuration;
		Invalidate(EInvalidateWidget::Paint);
		return EActiveTimerReturnType::Continue;
	}
	IsAnimPlaying = false;
	return EActiveTimerReturnType::Stop;
}

void SGraphBase::CancelActiveTimer()
{
	if (ActiveTimerHandle.IsValid())
	{
		TSharedPtr<FActiveTimerHandle> SharedActiveTimerHandle = ActiveTimerHandle.Pin();
		if (SharedActiveTimerHandle.IsValid())
		{
			UnRegisterActiveTimer(SharedActiveTimerHandle.ToSharedRef());
			IsAnimPlaying = false;
		}
	}
}

#undef LOCTEXT_NAMESPACE