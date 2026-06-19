// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TextWidget.h"

#include "Components/TextBlock.h"


UTextWidget::UTextWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {
}

void UTextWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	if (TextBox) {
		TextBox->SetText(Text);
	}
}

void UTextWidget::SetTextBlock(FString EquipmentName) {
	FSlateFontInfo FontInfo = TextBox->GetFont();
	FontInfo.Size = FontSize;
	TextBox->SetFont(FontInfo);
	Text = FText::FromString(EquipmentName);
	if (TextBox != nullptr) {
		TextBox->SetText(Text);
	}
}