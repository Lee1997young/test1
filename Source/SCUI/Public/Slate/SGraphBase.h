#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SCUITypes.h"
#include "GraphHelper.h"
#include "Widgets/Layout/SBox.h"

class SCUI_API SGraphBase : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGraphBase)
		: _GraphPadding(FMargin(10.f))
		, _OnDataHover()
	{}
	SLATE_ARGUMENT(FMargin, GraphPadding)
	SLATE_EVENT(FOnDataHover, OnDataHover)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void SetDatas(const TArray<TSharedPtr<FGraphDataBase>>& InDatas);
	void SetLabelFont(const FSlateFontInfo& InFontInfo);
	void SetLabelTextColorAndOpacity(const FSlateColor& InColorAndOpacity);

	void SetValueFont(const FSlateFontInfo& InFontInfo);
	void SetValueTextColorAndOpacity(const FSlateColor& InColorAndOpacity);

	void SetDataFont(const FSlateFontInfo& InFontInfo);
	void SetDataTextColorAndOpacity(const FSlateColor& InColorAndOpacity);

	void SetGridColorAndOpacity(const FSlateColor& InColorAndOpacity);
	void SetSizeScale(float InSizeScale);

protected:
	FSlateFontInfo GetLabelFont() const;
	FSlateColor GetLabelColor() const;

	FSlateFontInfo GetValueFont() const;
	FSlateColor GetValueColor() const;

	FSlateFontInfo GetDataFont() const;
	FSlateColor GetDataColor() const;

	FSlateColor GetGridColor() const;

	float GetWidgetOpacity() const;

protected:
	virtual int32 OnPaintAnim(const FOnPaintHandlerParams& InParams, bool bAnimPlaying, float InAnimAlpha) const;

protected:
	void DrawLine(const FOnPaintHandlerParams& InParams, const FVector2D& InPoint1, const FVector2D& InPoint2, const FLinearColor& InLinearColor = FLinearColor::White, float InWidth = 1.0f) const;

	int32 DrawGradient(
		const FOnPaintHandlerParams& InParams,
		const FQuadPoints& QuadPoints,
		const FQuadPoints& QuadPointsUV,
		const FSlateResourceHandle& InHandle,
		const FLinearColor& InColor) const;

protected: //
	inline void InitHoverData()
	{
		bDataHovering = false;
		DataLineHoverIndex = INDEX_NONE;
		DataHoverIndex = INDEX_NONE;
		HoverDataPtr = nullptr;
	}

	virtual void HoverDataCheck(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

public:
	void PlayAnim(float Duration);
	void SetAnimProgress(float InAnimProgress);
	float GetAnimProgress() const;
private:
	void RequestActiveTimer();
	EActiveTimerReturnType ActiveTick(double InCurrentTime, float InDeltaTime);
	void CancelActiveTimer();

protected:
	//TimeHandle
	TWeakPtr<FActiveTimerHandle> ActiveTimerHandle;

	//Anim
	float AnimDuration;
	double LastTime;
	float TimeInterval;
	bool IsAnimPlaying;

	float AnimProgress = 1.f;

protected:
	FMargin GraphPadding;

	mutable FWidgetStyle WidgetStyle;

	TSharedPtr<SBox> DataBox;
	TArray<TSharedPtr<FGraphDataBase>> DataPtrs;
	FGraphDataBasePtr HoverDataPtr;

	FSlateFontInfo LabelFont;
	FSlateFontInfo ValueFont;
	FSlateFontInfo DataFont;
	FSlateColor LabelColor;
	FSlateColor ValueColor;
	FSlateColor DataColor;
	FSlateColor GridColor;
	float SizeScale;
	
protected:
	FVector2D MousePosition;
	int32 DataLineHoverIndex = INDEX_NONE;
	int32 DataHoverIndex = INDEX_NONE;
	bool bDataHovering = false;
	FOnDataHover OnDataHover;



};