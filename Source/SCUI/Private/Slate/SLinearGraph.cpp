#include "Slate/SLinearGraph.h"
#include "SCUIStyle.h"
#include "Fonts/FontMeasure.h"

DEFINE_LOG_CATEGORY_STATIC(LogLinearGraph, Warning, All);
#define LOCTEXT_NAMESPACE "SLinearGraph"

#pragma optimize("", off)

void SLinearGraph::Construct(const FArguments& InArgs)
{
	SCoordGraph::Construct(InArgs);
}

void SLinearGraph::SetSplineType(bool bSpline)
{
	bSplineType = bSpline;
}

void SLinearGraph::SetSplineTangentScale(float InTangentScale)
{
	TangentScale = InTangentScale;
}

void SLinearGraph::SetSplinePointSizeScale(float InPointSizeScale)
{
	PointSizeScale = InPointSizeScale;
}

void SLinearGraph::SetShowLabelsData(bool bShow)
{
	bShowLabelsData = bShow;
}

void SLinearGraph::SetLabelsDataFont(const FSlateFontInfo& InFontInfo)
{
	LabelsDataFont = InFontInfo;
}

void SLinearGraph::SetLabelsDataTextColorAndOpacity(const FSlateColor& InColorAndOpacity)
{
	LabelsDataColor = InColorAndOpacity;
}

int32 SLinearGraph::OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const
{
	SCoordGraph::OnPaintAnim(InParams, bAnimPlaying, InAnimAlpha);

	FVector2D CoordOrigin = GetCoordOrigin();
	FVector2D CoordSize = GetCoordSize();

	//Draw Data
	const FSlateBrush* DataPointBrush = FSCUIStyle::Get().GetBrush("DataPoint");
	const FSlateBrush* GradientBrush = bDrawGradient ? FSCUIStyle::Get().GetBrush("GradientV") : FSCUIStyle::Get().GetBrush("WhiteBrush");
	FSlateResourceHandle Handle = GradientBrush->GetRenderingResource();
	const FSlateShaderResourceProxy* ResourceProxy = Handle.GetResourceProxy();
	FVector2f StartUV = FVector2f(0, 0);
	FVector2f SizeUV = FVector2f(0, 0);
	if (ResourceProxy != nullptr)
	{
		StartUV = ResourceProxy->StartUV;
		SizeUV = ResourceProxy->SizeUV;
	}

	const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D TextSize;
	float PosOffset = 0.f;

	if (DataPtrs.Num() > 0)
	{
		for (int32 i = 0; i < DataPtrs.Num(); i++)
		{
			TSharedPtr<FGraphDataLine> DataLine = StaticCastSharedPtr<FGraphDataLine>(DataPtrs[i]);
			if (DataLine.IsValid() && DataLine->DataArray.Num() > 0)
			{
				float MaxUV_Y = 0.f;
				TArray<FVector2D> PointPosArray;
				//Draw DataLine
				for (int32 j = 0; j < DataLine->DataArray.Num(); j++)
				{
					//Get Data Paint Position
					FVector2D ZeroPaintPos, ValuePaintPos;
					if (GetPaintPos(DataLine->DataArray[j].Label, MinValue, ZeroPaintPos) && GetPaintPos(DataLine->DataArray[j].Label, DataLine->DataArray[j].Value, ValuePaintPos))
					{
						PointPosArray.Add(ValuePaintPos);

						//Calc MaxUV_Y
						if ((CoordOrigin.Y - ValuePaintPos.Y) > MaxUV_Y)
						{
							MaxUV_Y = (CoordOrigin.Y - ValuePaintPos.Y);
						}
					}
				}
				
				if (PointPosArray.Num() < 2)
				{
					continue;
				}
				FVector2D FirstPaintPos = PointPosArray[0];
				FVector2D LastPaintPos = PointPosArray[PointPosArray.Num() - 1];
				FVector2D RefPaintPos = FMath::Lerp(FirstPaintPos, LastPaintPos, InAnimAlpha);
				//Get All Segments
				TArray<FSegment2D> Segments;

				//Calc Spline Segment
				if (PointPosArray.Num() >= 2)
				{
					FLinearColor DrawDataColor = DataLine->LineColor;
					for (int32 PosIndex = 0; PosIndex < PointPosArray.Num(); PosIndex++)
					{
						if (DataLineHoverIndex == i && DataHoverIndex == PosIndex)
						{
							DrawDataColor = DataLine->LineColor + FLinearColor(0.25f, 0.25f, 0.25f);
						}

						if (PointPosArray[PosIndex].X < RefPaintPos.X)
						{
							//Paint Data Point
							FSlateDrawElement::MakeBox
							(
								InParams.OutDrawElements,
								InParams.Layer + 1,
								InParams.Geometry.ToPaintGeometry(
									PointPosArray[PosIndex] + FVector2D(-4, -4) * SizeScale * PointSizeScale,
									FVector2D(8.f, 8.f) * SizeScale * PointSizeScale),
								DataPointBrush,
								ESlateDrawEffect::None,
								DrawDataColor * GetWidgetOpacity()
							);
						}

						//Draw Labels Data
						if (bShowLabelsData)
						{
							const FString LabelDataText = FGraphHelper::FloatToString(DataLine->DataArray[PosIndex].Value, 1);
							TextSize = FontMeasureService->Measure(LabelDataText, LabelsDataFont);
							FSlateDrawElement::MakeText
							(
								InParams.OutDrawElements,
								InParams.Layer,
								InParams.Geometry.ToPaintGeometry(PointPosArray[PosIndex] - FVector2D(0.5, 1) * TextSize, TextSize),
								LabelDataText,
								LabelsDataFont,
								ESlateDrawEffect::None,
								LabelsDataColor.GetSpecifiedColor() * GetWidgetOpacity()
							);
						}
					}

					for (int32 PosIndex = 0; PosIndex < PointPosArray.Num() - 1; PosIndex++)
					{
						if (bSplineType == false)
						{
							FSegment2D Segment(PointPosArray[PosIndex], PointPosArray[PosIndex + 1]);
							Segments.Add(Segment);
						}
						else
						{
							FVector2D TangentDir1, TangentDir2;
							float TangentLength = (PointPosArray[PosIndex + 1] - PointPosArray[PosIndex]).Size();

							TArray<FVector2D> BezierPoints;
							FVector2D ControlPos1, ControlPos2;
							
							int32 PreIndex, StartIndex, EndIndex, PostIndex;
							//Start Segment							
							StartIndex = PosIndex;
							EndIndex = PosIndex + 1;

							PreIndex = StartIndex - 1;
							PreIndex = (PreIndex < 0 ? StartIndex : PreIndex);

							PostIndex = EndIndex + 1;
							PostIndex = (PostIndex >= PointPosArray.Num() ? EndIndex : PostIndex);

							TangentDir2 = TangentDir1 = (PointPosArray[PostIndex] - PointPosArray[PreIndex]).GetSafeNormal();
							TangentDir1 = (PreIndex < 0 ? FVector2D(1,0) : TangentDir1);
							TangentDir2 = (PostIndex >= PointPosArray.Num() ? FVector2D(1, 0) : TangentDir2);

							ControlPos1 = PointPosArray[StartIndex] + TangentDir1 * TangentLength * TangentScale;
							ControlPos2 = PointPosArray[EndIndex] + TangentDir2 * TangentLength * TangentScale * (-1.f);
							BezierPoints.Add(PointPosArray[StartIndex]);
							BezierPoints.Add(ControlPos1);
							BezierPoints.Add(ControlPos2);
							BezierPoints.Add(PointPosArray[EndIndex]);

							TArray<FVector2D> OutPoints;
							FGraphHelper::EvaluateBezier(BezierPoints.GetData(), 20, OutPoints);
							for (int32 BezierPosIndex = 0; BezierPosIndex < OutPoints.Num() - 1; BezierPosIndex++)
							{
								FSegment2D Segment(OutPoints[BezierPosIndex], OutPoints[BezierPosIndex + 1]);
								Segments.Add(Segment);
							}
						}
					}
				}

				//Draw Segment
				TArray<FVector2D> LinePoints;
				for (int32 SegIndex = 0; SegIndex < Segments.Num(); SegIndex++)
				{
					bool bNeedDraw = false;
					LinePoints.Empty();
					int32 State = Segments[SegIndex].CheckPointOnSegmentX(RefPaintPos.X);

					if (State == 0)
					{
						//The Segment is Drawing
						bNeedDraw = true;
						float TempAlpha = (RefPaintPos.X - Segments[SegIndex].PointStart.X) / (Segments[SegIndex].PointEnd.X - Segments[SegIndex].PointStart.X);
						FVector2D PointEnd = FMath::Lerp(Segments[SegIndex].PointStart, Segments[SegIndex].PointEnd, TempAlpha);
						LinePoints.Add(Segments[SegIndex].PointStart);
						LinePoints.Add(PointEnd);
					}
					else if (State == 1)
					{
						//The Segment has been Drawn
						bNeedDraw = true;
						LinePoints.Add(Segments[SegIndex].PointStart);
						LinePoints.Add(Segments[SegIndex].PointEnd);
					}

					if (bNeedDraw)
					{
						//Need Draw
						FSlateDrawElement::MakeLines
						(
							InParams.OutDrawElements,
							InParams.Layer,
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
							if (LinePoints.Num() >= 2)
							{
								for (int32 j = 0; j < LinePoints.Num() - 1; j++)
								{
									FQuadPoints QuadPoints = FQuadPoints(LinePoints[j], FVector2D(LinePoints[j].X, CoordOrigin.Y), FVector2D(LinePoints[j + 1].X, CoordOrigin.Y), LinePoints[j + 1]);
									FVector2D UV1 = FVector2D(StartUV.X, StartUV.Y + SizeUV.Y * ((CoordOrigin.Y - LinePoints[j].Y) / MaxUV_Y));
									FVector2D UV2 = FVector2D(StartUV.X, StartUV.Y + SizeUV.Y * ((CoordOrigin.Y - LinePoints[j+1].Y) / MaxUV_Y));
									FQuadPoints QuadPointsUV = FQuadPoints(UV1, FVector2D(StartUV), FVector2D(StartUV), UV2);
									DrawGradient(InParams, QuadPoints, QuadPointsUV, Handle, DataLine->LineColor);
								}
							}
						}
					}
				}
			}
		}
	}
	return InParams.Layer + 1;
}


void SLinearGraph::HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	InitHoverData();

	if (DataPtrs.Num() > 0)
	{
		float PointDistance = 20.0f;
		for (int32 i = 0; i < DataPtrs.Num(); i++)
		{
			TSharedPtr<FGraphDataLine> DataLine = StaticCastSharedPtr<FGraphDataLine>(DataPtrs[i]);
			if (DataLine.IsValid() && DataLine->DataArray.Num() > 0)
			{
				TArray<FVector2D> PointPosArray;
				//DataLine
				for (int32 j = 0; j < DataLine->DataArray.Num(); j++)
				{
					//Get Data Paint Position
					FVector2D ValuePaintPos;
					if (GetPaintPos(DataLine->DataArray[j].Label, DataLine->DataArray[j].Value, ValuePaintPos))
					{
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
	}

	if (DataLineHoverIndex != INDEX_NONE && DataHoverIndex != INDEX_NONE)
	{
		bDataHovering = true;
		TSharedPtr<FGraphDataLine> DataLine = StaticCastSharedPtr<FGraphDataLine>(DataPtrs[DataLineHoverIndex]);
		HoverDataPtr = MakeShareable(new FGraphData(DataLine->DataArray[DataHoverIndex].Label, DataLine->DataArray[DataHoverIndex].Value));
	}
	OnDataHover.ExecuteIfBound(bDataHovering, HoverDataPtr, DataLineHoverIndex, MyGeometry, MouseEvent);
}

FSlateFontInfo SLinearGraph::GetLabelsDataFont() const
{
	return FSlateFontInfo();
}

FSlateColor SLinearGraph::GetLabelsDataColor() const
{
	return FSlateColor();
}

#pragma optimize("", on)

#undef LOCTEXT_NAMESPACE