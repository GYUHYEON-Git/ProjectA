// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

#include "MyGameplayTags.h"
#include "Components/StateComponent.h"

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
	// Updates the stamina bar UI through a broadcast when stamina decreases.
	BroadcastAttributeChanged(EAttributeType::Stamina);
}

void UAttributeComponent::ToggleStaminaRegeneration(bool bEnable, float StartDelay) {
	if (bEnable) {
		if (!GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimeHandler)) {
			GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimeHandler, this, &ThisClass::RegenrateStaminaHandler, 0.1f, true, StartDelay);
		}
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimeHandler);
	}
}

void UAttributeComponent::BroadcastAttributeChanged(EAttributeType AttributeType) {
	float Ratio = 0.f;
	switch (AttributeType) {
	case EAttributeType::Stamina:
		Ratio = GetStaminaRatio();
		break;
	case EAttributeType::Health:
		Ratio = GetHealthRatio();
		break;
	}
	OnAttributeChanged.Broadcast(AttributeType, Ratio);
}

void UAttributeComponent::TakeDamageAmount(float DamageAmount) {
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	// Updates the health bar UI through a broadcast when health decreases.
	BroadcastAttributeChanged(EAttributeType::Health);

	if (CurrentHealth <= 0.f) {
		if (UStateComponent* StateComp = GetOwner()->FindComponentByClass<UStateComponent>()) {
			StateComp->SetState(MyGameplayTags::Character_State_Death);
		}	
		if (OnDeath.IsBound()) {
			OnDeath.Broadcast();
		}
	}
}

void UAttributeComponent::RegenrateStaminaHandler() {
	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate, 0.f, MaxStamina);
	BroadcastAttributeChanged(EAttributeType::Stamina);
	if (CurrentStamina >= MaxStamina) {
		ToggleStaminaRegeneration(false);
	}
}