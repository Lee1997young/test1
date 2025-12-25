#include "SCUIStyle.h"

#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "Framework/Application/SlateApplication.h"

TSharedPtr<FSlateStyleSet> FSCUIStyle::StyleInstance = NULL;

void FSCUIStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSCUIStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FSCUIStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FSCUIStyle::Get()
{
	return *StyleInstance;
}

FName FSCUIStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SCUIStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon4x4(4.0f, 4.0f);
const FVector2D Icon8x8(8.0f, 8.0f);
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedPtr<class FSlateStyleSet> FSCUIStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("SCUIStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SCUI")->GetContentDir() / TEXT("Slate"));

	Style->Set("WhiteBrush", new FSlateColorBrush(FLinearColor::White));
	Style->Set("GradientV", new IMAGE_BRUSH(TEXT("GradientV"), FVector2D(128, 128)));
	Style->Set("WhiteCircle", new IMAGE_BRUSH(TEXT("WhiteCircle"), FVector2D(1024, 1024)));
	Style->Set("WhiteRing", new IMAGE_BRUSH(TEXT("WhiteRing"), FVector2D(1024, 1024)));
	Style->Set("GroupBorder", new BORDER_BRUSH("GroupBorder", FMargin(2.0f / 16.0f)));
	Style->Set("GroupBox", new BOX_BRUSH("GroupBorder", FMargin(4.0f / 16.0f)));
	Style->Set("DataPoint", new IMAGE_BRUSH("DataPoint", Icon16x16));
	return Style;
}


