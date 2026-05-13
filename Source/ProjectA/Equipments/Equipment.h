// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Equipment.generated.h"

UCLASS()
class PROJECTA_API AEquipment : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Equipment | Mesh")
	TObjectPtr<UStaticMesh> MeshAsset;

	UPROPERTY(VisibleAnywhere, Category = "Equipment | Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
public:	
	AEquipment();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void EquipItem();
	virtual void UnequipItem();
	virtual void AttachToOwner(FName SocketName);

};
