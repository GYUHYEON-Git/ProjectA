// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TargetingComponent.h"


#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/Targeting.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<ACharacter>(GetOwner());
	if (Character) {
		Camera = Character->GetComponentByClass<UCameraComponent>();
	}
}

void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsLockOn) return;
	if (!IsValid(Character)) return;
	if (!IsValid(LockedTargetActor)) return;

	const float Distance = FVector::Distance(Character->GetActorLocation(), LockedTargetActor->GetActorLocation());

	if (ITargeting* Targeting = Cast<ITargeting>(LockedTargetActor)) {
		// Stop LockedOn if the target moves beyond TargetingRadius
		if (!Targeting->CanBeTargeted() || Distance > TargetingRadius + 50.f) {
			StopLockOn();
		}
		else {
			FaceLockOnActor();
		}
	}
}

void UTargetingComponent::ToggleLockOn() {
	if (bIsLockOn) {
		StopLockOn();
	}
	else {
		LockOnTarget();
	}
}

void UTargetingComponent::SwitchingLockedOnActor(ESwitchingDirection InDirection) {
	if (::IsValid(LockedTargetActor)) {
		if (ITargeting* Targeting = Cast<ITargeting>(LockedTargetActor)) {
			// Clear the current target
			Targeting->OnTargeted(false);

			// Assign the new target
			TArray<AActor*> OutTargets;
			FindTargets(OutTargets);
			AActor* TargetActor = FindClosestTarget(OutTargets, InDirection);

			if (::IsValid(TargetActor)) {
				if (ITargeting* NewTargeting = Cast<ITargeting>(TargetActor)) {
					LockedTargetActor = TargetActor;
					NewTargeting->OnTargeted(true);
				}
			}
			else {
				StopLockOn();
			}
		}
	}
}

void UTargetingComponent::FindTargets(TArray<AActor*>& OutTargetingActors) const {
	TArray<FHitResult> OutHits;
	const FVector Start = Character->GetActorLocation();
	const FVector End = Start;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_TARGETING));

	TArray<AActor*> ActorsToIgnore;

	// When the lock-on key is pressed, uses a SphereTrace to find targets within the lock-on range.
	const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start,
		End,
		TargetingRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true);

	if (bHit) {
		for (const FHitResult& HitResult : OutHits) {
			AActor* HitActor = HitResult.GetActor();
			if (ITargeting* Targeting = Cast<ITargeting>(HitActor)) {
				if (Targeting->CanBeTargeted()) {
					OutTargetingActors.Add(HitActor);
				}
			}
		}
	}
}

AActor* UTargetingComponent::FindClosestTarget(TArray<AActor*>& InTargets, ESwitchingDirection InDirection) const {
	float TargetCompareValue = -1.f;
	AActor* ClosestTarget = nullptr;

	for (const AActor* TargetActor : InTargets) {
		if (InDirection != ESwitchingDirection::None && LockedTargetActor == TargetActor) {
			continue;
		}
		FHitResult OutHit;
		const FVector Start = Camera->GetComponentLocation();
		const FVector End = TargetActor->GetActorLocation();

		TArray<AActor*> ActorsToIgnore;

		const bool bHit = UKismetSystemLibrary::LineTraceSingle(
			GetOwner(),
			Start,
			End,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			OutHit,
			true);

		if (bHit) {
			// Determines left or right using the dot product of the target's normal vector and the camera's right vector.
			// If the dot product is positive, the target is on the left.
			if (InDirection == ESwitchingDirection::Left) {
				if (!(FVector::DotProduct(Camera->GetRightVector(), OutHit.Normal) > 0.f)) {
					continue;
				}
			}
			// If the dot product is negative, the target is on the right.
			if (InDirection == ESwitchingDirection::Right) {
				if (!(FVector::DotProduct(Camera->GetRightVector(), OutHit.Normal) < 0.f)) {
					continue;
				}
			}
			AActor* HitActor = OutHit.GetActor();

			// Calculates the dot product between the player-to-target vector and the camera's forward vector, then selects the lock-on target based on the result.
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), HitActor->GetActorLocation());
			float CheckValue = FVector::DotProduct(Camera->GetForwardVector(), LookAtRotation.Vector());
			// The larger the dot product (the closer to 1), the closer the target is to the player's center view, so it is selected as the lock-on target.
			if (CheckValue > TargetCompareValue) {
				TargetCompareValue = CheckValue;
				ClosestTarget = HitActor;
			}
		}
	}
	return ClosestTarget;
}

void UTargetingComponent::OrientCamera() const {
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void UTargetingComponent::OrientMovement() const {
	Character->GetCharacterMovement()->bOrientRotationToMovement = true;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

// Locks the camera view onto the target.
void UTargetingComponent::FaceLockOnActor() const {
	const FRotator CurrentControlRotation = Character->GetControlRotation();
	const FVector TargetLocation = LockedTargetActor->GetActorLocation() - FVector(0.f, 0.f, 150.f);
	const FRotator TargetLookAtRotation = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), TargetLocation);

	FRotator InterpRotation = FMath::RInterpTo(CurrentControlRotation, TargetLookAtRotation, GetWorld()->GetDeltaSeconds(), FaceLockOnRotationSpeed);

	Character->GetController()->SetControlRotation(FRotator(InterpRotation.Pitch, InterpRotation.Yaw, CurrentControlRotation.Roll));
}

void UTargetingComponent::LockOnTarget() {
	TArray<AActor*> OutTargets;
	FindTargets(OutTargets);
	AActor* TargetActor = FindClosestTarget(OutTargets);
	if (::IsValid(TargetActor)) {
		LockedTargetActor = TargetActor;
		bIsLockOn = true;
		if (ITargeting* Targeting = Cast<ITargeting>(LockedTargetActor)) {
			Targeting->OnTargeted(bIsLockOn);
		}
		OrientCamera();
	}
}

void UTargetingComponent::StopLockOn() {
	bIsLockOn = false;
	if (ITargeting* Targeting = Cast<ITargeting>(LockedTargetActor)) {
		Targeting->OnTargeted(bIsLockOn);
	}
	LockedTargetActor = nullptr;
	OrientMovement();
}