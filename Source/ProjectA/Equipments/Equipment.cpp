// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/Equipment.h"

#include "GameFramework/Character.h"

AEquipment::AEquipment() {
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipmentMesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

void AEquipment::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEquipment::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	if (MeshAsset) {
		Mesh->SetStaticMesh(MeshAsset);
	}
}

void AEquipment::EquipItem() {
}

void AEquipment::UnequipItem() {
}

void AEquipment::AttachToOwner(FName SocketName) {
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner())) {
		if (USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh()) {
			AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
		}
	}
}