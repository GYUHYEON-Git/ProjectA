// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/MyAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Characters/PlayerCharacter.h"
#include "Components/StateComponent.h"
#include "Components/CombatComponent.h"


void UMyAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	Character = Cast<ACharacter>(GetOwningActor());
	if (Character) {
		MovementComponent = Character->GetCharacterMovement();
		if (UCombatComponent* CombatComponent = Character->GetComponentByClass<UCombatComponent>()) {
			CombatComponent->OnChangedCombat.AddUObject(this, &ThisClass::OnChangedCombat);
		}
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Character == nullptr) return;
	if (MovementComponent == nullptr) return;
	Velocity = MovementComponent->Velocity;
	Speed = Velocity.Size2D();
	bShouldMove = Speed > 3.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector;
	bIsFalling = MovementComponent->IsFalling();

	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
}

void UMyAnimInstance::AnimNotify_ResetMovementInput() {
	if (APlayerCharacter* LocalCharacter = Cast<APlayerCharacter>(GetOwningActor())) {
		LocalCharacter->GetStateComponent()->ToggleMovementInput(true);
	}
}

void UMyAnimInstance::AnimNotify_ResetState() {
	if (const APlayerCharacter* LocalCharacter = Cast<APlayerCharacter>(GetOwningActor())) {
		LocalCharacter->GetStateComponent()->ClearState();
	}
}

void UMyAnimInstance::UpdateCombatMode(const ECombatType InCombatType) {
	CombatType = InCombatType;
}

void UMyAnimInstance::OnChangedCombat(const bool bInCombatEnabled) {
	bCombatEnabled = bInCombatEnabled;
}
