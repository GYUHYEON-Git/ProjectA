// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UStatBarWidget;
class UTextWidget;
/**
 * 
 */
UCLASS()
class PROJECTA_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UStatBarWidget> StaminaBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UStatBarWidget> HealthBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UTextWidget> EquipmentTextWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UTextWidget> InteractionTextWidget;

public:
	UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;

	void OnAttributeChanged(EAttributeType AttributeType, float InValue);

public:
	FORCEINLINE UTextWidget* GetEquipmentTextWidget() { return EquipmentTextWidget; }
	FORCEINLINE UTextWidget* GetInteractionTextWidget() { return InteractionTextWidget; }

	void SetTextBlock(const FString& EquipmentName);
	
	void SetTextWidgetVisiblity(bool bVisible);
};
