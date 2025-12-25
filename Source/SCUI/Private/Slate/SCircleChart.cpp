#include "Slate/SCircleChart.h"
#include "SCUIStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"

#define LOCTEXT_NAMESPACE "SCircleChart"

void SCircleChart::Construct(const FArguments& InArgs)
{
	DataNum = 0;
	DataTotal = 0.f;

	SGraphBase::Construct(InArgs);
}

int32 SCircleChart::OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const
{
	const FSlateRenderTransform& Transform = InParams.Geometry.GetAccumulatedRenderTransform();
	const float Scale = InParams.Geometry.Scale;
	const float Radius = FMath::Min(InParams.Geometry.GetLocalSize().X, InParams.Geometry.GetLocalSize().Y) * Scale * 0.5f;
	const FVector2D Center = Transform.GetTranslation() + FVector2D(InParams.Geometry.GetLocalSize() * Scale * 0.5f);

	const FSlateBrush* MyPieBrush = FSCUIStyle::Get().GetBrush(TEXT("WhiteCircle"));
	const FSlateBrush* MyRingBrush = FSCUIStyle::Get().GetBrush(TEXT("WhiteRing"));

	FSlateResourceHandle Handle;
	switch (ChartType)
	{
	case Pie:
		Handle = MyPieBrush->GetRenderingResource();
		break;
	case Ring:
		Handle = MyRingBrush->GetRenderingResource();
		break;
	}
	const FSlateShaderResourceProxy* ResourceProxy = Handle.GetResourceProxy();

	FVector2f UVCenter = FVector2f::ZeroVector;
	FVector2f UVRadius = FVector2f(1, 1);
	if (ResourceProxy != nullptr)
	{
		UVRadius = 0.5f * ResourceProxy->SizeUV;
		UVCenter = ResourceProxy->StartUV + UVRadius;
	}

	if (DataTotal <= 0)
	{
		return InParams.Layer;
	}

	int32 TrisTotal = TrisTotalNum; //1000

	//Calc and Get data of each Dataline
	if (DataPtrs.Num() > 0)
	{
		int32 NumTrisTotal = 0;
		int32 StartTrisIndex = 0;
		for (int32 index = 0; index < DataPtrs.Num(); index++)
		{
			TSharedPtr<FGraphDataCircle> Data = StaticCastSharedPtr<FGraphDataCircle>(DataPtrs[index]);

			//Calc Percent of Data
			float Percent = Data->Value / DataTotal;

			FString DataTextString;
			if (DataTextType == DataPercent)
			{
				DataTextString = FString::Printf(TEXT("%.01f"), Percent * 100) + TEXT(" %");
			}
			else if (DataTextType == DataValue)
			{
				FString TempValueString = FGraphHelper::IsNumeric(Data->Value) ? FGraphHelper::ValueToString(Data->Value) : FString::Printf(TEXT("%.01f"), Data->Value);
				DataTextString = (Data->Label.IsEmpty() ? TEXT("") : FString::Printf(TEXT("%s:"), *Data->Label)) + TempValueString + DataTextSuffix;
			}
			
			//The number of triangles required to calculate the percentage(decimal points are rounded up)
			int32 NumTris = FMath::CeilToInt(Percent * TrisTotal);
			if (index == DataPtrs.Num() - 1)
			{
				NumTris = TrisTotal - NumTrisTotal;
			}
			else
			{
				NumTrisTotal += NumTris;
			}

			FLinearColor DrawDataColor = Data->Color;
			if (DataHoverIndex == index)
			{
				DrawDataColor = Data->Color + FLinearColor(0.25f, 0.25f, 0.25f);
				DrawDataColor *= GetWidgetOpacity();
			}

			NumTris = NumTris > 0 ? NumTris : 1;
			// Make a triangle fan in the area allotted
			TArray<FSlateVertex> Verts;
			Verts.Reserve(NumTris * 3);

			// Center Vertex
			Verts.AddZeroed();
			{
				FSlateVertex& NewVert = Verts.Last();
				NewVert.Position[0] = Center.X;
				NewVert.Position[1] = Center.Y;
				NewVert.TexCoords[0] = UVCenter.X;
				NewVert.TexCoords[1] = UVCenter.Y;
				NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
				NewVert.Color = DrawDataColor.ToFColor(false);
				NewVert.Color.A *= GetWidgetOpacity();
			}

			for (int i = 0; i < NumTris + 1; ++i)
			{
				Verts.AddZeroed();
				{
					int32 CurTriIndex = StartTrisIndex + i;
					float AnimRate = 1.f;
					if (IsAnimPlaying)
					{
						//Draw Animation
						AnimRate = FMath::Lerp(0.1f, 1.0f, InAnimAlpha);
					}
					const float Angle = (2 * PI*CurTriIndex) * AnimRate / TrisTotal;
					FVector2D EdgeDirection(FMath::Cos(Angle), FMath::Sin(Angle));
					//The default direction is 3 o'clock, rotate 90 counterclockwise,
					//and adjust the initial direction to start at 12 o'clock
					EdgeDirection = EdgeDirection.GetRotated(StartAngle - 90);
					const FVector2D Edge(Radius*EdgeDirection);
					FSlateVertex& NewVert = Verts.Last();
					NewVert.Position[0] = Center.X + Edge.X * Data->Thickness;
					NewVert.Position[1] = Center.Y + Edge.Y * Data->Thickness;

					switch (ChartType)
					{
					case Pie:
					{
						NewVert.TexCoords[0] = UVCenter.X + UVRadius.X*EdgeDirection.X * Data->Thickness;
						NewVert.TexCoords[1] = UVCenter.Y + UVRadius.Y*EdgeDirection.Y * Data->Thickness;
						break;
					}
					case Ring:
					{
						NewVert.TexCoords[0] = UVCenter.X + UVRadius.X*EdgeDirection.X * FMath::Lerp(0.5f, 1.0f, Thickness) * Data->Thickness;
						NewVert.TexCoords[1] = UVCenter.Y + UVRadius.Y*EdgeDirection.Y * FMath::Lerp(0.5f, 1.0f, Thickness) * Data->Thickness;
						break;
					}
					}
					NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
					NewVert.Color = DrawDataColor.ToFColor(false);
					NewVert.Color.A *= GetWidgetOpacity();
				}
			}

			TArray<SlateIndex> Indexes;
			for (int i = 0; i <= NumTris; ++i)
			{
				Indexes.Add(0);
				Indexes.Add(i);
				Indexes.Add(i + 1);
			}

			FSlateDrawElement::MakeCustomVerts(InParams.OutDrawElements, InParams.Layer, Handle, Verts, Indexes, nullptr, 0, 0);

			if (bShowDataText)
			{
				//Draw Data Text
				if (IsAnimPlaying == false)
				{
					const FString ValueString = DataTextString;
					const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
					FVector2D TextSize = FontMeasureService->Measure(ValueString, ValueFont);

					//Get Center of sector
					int32 CenterIndex = StartTrisIndex + NumTris * 0.5;
					const float CenterAngle = (2 * PI * CenterIndex) / TrisTotal;
					FVector2D CenterEdgeDirection(FMath::Cos(CenterAngle), FMath::Sin(CenterAngle));
					//The default direction is 3 o'clock, rotate 90 counterclockwise,
					//and adjust the initial direction to start at 12 o'clock
					CenterEdgeDirection = CenterEdgeDirection.GetRotated(StartAngle + 270);
					const FVector2D CenterEdge(Radius * CenterEdgeDirection * Data->Thickness);
					FVector2D RoutePointStart = Center + CenterEdge;
					FVector2D RoutePointEnd = RoutePointStart + FVector2D((Radius * LineLength) * CenterEdgeDirection);

					FVector2D TextOffset = InParams.Geometry.AbsoluteToLocal(RoutePointEnd);
					float FinalAngle = FMath::RadiansToDegrees(CenterAngle) + (StartAngle + 270);
					FinalAngle = FMath::Fmod(FinalAngle, 360);
					if ((FinalAngle >= 270 && FinalAngle <= 360) || (FinalAngle <= 0 && FinalAngle >= -90)
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
						ValueString,
						ValueFont,
						ESlateDrawEffect::None,
						LabelColor.GetSpecifiedColor() * GetWidgetOpacity()
					);

					//Draw Route line
					TArray<FVector2D> LinePoints;
					LinePoints.Init(FVector2D(0, 0), 2);
					LinePoints[0] = InParams.Geometry.AbsoluteToLocal(RoutePointStart);
					LinePoints[1] = InParams.Geometry.AbsoluteToLocal(RoutePointEnd);

					FSlateDrawElement::MakeLines
					(
						InParams.OutDrawElements,
						InParams.Layer,
						InParams.Geometry.ToPaintGeometry(),
						LinePoints,
						ESlateDrawEffect::None,
						Data->Color * GetWidgetOpacity(),
						true,
						1.0f
					);
				}
			}

			//Triangle index of the beginning of the current block
			StartTrisIndex += NumTris;
		}
	}

	return InParams.Layer;
}

ECircleChartType SCircleChart::GetChartType()
{
	return ChartType;
}

void SCircleChart::SetChartType(ECircleChartType InChartType)
{
	if (ChartType != InChartType)
	{
		ChartType = InChartType;
	}
}


void SCircleChart::SetDatas(const TArray<TSharedPtr<FGraphDataBase>>& InDatas)
{
	DataTotal = 0.f;
	DataPtrs = InDatas;
	DataNum = DataPtrs.Num();
	for (int32 i = 0; i < DataPtrs.Num(); i++)
	{
		TSharedPtr<FGraphDataCircle> Data = StaticCastSharedPtr<FGraphDataCircle>(DataPtrs[i]);
		DataTotal = DataTotal + Data->Value;
	}
}

void SCircleChart::SetThickness(float InThickness)
{
	Thickness = InThickness;
}

void SCircleChart::SetShowDataText(bool bShow)
{
	bShowDataText = bShow;
}

void SCircleChart::SetDataTextType(ECircleChartDataTextType InDataTextType)
{
	if (DataTextType != InDataTextType)
	{
		DataTextType = InDataTextType;
	}
}

void SCircleChart::SetDataTextSuffix(const FString& InDataTextSuffix)
{
	if (DataTextSuffix != InDataTextSuffix)
	{
		DataTextSuffix = InDataTextSuffix;
	}
}

void SCircleChart::SetLineLength(float Length)
{
	LineLength = Length;
}

void SCircleChart::SetStartAngle(float Angle)
{
	StartAngle = Angle;
}

void SCircleChart::HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	InitHoverData();

	const FGeometry DataGeometry = DataBox->GetTickSpaceGeometry();
	const float Radius = FMath::Min(DataGeometry.GetLocalSize().X, DataGeometry.GetLocalSize().Y) * 0.5f;
	const FVector2D Center = DataGeometry.GetLocalSize() * 0.5f;
	const FVector2D MouseDir = (MousePosition - Center).GetSafeNormal();
	const float MouseVectorSize = (MousePosition - Center).Size();

	if (DataPtrs.Num() > 0)
	{
		float DataAngle = 0.f;
		TArray<float> DataSectorAngles;
		DataSectorAngles.Add(DataAngle);

		for (int32 Index = 0; Index < DataPtrs.Num(); Index++)
		{
			TSharedPtr<FGraphDataCircle> Data = StaticCastSharedPtr<FGraphDataCircle>(DataPtrs[Index]);
			//Calc Percent of Data
			float Percent = Data->Value / DataTotal;

			DataAngle = DataAngle + 360.f * Percent;
			DataSectorAngles.Add(DataAngle);
		}

		for (int32 Index = 0; Index < DataSectorAngles.Num() - 1; Index++)
		{
			const FVector2D MouseLocPos = (MouseDir * MouseVectorSize).GetRotated(FMath::Fmod(-StartAngle + 270, 360.f));
			const float TargetAngle = FMath::RadiansToDegrees(FMath::Atan2(MouseLocPos.Y, MouseLocPos.X) + PI);

			bool bInside = FGraphHelper::IsInsideSector(TargetAngle, DataSectorAngles[Index], DataSectorAngles[Index + 1]);
			if (bInside)
			{
				//1.distance <= radiu
				TSharedPtr<FGraphDataCircle> Data = StaticCastSharedPtr<FGraphDataCircle>(DataPtrs[Index]);
				bool bDistVaild = (MouseVectorSize <= Radius * Data->Thickness);
				if (bDistVaild && ChartType == ECircleChartType::Ring)
				{
					const float InnerRingDistance = Radius - Radius * 0.5 * (FMath::Lerp(0.f, 1.f, FMath::Sqrt(Thickness)));
					bDistVaild = (MouseVectorSize >= InnerRingDistance);
				}
				
				if (bDistVaild)
				{
					DataHoverIndex = Index;
					break;
				}
			}
		}
	}

	if (DataHoverIndex != INDEX_NONE)
	{
		bDataHovering = true;
		HoverDataPtr = DataPtrs[DataHoverIndex];
	}
	OnDataHover.ExecuteIfBound(bDataHovering, HoverDataPtr, DataHoverIndex, MyGeometry, MouseEvent);
}

#undef LOCTEXT_NAMESPACE