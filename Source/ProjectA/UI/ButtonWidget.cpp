// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

UButtonWidget::UButtonWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {}

void UButtonWidget::NativePreConstruct() {
	Super::NativePreConstruct();


}


void UButtonWidget::SetText(FText text) {
	if (Text) {
		Text->SetText(text);
		FSlateFontInfo FontInfo = Text->GetFont();
		FontInfo.Size = 36;
		Text->SetFont(FontInfo);
	}
}
