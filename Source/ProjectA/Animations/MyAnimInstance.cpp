// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/MyAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMyAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	Character = Cast<ACharacter>(GetOwningActor());
	if (Character) {
		MovementComponent = Character->GetCharacterMovement();
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
}
