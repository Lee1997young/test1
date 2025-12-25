#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Guid.h"
#include "Layout/Geometry.h"
#include "Input/Events.h"
#include "SCUITypes.generated.h"

struct FSegment2D
{
	FVector2D PointStart;
	FVector2D PointEnd;

	FSegment2D() {}
	FSegment2D(const FVector2D& InPointStart, const FVector2D& InPointEnd)
		: PointStart(InPointStart)
		, PointEnd(InPointEnd)
	{}

	FVector2D GetDir() const
	{
		return (PointEnd - PointStart).GetSafeNormal();
	}

	int32 CheckPointOnSegmentX(float InPointX) const
	{
		if (InPointX < PointStart.X)
		{
			return -1;
		}
		else if (InPointX >= PointStart.X && InPointX <= PointEnd.X)
		{
			return 0;
		}
		else if (InPointX > PointEnd.X)
		{
			return 1;
		}

		return -1;
	}
};

struct FQuadPoints
{
	FVector2D LeftTop;
	FVector2D LeftBottom;
	FVector2D RightBottom;
	FVector2D RightTop;

	FQuadPoints(const FVector2D& LT, const FVector2D& LB, const FVector2D& RB, const FVector2D& RT)
		: LeftTop(LT)
		, LeftBottom(LB)
		, RightBottom(RB)
		, RightTop(RT)
	{}
};

struct FOnPaintHandlerParams
{
	const FGeometry& Geometry;
	const FSlateRect& ClippingRect;
	FSlateWindowElementList& OutDrawElements;
	int32 Layer;
	const bool bEnabled;

	FOnPaintHandlerParams(const FGeometry& InGeometry, const FSlateRect& InClippingRect, FSlateWindowElementList& InOutDrawElements, int32 InLayer, bool bInEnabled)
		: Geometry(InGeometry)
		, ClippingRect(InClippingRect)
		, OutDrawElements(InOutDrawElements)
		, Layer(InLayer)
		, bEnabled(bInEnabled)
	{
	}
};

UENUM(BlueprintType)
enum ECircleChartType
{
	Pie,
	Ring,
};

UENUM(BlueprintType)
enum ECircleChartDataTextType
{
	DataPercent,
	DataValue,
};

USTRUCT(BlueprintType)
struct SCUI_API FGraphDataBase
{
	GENERATED_USTRUCT_BODY()
		FGraphDataBase() {}
};

USTRUCT(BlueprintType)
struct SCUI_API FGraphData : public FGraphDataBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info")
		FString Label;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info")
		float Value;

	FGraphData() {}

	FGraphData(const FString& InLabel, float InValue)
		: Label(InLabel)
		, Value(InValue)
	{}
};

USTRUCT(BlueprintType)
struct SCUI_API FGraphDataColor : public FGraphData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info")
		FLinearColor Color;

	FGraphDataColor(){ Color = FColor::White; }

	FGraphDataColor(const FString& InLabel, float InValue)
		: FGraphData(InLabel, InValue)
	{
		Color = FColor::White;
	}

	FGraphDataColor(const FString& InLabel, float InValue, const FLinearColor& InColor)
		: FGraphData(InLabel, InValue)
	{
		Color = InColor;
	}
};

USTRUCT(BlueprintType)
struct SCUI_API FGraphDataGroup : public FGraphDataBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info")
		FString Label;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info")
		TArray<float> Values;

	FGraphDataGroup() {}

	FGraphDataGroup(const FString& InLabel, const TArray<float>& InValues)
		: Label(InLabel)
		, Values(InValues)
	{}
};


USTRUCT(BlueprintType)
struct SCUI_API FGraphDataCircle : public FGraphDataColor
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Thickness;

	FGraphDataCircle() { Thickness = 1.0f; }

	FGraphDataCircle(const FString& InLabel, float InValue)
		: FGraphDataColor(InLabel, InValue)
		, Thickness(1.0f)
	{
	}

	FGraphDataCircle(const FString& InLabel, float InValue, const FLinearColor& InColor, float InThickness)
		: FGraphDataColor(InLabel, InValue, InColor)
		, Thickness(InThickness)
	{
	}
};

USTRUCT(BlueprintType)
struct SCUI_API FGraphDataLine : public FGraphDataBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info")
		FLinearColor LineColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info")
		TArray<FGraphData> DataArray;

	FGraphDataLine() 
	{
		LineColor = FLinearColor::White;
	}

	FGraphDataLine(const FLinearColor& InColor, const TArray<FGraphData>& InDataArray)
		: LineColor(InColor)
		, DataArray(InDataArray)
	{}
};

UENUM(BlueprintType)
enum ETableBorderType
{
	BorderNone,
	BorderUpDown,
	BorderAll,
};

UENUM(BlueprintType)
enum ETableRowType
{
	RowNone,
	RowOdd,
	RowEven,
	RowAll
};

USTRUCT(BlueprintType)
struct SCUI_API FGraphDataTableRow : public FGraphDataBase
{
	GENERATED_USTRUCT_BODY()

		int32 Index;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info")
		FLinearColor RowColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SCUI Info")
		TArray<FString> RowDatas;

	FGraphDataTableRow()
	{
		Index = 0;
		RowColor = FLinearColor::White;
	}

	FGraphDataTableRow(const FLinearColor& InColor, const TArray<FString>& InRowDatas)
		: RowColor(InColor)
		, RowDatas(InRowDatas)
	{}

	FGraphDataTableRow(int32 InIndex, const FLinearColor& InColor, const TArray<FString>& InRowDatas)
		: Index(InIndex)
		, RowColor(InColor)
		, RowDatas(InRowDatas)
	{}
};


typedef TSharedPtr<FGraphDataBase> FGraphDataBasePtr;
typedef TSharedPtr<FGraphData> FGraphDataPtr;
typedef TSharedPtr<FGraphDataColor> FGraphDataColorPtr;
typedef TSharedPtr<FGraphDataCircle> FGraphDataCirclePtr;
typedef TSharedPtr<FGraphDataLine> FGraphDataLinePtr;
typedef TSharedPtr<FGraphDataTableRow> FGraphDataTableRowPtr;

DECLARE_DELEGATE_FiveParams(FOnDataHover, bool , FGraphDataBasePtr, int32, const FGeometry&, const FPointerEvent&)

