#include "Slate/SAdvHistogram.h"
#include "SCUIStyle.h"
#include "Fonts/FontMeasure.h"

#define LOCTEXT_NAMESPACE "SGraphBase"

void SAdvHistogram::Construct(const FArguments& InArgs)
{
	SCoordGraph::Construct(InArgs);
}

void SAdvHistogram::SetColors(const TArray<FLinearColor>& InColors)
{
	if (Colors != InColors)
	{
		Colors = InColors;
	}
}

int32 SAdvHistogram::OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const
{
	SCoordGraph::OnPaintAnim(InParams, bAnimPlaying, InAnimAlpha);

	FVector2D CoordOrigin = GetCoordOrigin();
	FVector2D CoordSize = GetCoordSize();

	float LabelSpace = 0.f;
	if (Orientation == EOrientation::Orient_Horizontal)
	{
		LabelSpace = CoordSize.Y / Labels.Num();
	}
	else if (Orientation == EOrientation::Orient_Vertical)
	{
		LabelSpace = CoordSize.X / Labels.Num();
	}

	//Draw
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

	if (DataPtrs.Num() > 0)
	{
		for (int32 i = 0; i < DataPtrs.Num(); i++)
		{
			//DataColorGroup
			TSharedPtr<FGraphDataGroup> DataGroup = StaticCastSharedPtr<FGraphDataGroup>(DataPtrs[i]);
			float ItemLocalWidth = LabelSpace / (DataGroup->Values.Num() + 2);
			float ItemSpaceWidth = ItemLocalWidth * 0.2f;

			for (int32 ItemIndex = 0; ItemIndex < DataGroup->Values.Num(); ItemIndex++)
			{
				const FString ItemLabel = DataGroup->Label;
				FLinearColor ItemColor = Colors.IsValidIndex(ItemIndex) ? Colors[ItemIndex] : FLinearColor::White;
				float DataValue = DataGroup->Values.IsValidIndex(ItemIndex) ? DataGroup->Values[ItemIndex] : 0.f;

				//Get Data Paint Position
				FVector2D ZeroPaintPos, ValuePaintPos;
				if (GetPaintPos(ItemLabel, MinValue, ZeroPaintPos) && GetPaintPos(ItemLabel, DataValue, ValuePaintPos))
				{
					FVector2D RefPaintPos = ValuePaintPos;
					float RefValue = DataValue;

					if (IsAnimPlaying)
					{
						//Draw Animation
						RefPaintPos = FMath::Lerp(ZeroPaintPos, ValuePaintPos, InAnimAlpha);
						RefValue = FMath::Lerp(MinValue, DataValue, InAnimAlpha);
					}

					FVector2D LocalOffset, LocalSize;
					switch (Orientation)
					{
					case Orient_Horizontal:
					{
						LocalOffset = ZeroPaintPos + FVector2D(0, ( -LabelSpace * 0.5 + (ItemIndex + 1) * (ItemLocalWidth + ItemSpaceWidth)) * (SizeScale * 0.5));
						LocalSize = FVector2D(RefPaintPos.X - ZeroPaintPos.X, ItemLocalWidth * 0.5 * SizeScale);
						break;
					}
					case Orient_Vertical:
					{
						LocalOffset = ZeroPaintPos + FVector2D(( -LabelSpace * 0.5 + (ItemIndex + 1) * (ItemLocalWidth + ItemSpaceWidth)) * (SizeScale * 0.5), -(ZeroPaintPos.Y - RefPaintPos.Y));
						LocalSize = FVector2D(ItemLocalWidth * 0.5 * SizeScale, ZeroPaintPos.Y - RefPaintPos.Y);
						break;
					}
					}

					FLinearColor DrawDataColor = ItemColor;
					if (DataHoverIndex == i)
					{
						DrawDataColor = ItemColor + FLinearColor(0.25f, 0.25f, 0.25f);
					}
					DrawDataColor *= GetWidgetOpacity();

					//Draw Square
					if (bDrawGradient)
					{
						TArray<FSlateGradientStop> GradientStops;
						EOrientation GradientOrientation = EOrientation::Orient_Vertical;
						switch (Orientation)
						{
						case Orient_Horizontal:
						{
							GradientOrientation = EOrientation::Orient_Vertical;
							GradientStops.Add(FSlateGradientStop(FVector2D::ZeroVector, ItemColor * 0.1));
							GradientStops.Add(FSlateGradientStop(LocalSize, DrawDataColor));
							break;
						}
						case Orient_Vertical:
						{
							GradientOrientation = EOrientation::Orient_Horizontal;
							GradientStops.Add(FSlateGradientStop(FVector2D::ZeroVector, ItemColor));
							GradientStops.Add(FSlateGradientStop(LocalSize, DrawDataColor * 0.1));
							break;
						}
						}

						FSlateDrawElement::MakeGradient(
							InParams.OutDrawElements,
							InParams.Layer,
							InParams.Geometry.ToPaintGeometry(LocalOffset, LocalSize),
							GradientStops,
							GradientOrientation
						);
					}
					else
					{
						FSlateDrawElement::MakeBox(
							InParams.OutDrawElements,
							InParams.Layer,
							InParams.Geometry.ToPaintGeometry(LocalOffset, LocalSize),
							GradientBrush,
							ESlateDrawEffect::None,
							DrawDataColor
						);
					}

					//Draw Data Text
					const FString DataString = FGraphHelper::ValueToString(RefValue);
					const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
					FVector2D TextSize = FontMeasureService->Measure(DataString, DataFont);

					FVector2D TextLocalOffset;
					switch (Orientation)
					{
					case Orient_Horizontal:
					{
						float TextLocalOffsetY = ZeroPaintPos.Y + (-LabelSpace * 0.5 + ItemLocalWidth * 0.5 + (ItemIndex + 1) * (ItemLocalWidth + ItemSpaceWidth)) * (SizeScale * 0.5);
						TextLocalOffset = FVector2D(RefPaintPos.X + 2.0f, (TextLocalOffsetY - TextSize.Y * 0.5));
						break;
					}
					case Orient_Vertical:
					{
						float TextLocalOffsetX = ZeroPaintPos.X + (-LabelSpace * 0.5 + ItemLocalWidth * 0.5 + (ItemIndex + 1) * (ItemLocalWidth + ItemSpaceWidth)) * (SizeScale * 0.5);
						TextLocalOffset = FVector2D((TextLocalOffsetX - TextSize.X * 0.5), (RefPaintPos.Y - TextSize.Y));
						break;
					}
					}

					FSlateDrawElement::MakeText
					(
						InParams.OutDrawElements,
						InParams.Layer,
						InParams.Geometry.ToPaintGeometry(TextLocalOffset, TextSize),
						DataString,
						DataFont,
						ESlateDrawEffect::None,
						DataColor.GetSpecifiedColor() * GetWidgetOpacity()
					);
				}
			}

		}
	}

	return InParams.Layer;
}


void SAdvHistogram::HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	InitHoverData();

	FVector2D CoordOrigin = GetCoordOrigin();
	FVector2D CoordSize = GetCoordSize();

	float LabelSpace = 0.f;
	if (Orientation == EOrientation::Orient_Horizontal)
	{
		LabelSpace = CoordSize.Y / Labels.Num();
	}
	else if (Orientation == EOrientation::Orient_Vertical)
	{
		LabelSpace = CoordSize.X / Labels.Num();
	}

	if (DataPtrs.Num() > 0)
	{
		for (int32 i = 0; i < DataPtrs.Num(); i++)
		{
			//DataColorGroup
			TSharedPtr<FGraphDataGroup> DataGroup = StaticCastSharedPtr<FGraphDataGroup>(DataPtrs[i]);

			//Get Data Paint Position
			FVector2D ZeroPaintPos, ValuePaintPos;
			if (GetPaintPos(DataGroup->Label, MinValue, ZeroPaintPos) && GetPaintPos(DataGroup->Label, MaxValue, ValuePaintPos))
			{
				FVector2D LocalOffset, LocalSize;
				
				switch (Orientation)
				{
				case Orient_Horizontal:
				{
					LocalOffset = ZeroPaintPos + FVector2D(0, -(LabelSpace * 0.5) * (SizeScale * 0.5));
					LocalSize = FVector2D(ValuePaintPos.X - ZeroPaintPos.X, LabelSpace * 0.5 * SizeScale);
					break;
				}
				case Orient_Vertical:
				{
					LocalOffset = ZeroPaintPos + FVector2D(-(LabelSpace * 0.5) * (SizeScale * 0.5), -(ZeroPaintPos.Y - ValuePaintPos.Y));
					LocalSize = FVector2D(LabelSpace * 0.5 * SizeScale, ZeroPaintPos.Y - ValuePaintPos.Y);
					break;
				}
				}

				//Check MousePosition IsInside Data Area
				FBox2D DataBox2D(LocalOffset, LocalOffset + LocalSize);
				if (DataBox2D.IsInside(MousePosition))
				{
					DataHoverIndex = i;
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