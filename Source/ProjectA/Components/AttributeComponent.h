// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateOnAttributeChanged, EAttributeType, float)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTA_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FDelegateOnAttributeChanged OnAttributeChanged;

protected:
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float CurrentStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRegenRate = 0.5f;

	UPROPERTY()
	FTimerHandle StaminaRegenTimeHandler;

public:	
	// Sets default values for this component's properties
	UAttributeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE float GetBaseStamina() const { return CurrentStamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE float GetStaminaRatio() const { return CurrentStamina / MaxStamina; }

public:
	bool CheckHasEnoughStamina(float StaminaCost) const;

	void DecreaseStamina(float StaminaCost);

	void ToggleStaminaRegeneration(bool bEnable, float StartDelay = 2.f);

	void BroadcastOnAttributeType(EAttributeType AttributeType);

private:
	void RegenrateStaminaHandler();
		
};
