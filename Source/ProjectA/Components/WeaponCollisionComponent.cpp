// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponCollisionComponent.h"

UWeaponCollisionComponent::UWeaponCollisionComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
}


void UWeaponCollisionComponent::BeginPlay() {
	Super::BeginPlay();


}


void UWeaponCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsCollisionEnabled) {
		CollisionTrace();
	}
}

void UWeaponCollisionComponent::TurnOnCollision() {
	AlreadyHitActors.Empty();
	bIsCollisionEnabled = true;
}

void UWeaponCollisionComponent::TurnOffCollision() {
	bIsCollisionEnabled = false;
}

void UWeaponCollisionComponent::SetWeaponMesh(UPrimitiveComponent* MeshComponent) {
	WeaponMesh = MeshComponent;
}

void UWeaponCollisionComponent::AddIgnoredActor(AActor* Actor) {
	IgnoredActors.Add(Actor);
}

void UWeaponCollisionComponent::RemoveIgnoredActor(AActor* Actor) {
	IgnoredActors.Remove(Actor);
}

bool UWeaponCollisionComponent::CanHitActor(AActor* Actor) const {
	return AlreadyHitActors.Contains(Actor) == false;
}

void UWeaponCollisionComponent::CollisionTrace() {
	TArray<FHitResult> OutHits;

	const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
	const FVector End = WeaponMesh->GetSocketLocation(TraceEndSocketName);

	bool const bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start,
		End,
		TraceRadius,
		TraceObjectTypes,
		false,
		IgnoredActors,
		DrawDebugType,
		OutHits,
		true);

	if (bHit) {
		for (const FHitResult& Hit : OutHits) {
			AActor* HitActor = Hit.GetActor();

			if (HitActor == nullptr) {
				continue;
			}

			if (CanHitActor(HitActor)) {
				AlreadyHitActors.Add(HitActor);

				// Call OnHitActor Broadcast
				if (OnHitActor.IsBound()) {
					OnHitActor.Broadcast(Hit);
				}
			}
		}
	}
}
