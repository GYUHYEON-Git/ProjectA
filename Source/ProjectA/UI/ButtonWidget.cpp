// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

UButtonWidget::UButtonWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {}

void UButtonWidget::NativePreConstruct() {
	Super::NativePreConstruct();


}


void UButtonWidget::SetText(FText text, float size) {
	if (Text) {
		Text->SetText(text);
		FSlateFontInfo FontInfo = Text->GetFont();
		FontInfo.Size = size;
		Text->SetFont(FontInfo);
	}
}
