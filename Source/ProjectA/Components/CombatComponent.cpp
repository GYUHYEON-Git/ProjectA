// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Items/PickupItem.h"
#include "Equipments/Weapon.h"

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
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner())) {
			APickupItem* PickupItem = GetWorld()->SpawnActorDeferred<APickupItem>(APickupItem::StaticClass(), OwnerCharacter->GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			PickupItem->SetEquipmentClass(MainWeapon->GetClass());
			PickupItem->FinishSpawning(GetOwner()->GetActorTransform());
			MainWeapon->Destroy();
		}
	}
	MainWeapon = NewWeapon;
}