// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interact.h"
#include "PickupItem.generated.h"

class AEquipment;

UCLASS()
class PROJECTA_API APickupItem : public AActor, public IInteract
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AEquipment> EquipmentClass;

public:	
	APickupItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	FORCEINLINE void SetEquipmentClass(const TSubclassOf<AEquipment>& NewEquipmentClass) { EquipmentClass = NewEquipmentClass; }
	virtual void Interact(AActor* InteractorActor) override;

};
