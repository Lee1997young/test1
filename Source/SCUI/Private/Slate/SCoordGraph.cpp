#include "Slate/SCoordGraph.h"
#include "SCUIStyle.h"
#include "Fonts/FontMeasure.h"

DEFINE_LOG_CATEGORY_STATIC(LogCoordGraph, Warning, All);
#define LOCTEXT_NAMESPACE "SCoordGraph"

void SCoordGraph::Construct(const FArguments& InArgs)
{
	SGraphBase::Construct(InArgs);
}

EOrientation SCoordGraph::GetOrientation()
{
	return Orientation;
}

void SCoordGraph::SetOrientation(EOrientation InOrientation)
{
	if (Orientation != InOrientation)
	{
		Orientation = InOrientation;
	}
}

void SCoordGraph::SetLabels(const TArray<FString>& InLabels)
{
	if (Labels != InLabels)
	{
		Labels = InLabels;
	}

	CalcLongestLabelWidth();
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

void SCoordGraph::SetValues(float InMinValue, float InMaxValue, int32 InRowNum)
{
	if (InMinValue >= InMaxValue)
	{
		return;
	}

	MinValue = InMinValue;
	MaxValue = InMaxValue;
	RowNum = InRowNum;

	bFloatValue = FGraphHelper::HasFloatValue(MinValue, MaxValue, RowNum);

	//Values
	Values.Empty();
	for (int32 i = 0; i < RowNum; i++)
	{
		float Alpha = i * (1.0 / (RowNum - 1));
		float Value = FMath::Lerp(MinValue, MaxValue, Alpha);
		Values.Add(Value);
	}

	CalcLongestValueWidth();
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

void SCoordGraph::SetHideLabels(const TArray<FString>& InLabels)
{
	HideLabels = InLabels;
}

void SCoordGraph::SetDrawColorBlock(bool bDraw)
{
	bDrawColorBlock = bDraw;
}

void SCoordGraph::SetDrawGradient(bool bGradient)
{
	bDrawGradient = bGradient;
}

void SCoordGraph::SetLableTextRot(float InRot)
{
	LableTextRot = InRot;
}

int32 SCoordGraph::OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const
{
	FOnPaintHandlerParams Params(InParams);
	Params.Layer++;

	//Draw Axis
	const FVector2D CoordOrigin = GetCoordOrigin();
	const FVector2D CoordSize = GetCoordSize();

	if (Labels.Num() > 0 && Values.Num() >= 2)
	{
		float LabelSpace = 0.f;
		float ValueSpace = 0.f;
		float PosOffset = 0.f;
		FVector2D LabelPos, ValuePos = FVector2D::ZeroVector;
		if (Orientation == EOrientation::Orient_Horizontal)
		{
			LabelSpace = CoordSize.Y / Labels.Num();
			ValueSpace = CoordSize.X / (Values.Num() - 1);
		}
		else if (Orientation == EOrientation::Orient_Vertical)
		{
			LabelSpace = CoordSize.X / Labels.Num();
			ValueSpace = CoordSize.Y / (Values.Num() - 1);
		}
		//Gridline Points
		FVector2D Point1, Point2;

		const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		FVector2D TextSize;

		//Draw Label
		for (int32 Index = 0; Index < Labels.Num(); Index++)
		{
			if ((HideLabels.Num() > 0) && (HideLabels.Find(Labels[Index]) != INDEX_NONE))
			{
				continue;
			}

			TextSize = FontMeasureService->Measure(Labels[Index], LabelFont);

			FSlateRenderTransform TextRenderTransform = TransformCast<FSlateRenderTransform>(FQuat2D(FMath::DegreesToRadians(LableTextRot)));
			FGeometry TextRenderGeometry = Params.Geometry.MakeChild(TextRenderTransform);
			FVector2D PaintPos = FVector2D::ZeroVector;

			//Calc Text Paint Pos
			PosOffset = LabelSpace * Index + LabelSpace * 0.5;
			if (Orientation == EOrientation::Orient_Horizontal)
			{
				LabelPos = FVector2D(-TextSize.X, -TextSize.Y * 0.5) + FVector2D(-LongestValueSize.Y * TextAxisSpace, 0);
				PaintPos = TextRenderGeometry.AbsoluteToLocal(Params.Geometry.LocalToAbsolute(FVector2D(CoordOrigin.X, CoordOrigin.Y - PosOffset))) + LabelPos;
				
			}
			else if (Orientation == EOrientation::Orient_Vertical)
			{
				LabelPos = FVector2D(-TextSize.X * 0.5, 0) + FVector2D(0, LongestValueSize.Y * TextAxisSpace);
				PaintPos = TextRenderGeometry.AbsoluteToLocal(Params.Geometry.LocalToAbsolute(FVector2D(CoordOrigin.X + PosOffset, CoordOrigin.Y))) + LabelPos;
			}

			FSlateDrawElement::MakeText
			(
				Params.OutDrawElements,
				Params.Layer,
				TextRenderGeometry.ToPaintGeometry(PaintPos, TextSize),
				Labels[Index],
				LabelFont,
				ESlateDrawEffect::None,
				LabelColor.GetSpecifiedColor() * GetWidgetOpacity()
			);
		}

		//Draw Value
		for (int32 Index = 0; Index < Values.Num(); Index++)
		{
			FString ValueString = FGraphHelper::ValueToString(Values[Index], bFloatValue);
			TextSize = FontMeasureService->Measure(ValueString, GetValueFont());

			//Calc Text Paint Pos
			PosOffset = ValueSpace * Index;
			if (Orientation == EOrientation::Orient_Horizontal)
			{
				Point1 = FVector2D(CoordOrigin.X + PosOffset, CoordOrigin.Y);
				Point2 = FVector2D(CoordOrigin.X + PosOffset, CoordOrigin.Y - CoordSize.Y);
				ValuePos = FVector2D(CoordOrigin.X + PosOffset, CoordOrigin.Y) + FVector2D(-TextSize.X * 0.5, 0) + FVector2D(0, LongestValueSize.Y * TextAxisSpace);
			}
			else if (Orientation == EOrientation::Orient_Vertical)
			{
				Point1 = FVector2D(CoordOrigin.X, CoordOrigin.Y - PosOffset);
				Point2 = FVector2D(CoordOrigin.X + CoordSize.X, CoordOrigin.Y - PosOffset);
				ValuePos = FVector2D(CoordOrigin.X, CoordOrigin.Y - PosOffset) + FVector2D(-TextSize.X, -TextSize.Y * 0.5) + FVector2D(-LongestValueSize.Y * TextAxisSpace, 0);
			}

			FSlateDrawElement::MakeText
			(
				Params.OutDrawElements,
				Params.Layer,
				Params.Geometry.ToPaintGeometry(ValuePos, TextSize),
				ValueString,
				GetValueFont(),
				ESlateDrawEffect::None,
				ValueColor.GetSpecifiedColor() * GetWidgetOpacity()
			);

			//Draw Gridline
			DrawLine(InParams, Point1, Point2, GetGridColor().GetSpecifiedColor(), 1.f);
		}
	}
	return Params.Layer;
}

void SCoordGraph::CalcLongestLabelWidth()
{
	const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	int32 TextWidth = 0;
	FVector2D TextSize = FVector2D::ZeroVector;

	for (int32 i = 0; i < Labels.Num(); i++)
	{
		TextSize = FontMeasureService->Measure(Labels[i], LabelFont);

		if (TextSize.X > TextWidth)
		{
			TextWidth = TextSize.X;
			LongestLabelSize = TextSize;
		}
	}
}

void SCoordGraph::CalcLongestValueWidth()
{
	const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	int32 TextWidth = 0;
	FVector2D TextSize = FVector2D::ZeroVector;

	for (int32 i = 0; i < Values.Num(); i++)
	{
		FString ValueString = FGraphHelper::ValueToString(Values[i], bFloatValue);
		TextSize = FontMeasureService->Measure(ValueString, GetValueFont());

		if (TextSize.X > TextWidth)
		{
			TextWidth = TextSize.X;
			LongestValueSize = TextSize;
		}
	}
}

FVector2D SCoordGraph::GetCoordOrigin(bool bNeedOffset /*= true*/) const
{
	FVector2D DrawLocalSize = DataBox->GetPaintSpaceGeometry().GetLocalSize();
	FVector2D RetVector = FVector2D::ZeroVector;
	if (!bNeedOffset)
	{
		RetVector = FVector2D(0, DrawLocalSize.Y);
	}
	else
	{
		if (Orientation == EOrientation::Orient_Horizontal)
		{
			RetVector = FVector2D(LongestLabelSize.X, DrawLocalSize.Y - LongestValueSize.Y);
		}
		else if (Orientation == EOrientation::Orient_Vertical)
		{
			RetVector = FVector2D(LongestValueSize.X, DrawLocalSize.Y - LongestLabelSize.Y);
		}
		RetVector += FVector2D(LongestValueSize.Y, -LongestValueSize.Y) * TextAxisSpace;
	}
	return RetVector;
}

FVector2D SCoordGraph::GetCoordSize(bool bNeedOffset/* = true*/) const
{
	FVector2D DrawLocalSize = DataBox->GetPaintSpaceGeometry().GetLocalSize();
	FVector2D CoordOrigin = GetCoordOrigin(bNeedOffset);
	return FVector2D(DrawLocalSize.X - CoordOrigin.X, CoordOrigin.Y);
	
}

bool SCoordGraph::GetPaintPos(const FString& InLabel, float InValue, FVector2D& PaintPos) const
{
	int32 LabelIndex = Labels.Find(InLabel);
	bool bValueValid = (InValue >= MinValue && InValue <= MaxValue);

	if (LabelIndex != INDEX_NONE && bValueValid)
	{
		//Draw Axis
		FVector2D CoordOrigin = GetCoordOrigin();
		FVector2D CoordSize = GetCoordSize();

		float LabelSpace = 0.f;
		float PosOffset = 0.f;
		FVector2D LabelPos, ValuePos = FVector2D::ZeroVector;

		float ValueAlpha = (InValue - MinValue) / (MaxValue - MinValue);

		if (Orientation == EOrientation::Orient_Horizontal)
		{
			LabelSpace = CoordSize.Y / Labels.Num();
			PosOffset = LabelSpace * LabelIndex + LabelSpace * 0.5;
			//Get Label Draw Pos
			LabelPos = FVector2D(CoordOrigin.X, CoordOrigin.Y - PosOffset);
			PaintPos.Y = LabelPos.Y;
			//Get Value Draw Pos
			PaintPos.X = FMath::Lerp(CoordOrigin.X, CoordOrigin.X + CoordSize.X, ValueAlpha);
		}
		else if (Orientation == EOrientation::Orient_Vertical)
		{
			LabelSpace = CoordSize.X / Labels.Num();
			PosOffset = LabelSpace * LabelIndex + LabelSpace * 0.5;
			//Get Label Draw Pos
			LabelPos = FVector2D(CoordOrigin.X + PosOffset, CoordOrigin.Y);
			PaintPos.X = LabelPos.X;
			//Get Value Draw Pos
			PaintPos.Y = FMath::Lerp(CoordOrigin.Y, CoordOrigin.Y - CoordSize.Y, ValueAlpha);
		}
		
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE