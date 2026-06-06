// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StateComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "MyGameplayTags.h"

UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UStateComponent::ClearState() {
	CurrentState = FGameplayTag::EmptyTag;
}

bool UStateComponent::IsCrrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const {
	return TagsToCheck.HasTagExact(CurrentState);
}

void UStateComponent::ToggleMovementInput(bool bEnabled, float Duration) {
	if (bEnabled) {
		FLatentActionInfo LatentAction;
		LatentAction.CallbackTarget = this;
		LatentAction.ExecutionFunction = "MovementInputEnableAction";
		LatentAction.Linkage = 0;
		LatentAction.UUID = 0;
		UKismetSystemLibrary::RetriggerableDelay(GetWorld(), Duration, LatentAction);
	}
	else {
		bMovementInputEnabled = false;
	}
}

void UStateComponent::MovementInputEnableAction() {
	bMovementInputEnabled = true;
	if (CurrentState != MyGameplayTags::Character_State_Death) {
		ClearState();
	}
}

