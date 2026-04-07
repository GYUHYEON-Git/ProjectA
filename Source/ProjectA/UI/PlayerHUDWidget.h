// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UStatBarWidget;
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

public:
	UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;

	void OnAttributeChanged(EAttributeType AttributeType, float InValue);
	
};
