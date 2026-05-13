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
	BroadcastAttributeChanged(EAttributeType::Stamina);
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

void UAttributeComponent::BroadcastAttributeChanged(EAttributeType AttributeType) {
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

void UAttributeComponent::TakeDamageAmount(float DamageAmount) {
	BaseHealth = FMath::Clamp(BaseHealth - DamageAmount, 0.f, MaxHealth);

	BroadcastAttributeChanged(EAttributeType::Health);

	if (BaseHealth <= 0.f) {
		// Call Death Delegate
		if (OnDeath.IsBound()) {
			OnDeath.Broadcast();
		}

		// Set Death State
		if (UStateComponent* StateComp = GetOwner()->FindComponentByClass<UStateComponent>()) {
			StateComp->SetState(MyGameplayTags::Character_State_Death);
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

