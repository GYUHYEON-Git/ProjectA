// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Items/PickupItem.h"
#include "Equipments/Weapon.h"
#include "Equipments/Shield.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::SetWeapon(AWeapon* NewWeapon) {
	if (::IsValid(MainWeapon)) {
		if (const AActor* OwnerActor = GetOwner()) {
			SpawnPickupItem(OwnerActor, MainWeapon->GetClass(), MainWeapon->GetEquipmentName());
			MainWeapon->Destroy();
		}
	}
	MainWeapon = NewWeapon;
}

void UCombatComponent::SetShield(AShield* NewShield) {
	if (::IsValid(Shield)) {
		if (const AActor* OwnerActor = GetOwner()) {
			SpawnPickupItem(OwnerActor, Shield->GetClass(), Shield->GetEquipmentName());
			Shield->Destroy();
		}
	}
	Shield = NewShield;
}

void UCombatComponent::SetCombatEnabled(const bool bEnabled) {
	bCombatEnabled = bEnabled;
	if (OnChangedCombat.IsBound()) {
		OnChangedCombat.Broadcast(bCombatEnabled);
	}
}

void UCombatComponent::SpawnPickupItem(const AActor* OwnerActor, const TSubclassOf<AEquipment>& NewEquipmentClass, const FString NewEquipmentName) const {
	APickupItem* PickupItem = GetWorld()->SpawnActorDeferred<APickupItem>(APickupItem::StaticClass(), OwnerActor->GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	PickupItem->SetEquipmentClass(NewEquipmentClass);
	PickupItem->SetItemName(NewEquipmentName);
	PickupItem->FinishSpawning(GetOwner()->GetActorTransform());
	MainWeapon->Destroy();
}
