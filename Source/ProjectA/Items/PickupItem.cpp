// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickupItem.h"

#include "Define.h"
#include "Equipments/Equipment.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupItemMesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionObjectType(COLLISION_OBJECT_INTERACTION);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupItem::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	if (EquipmentClass) {
		if (AEquipment* CDO = EquipmentClass->GetDefaultObject<AEquipment>()) {
			Mesh->SetStaticMesh(CDO->MeshAsset);
			Mesh->SetSimulatePhysics(true);
		}
	}
}

void APickupItem::Interact(AActor* InteractorActor) {
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InteractorActor;
	AEquipment* SpawnItem = GetWorld()->SpawnActor<AEquipment>(EquipmentClass, GetActorTransform(), SpawnParams);
	if (SpawnItem) {
		SpawnItem->EquipItem();
		Destroy();
	}
}