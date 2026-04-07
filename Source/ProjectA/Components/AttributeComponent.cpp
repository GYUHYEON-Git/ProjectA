// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UAttributeComponent::CheckHasEnoughStamina(float StaminaCost) const {
	return CurrentStamina >= StaminaCost;
}

void UAttributeComponent::DecreaseStamina(float StaminaCost) {
	CurrentStamina = FMath::Clamp(CurrentStamina - StaminaCost, 0.f, MaxStamina);
	BroadcastOnAttributeType(EAttributeType::Stamina);
}

void UAttributeComponent::ToggleStaminaRegeneration(bool bEnable, float StartDelay) {
	if (bEnable) {
		if (GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimeHandler) == false) {
			GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimeHandler, this, &ThisClass::RegenrateStaminaHandler, 0.1f, true, StartDelay);
		}
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimeHandler);
	}
}

void UAttributeComponent::BroadcastOnAttributeType(EAttributeType AttributeType) {
	float Ratio = 0.f;
	switch (AttributeType) {
	case EAttributeType::Stamina:
		Ratio = GetStaminaRatio();
		break;
	case EAttributeType::Health:
		break;
	}
	OnAttributeChanged.Broadcast(AttributeType, Ratio);
}

void UAttributeComponent::RegenrateStaminaHandler() {
	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate, 0.f, MaxStamina);
	BroadcastOnAttributeType(EAttributeType::Stamina);
	if (CurrentStamina >= MaxStamina) {
		ToggleStaminaRegeneration(false);
	}
}

