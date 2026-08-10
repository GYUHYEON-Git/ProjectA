// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlsWidget.generated.h"

class UTextBlock;
class UButtonWidget;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnControlsBackButtonClicked);

UCLASS()
class PROJECTA_API UControlsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UTextBlock> ActionText;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UTextBlock> ControlText;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButtonWidget> BackButton;

public:
	UPROPERTY(BlueprintAssignable)
	FOnControlsBackButtonClicked OnBackButtonClicked;

public:
	UControlsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleBackButtonClicked();

	
};
