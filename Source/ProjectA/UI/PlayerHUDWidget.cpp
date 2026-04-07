// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"

#include "Components/AttributeComponent.h"
#include "UI/StatBarWidget.h"

UPlayerHUDWidget::UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {
}

void UPlayerHUDWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (APawn* Pawn = GetOwningPlayerPawn()) {
		if (UAttributeComponent* Attribute = Pawn->GetComponentByClass<UAttributeComponent>()) {
			Attribute->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
			Attribute->BroadcastOnAttributeType(EAttributeType::Stamina);
		}
	}
}

void UPlayerHUDWidget::OnAttributeChanged(EAttributeType AttributeType, float InValue) {
	switch (AttributeType) {
	case EAttributeType::Stamina:
		StaminaBarWidget->SetRatio(InValue);
		break;
	case EAttributeType::Health:
		break;
	}
}