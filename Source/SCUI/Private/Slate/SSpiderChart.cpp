#include "Slate/SSpiderChart.h"
#include "SCUIStyle.h"
#include "SCUIMacro.h"
#include "Fonts/FontMeasure.h"

#define LOCTEXT_NAMESPACE "SSpiderChart"

void SSpiderChart::Construct(const FArguments& InArgs)
{
	SCoordGraph::Construct(InArgs);
}

int32 SSpiderChart::OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const
{
	//Draw Gridlines
	FOnPaintHandlerParams Params = InParams;
	Params.Layer++;
	DrawGridlines(Params);

	//Draw LabelText
	Params.Layer++;
	DrawLabelText(Params);

	//Draw Data
	Params.Layer++;
	DrawDatas(Params, IsAnimPlaying, InAnimAlpha);

	return InParams.Layer;
}

void SSpiderChart::HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	InitHoverData();

	const FVector2D CoordSize = GetCoordSize(false);
	float Radius = FMath::Min(CoordSize.X, CoordSize.Y) * 0.5f;
	FVector2D Center = CoordSize * 0.5f;

	if (DataPtrs.Num() > 0)
	{
		float PointDistance = 20.0f;
		for (int32 i = 0; i < DataPtrs.Num(); i++)
		{
			TSharedPtr<FGraphDataLine> DataLine = StaticCastSharedPtr<FGraphDataLine>(DataPtrs[i]);
			if (DataLine.IsValid() && DataLine->DataArray.Num() > 0)
			{
				//DataLine
				for (int32 j = 0; j < DataLine->DataArray.Num(); j++)
				{
					const float DataRadius = Radius * ((DataLine->DataArray[j].Value - MinValue) / (MaxValue - MinValue));
					FVector2D ValuePaintPos = GetPaintLoc(j, DataRadius, Center);

					if ((MousePosition - ValuePaintPos).Size() < PointDistance)
					{
						DataLineHoverIndex = i;
						DataHoverIndex = j;
						PointDistance = (MousePosition - ValuePaintPos).Size();
					}
				}
			}
		}
	}

	if (DataLineHoverIndex != INDEX_NONE && DataHoverIndex != INDEX_NONE)
	{
		bDataHovering = true;
		TSharedPtr<FGraphDataLine> DataLine = StaticCastSharedPtr<FGraphDataLine>(DataPtrs[DataLineHoverIndex]);
		HoverDataPtr = MakeShareable(new FGraphData(DataLine->DataArray[DataHoverIndex].Label, DataLine->DataArray[DataHoverIndex].Value));
	}
	OnDataHover.ExecuteIfBound(bDataHovering, HoverDataPtr, DataLineHoverIndex, MyGeometry, MouseEvent);
}

FVector2D SSpiderChart::GetPaintLoc(int32 Index, float InRadius, const FVector2D& InCenter) const
{
	const float Angle = (2 * PI * Index) / Labels.Num();
	FVector2D EdgeDirection(FMath::Cos(Angle), FMath::Sin(Angle));
	//The default direction is 3 o'clock, rotate 90 counterclockwise,
	//and adjust the initial direction to start at 12 o'clock
	EdgeDirection = EdgeDirection.GetRotated(-90);
	const FVector2D Edge(InRadius * EdgeDirection);

	return InCenter + Edge;
}

int32 SSpiderChart::DrawGridlines(const FOnPaintHandlerParams& InParams) const
{
	const FVector2D CoordSize = GetCoordSize(false);
	float Radius = FMath::Min(CoordSize.X, CoordSize.Y) * 0.5f;
	FVector2D Center = CoordSize * 0.5f;
	
	TArray<FVector2D> LabelPoints;
	LabelPoints.Init(FVector2D(0, 0), Labels.Num());

	for (int32 i = 0; i < Labels.Num(); i++)
	{
		LabelPoints[i] = GetPaintLoc(i, Radius, Center);
	}

	//Draw Central radiation
	TArray<FVector2D> LinePoints;
	LinePoints.Init(FVector2D(0, 0), 2);
	LinePoints[0] = Center;
	for (int32 i = 0; i < LabelPoints.Num(); i++)
	{
		LinePoints[1] = LabelPoints[i];
		FSlateDrawElement::MakeLines
		(
			InParams.OutDrawElements,
			InParams.Layer,
			InParams.Geometry.ToPaintGeometry(),
			LinePoints,
			ESlateDrawEffect::None,
			GetGridColor().GetSpecifiedColor() * GetWidgetOpacity(),
			true,
			1.0f
		);
	}

	for (int32 Row = 0; Row < RowNum; Row++)
	{
		const float RowRadius = Radius * Row / (RowNum - 1);
		//Draw Circular line
		for (int32 i = 0; i < LabelPoints.Num(); i++)
		{
			LinePoints.Init(FVector2D(0, 0), 2);
			LinePoints[0] = GetPaintLoc(i, RowRadius, Center);
			LinePoints[1] = GetPaintLoc((i + 1) == LabelPoints.Num() ? 0 : (i + 1) , RowRadius, Center);
			FSlateDrawElement::MakeLines
			(
				InParams.OutDrawElements,
				InParams.Layer,
				InParams.Geometry.ToPaintGeometry(),
				LinePoints,
				ESlateDrawEffect::None,
				GetGridColor().GetSpecifiedColor() * GetWidgetOpacity(),
				true,
				1.0f
			);
		}

		FString ValueString = FGraphHelper::ValueToString(FMath::Lerp(MinValue, MaxValue, (float)Row / (RowNum - 1)));
		const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		FVector2D TextSize = FontMeasureService->Measure(ValueString, ValueFont);

		FVector2D TextOffset = GetPaintLoc(0, RowRadius, Center) - FVector2D(TextSize.X, 0);

		FSlateDrawElement::MakeText
		(
			InParams.OutDrawElements,
			InParams.Layer + 10,
			InParams.Geometry.ToPaintGeometry(TextOffset, TextSize),
			ValueString,
			ValueFont,
			ESlateDrawEffect::None,
			ValueColor.GetSpecifiedColor() * GetWidgetOpacity()
		);
	}

	return InParams.Layer;
}

int32 SSpiderChart::DrawLabelText(const FOnPaintHandlerParams& InParams) const
{
	const FVector2D CoordSize = GetCoordSize(false);
	float Radius = FMath::Min(CoordSize.X, CoordSize.Y) * 0.5f;
	FVector2D Center = CoordSize * 0.5f;

	if (Labels.Num() > 0)
	{
		for (int32 i = 0; i < Labels.Num(); i++)
		{
			const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
			FVector2D TextSize = FontMeasureService->Measure(Labels[i], LabelFont);

			const float Angle = (2 * PI * i) / Labels.Num();
			FVector2D EdgeDirection(FMath::Cos(Angle), FMath::Sin(Angle));
			EdgeDirection = EdgeDirection.GetRotated(270);

			const FVector2D Edge(1.1 * Radius * EdgeDirection);
			FVector2D TextOffset = Center + Edge;

			float FinalAngle = FMath::RadiansToDegrees(Angle) + 270;
			FinalAngle = FMath::Fmod(FinalAngle, 360);
			if (FinalAngle == 90 || FinalAngle == 270)
			{
				TextOffset += FVector2D(-TextSize.X * 0.5, -TextSize.Y * 0.5);
			}
			else if ((FinalAngle >= 270 && FinalAngle <= 360) || (FinalAngle <= 0 && FinalAngle >= -90)
				|| (FinalAngle >= 0 && FinalAngle <= 90) || (FinalAngle <= -270 && FinalAngle >= -360))
			{
				//In Quadrant 1 or 4
				TextOffset += FVector2D(2, -TextSize.Y * 0.5);
			}
			else if ((FinalAngle >= 180 && FinalAngle < 270) || (FinalAngle >= -180 && FinalAngle < -90)
				|| (FinalAngle > 90 && FinalAngle <= 180) || (FinalAngle > -270 && FinalAngle <= -180))
			{
				//In Quadrant 2 or 3
				TextOffset += FVector2D(-TextSize.X, -TextSize.Y * 0.5);
			}

			FSlateDrawElement::MakeText
			(
				InParams.OutDrawElements,
				InParams.Layer,
				InParams.Geometry.ToPaintGeometry(TextOffset, TextSize),
				Labels[i],
				LabelFont,
				ESlateDrawEffect::None,
				LabelColor.GetSpecifiedColor() * GetWidgetOpacity()
			);
		}
	}

	return InParams.Layer;
}

int32 SSpiderChart::DrawDatas(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const
{
	int32 DrawLayer = InParams.Layer;

	const FSlateBrush* DataPointBrush = FSCUIStyle::Get().GetBrush("DataPoint");
	const FSlateBrush* GradientBrush = FSCUIStyle::Get().GetBrush("WhiteBrush");
	FSlateResourceHandle Handle = GradientBrush->GetRenderingResource();
	const FSlateShaderResourceProxy* ResourceProxy = Handle.GetResourceProxy();
	FVector2f StartUV = FVector2f(0, 0);
	FVector2f SizeUV = FVector2f(0, 0);
	if (ResourceProxy != nullptr)
	{
		StartUV = ResourceProxy->StartUV;
		SizeUV = ResourceProxy->SizeUV;
	}

	const FVector2D CoordSize = GetCoordSize(false);
	float Radius = FMath::Min(CoordSize.X, CoordSize.Y) * 0.5f;
	FVector2D Center = CoordSize * 0.5f;

	//Processing each data line
	for (int32 DataLineIndex = 0; DataLineIndex < DataPtrs.Num(); DataLineIndex++)
	{
		DrawLayer++;
		TSharedPtr<FGraphDataLine> DataLine = StaticCastSharedPtr<FGraphDataLine>(DataPtrs[DataLineIndex]);
		if (DataLine.IsValid() && DataLine->DataArray.Num() > 0)
		{
			TArray<FVector2D> DataPoints;
			DataPoints.Init(FVector2D(0, 0), DataLine->DataArray.Num());

			//Get Data Paint Position of each Data line
			for (int32 DataPointIndex = 0; DataPointIndex < DataLine->DataArray.Num(); DataPointIndex++)
			{
				bool bValueValid = (DataLine->DataArray[DataPointIndex].Value >= MinValue && DataLine->DataArray[DataPointIndex].Value <= MaxValue);
				if (bValueValid)
				{
					const float DataRadius = Radius * ((DataLine->DataArray[DataPointIndex].Value - MinValue) / (MaxValue - MinValue));
					float AnimDataRadius = DataRadius;
					if (bAnimPlaying)
					{
						AnimDataRadius = FMath::Lerp(0.f, DataRadius, InAnimAlpha);
					}
					DataPoints[DataPointIndex] = GetPaintLoc(DataPointIndex, AnimDataRadius, Center);
				}
			}

			for (int32 i = 0; i < DataPoints.Num(); i++)
			{
				FLinearColor DrawDataColor = DataLine->LineColor;
				if (DataLineHoverIndex == DataLineIndex && DataHoverIndex == i)
				{
					DrawDataColor = DataLine->LineColor + FLinearColor(0.25f, 0.25f, 0.25f);
				}

				//Draw Data Point
				FSlateDrawElement::MakeBox
				(
					InParams.OutDrawElements,
					DrawLayer + 2,
					InParams.Geometry.ToPaintGeometry(
						DataPoints[i] + FVector2D(-4, -4) * SizeScale,
						FVector2D(8.f, 8.f) * SizeScale),
					DataPointBrush,
					ESlateDrawEffect::None,
					DrawDataColor * GetWidgetOpacity()
				);

				//Draw Data line
				TArray<FVector2D> LinePoints;
				LinePoints.Init(FVector2D(0, 0), 2);
				LinePoints[0] = DataPoints[i];
				LinePoints[1] = (i + 1) == DataPoints.Num() ? DataPoints[0] : DataPoints[i + 1];
				FSlateDrawElement::MakeLines
				(
					InParams.OutDrawElements,
					DrawLayer + 1,
					InParams.Geometry.ToPaintGeometry(),
					LinePoints,
					ESlateDrawEffect::None,
					DataLine->LineColor * GetWidgetOpacity(),
					true,
					2.0 * SizeScale
				);

				//Draw Color Block
				if (bDrawColorBlock)
				{
					TArray<FSlateVertex> Verts;
					Verts.Reserve(3);

					FColor VertColor = DataLine->LineColor.ToFColor(true);
					VertColor.A *= 0.6 * GetWidgetOpacity();

					AddSlateVertex(Verts, FVector2f(InParams.Geometry.LocalToAbsolute(Center)), StartUV);
					AddSlateVertex(Verts, FVector2f(InParams.Geometry.LocalToAbsolute(DataPoints[i])), StartUV);
					AddSlateVertex(Verts, FVector2f(InParams.Geometry.LocalToAbsolute((i + 1) == DataPoints.Num() ? DataPoints[0] : DataPoints[i + 1])), StartUV);

					TArray<SlateIndex> Indexes = { 0,1,2 };

					FSlateDrawElement::MakeCustomVerts
					(
						InParams.OutDrawElements,
						DrawLayer,
						Handle,
						Verts,
						Indexes,
						nullptr,
						0,
						0
					);
				}
			}
		}
	}

	return DrawLayer + 2;
}

#undef LOCTEXT_NAMESPACE