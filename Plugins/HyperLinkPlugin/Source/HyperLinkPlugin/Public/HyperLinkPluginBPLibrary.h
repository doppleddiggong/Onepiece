#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/RichTextBlockDecorator.h"
#include "HyperLinkPluginBPLibrary.generated.h"

class FRichInlineHyperLinkDecorator : public FRichTextDecorator
{
public:
	FRichInlineHyperLinkDecorator(URichTextBlock* InOwner, UHyperLinkPluginBPLibrary* InDecorator);
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;

public:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override;

	FSimpleDelegate OnClickDelegate;
	mutable FString IdString;

private:
	UHyperLinkPluginBPLibrary* Decorator;
};

DECLARE_DELEGATE_TwoParams(FOnClickLink, const FString& /*LinkID*/, const FString& /*Content*/);

UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "HyperLink Decorator"))
class HYPERLINKPLUGIN_API UHyperLinkPluginBPLibrary : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	UHyperLinkPluginBPLibrary(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	UPROPERTY(EditAnywhere, Category = Appearance)
	FHyperlinkStyle Style;

	UFUNCTION(BlueprintNativeEvent, Category = "Hyperlink")
	void ClickFun(const FString& LinkID, const FString& Content);

	void SetNativeClickHandler(FOnClickLink InClick)
	{
		OnClickLink = InClick;
	}

private:
	FOnClickLink OnClickLink;
};
