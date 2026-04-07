// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAttributeComponent;
class UStateComponent;
class UPlayerHUDWidget;

UCLASS()
class PROJECTA_API APlayerCharacter : public ACharacter	
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// InputAction
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SprintRollingAction;

protected:
	// Montage
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> RollingMontage;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateComponent> StateComponent;

protected:
	// UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Movement Data")
	float NormalSpeed = 500.f;

	UPROPERTY(VisibleAnywhere, Category = "Movement Data")
	float SprintSpeed = 1000.f;

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyControllerChanged() override;

	void Move(const FInputActionValue& Values);

	void Look(const FInputActionValue& Values);

	FORCEINLINE UStateComponent* GetStateComponent() const { return StateComponent; }

protected:
	bool IsMoving() const;

	void Sprinting();

	void StopSprint();

	void Rolling();

};