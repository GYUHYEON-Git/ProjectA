// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"

#include "Components/AttributeComponent.h"
#include "UI/StatBarWidget.h"
#include "UI/TextWidget.h"

UPlayerHUDWidget::UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {
}

void UPlayerHUDWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (APawn* Pawn = GetOwningPlayerPawn()) {
		if (UAttributeComponent* Attribute = Pawn->GetComponentByClass<UAttributeComponent>()) {
			Attribute->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
			Attribute->BroadcastAttributeChanged(EAttributeType::Stamina);
			Attribute->BroadcastAttributeChanged(EAttributeType::Health);
			SetTextWidgetVisiblity(false);
		}
	}
}

void UPlayerHUDWidget::OnAttributeChanged(EAttributeType AttributeType, float InValue) {
	switch (AttributeType) {
	case EAttributeType::Stamina:
		StaminaBarWidget->SetRatio(InValue);
		break;
	case EAttributeType::Health:
		HealthBarWidget->SetRatio(InValue);
		break;
	}
}

void UPlayerHUDWidget::SetTextBlock(const FString& EquipmentName) {
	EquipmentTextWidget->SetTextBlock(EquipmentName);
}

void UPlayerHUDWidget::SetTextWidgetVisiblity(bool bVisible) {
	if (bVisible) {
		EquipmentTextWidget->SetVisibility(ESlateVisibility::Visible);
		InteractionTextWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		EquipmentTextWidget->SetVisibility(ESlateVisibility::Hidden);
		InteractionTextWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
