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

	if (bIsLockOn == false) return;
	if (IsValid(Character) == false) return;
	if (IsValid(LockedTargetActor) == false) return;

	const float Distance = FVector::Distance(Character->GetActorLocation(), LockedTargetActor->GetActorLocation());

	if (ITargeting* Targeting = Cast<ITargeting>(LockedTargetActor)) {
		// Stop LockedOn if the target moves beyond TargetingRadius
		if (Targeting->CanBeTargeted() == false || Distance > TargetingRadius + 50.f) {
			GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("StopLockOn")));
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

	const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start,
		End,
		TargetingRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		DrawDebugType,
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


		// Ensure the target blocks the ECC_Visibility channel
		const bool bHit = UKismetSystemLibrary::LineTraceSingle(
			GetOwner(),
			Start,
			End,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			DrawDebugType,
			OutHit,
			true);

		if (bHit) {
			// Since this is based on the camera view, calculate the dot product in camera space
			// Calculate the dot product for the left direction
			if (InDirection == ESwitchingDirection::Left) {
				// The comparison vector points along the normal direction, so the result is inverted
				if (FVector::DotProduct(Camera->GetRightVector(), OutHit.Normal) > 0.f == false) {
					continue;
				}
			}

			// Calculate the dot product for the right direction
			if (InDirection == ESwitchingDirection::Right) {
				if (FVector::DotProduct(Camera->GetRightVector(), OutHit.Normal) < 0.f == false) {
					continue;
				}
			}

			AActor* HitActor = OutHit.GetActor();

			// Find the target closest to the player
			// Use the dot product of the camera ForwardVector and the LookAt vector to find the closest target in front
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), HitActor->GetActorLocation());
			float CheckValue = FVector::DotProduct(Camera->GetForwardVector(), LookAtRotation.Vector());

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

void UTargetingComponent::FaceLockOnActor() const {
	const FRotator CurrentControlRotation = Character->GetControlRotation();

	const FVector TargetLocation = LockedTargetActor->GetActorLocation() - FVector(0.f, 0.f, 150.f);
	const FRotator TargetLookAtRotation = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), TargetLocation);

	FRotator InterpRotation = FMath::RInterpTo(CurrentControlRotation, TargetLookAtRotation, GetWorld()->GetDeltaSeconds(), FaceLockOnRotationSpeed);

	// Preserve the existing ControlRotation.Roll and apply only the Pitch and Yaw from InterpRotation
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