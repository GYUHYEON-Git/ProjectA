// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DiedWidget.generated.h"

class UTextBlock;
class UButtonWidget;
/**
 * 
 */
UCLASS()
class PROJECTA_API UDiedWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UTextBlock> Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButtonWidget> MainMenuButton;

public:
	UDiedWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnMainMenuButtonClicked();
	
};
