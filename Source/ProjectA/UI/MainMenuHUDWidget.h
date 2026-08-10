// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuHUDWidget.generated.h"

class UButtonWidget;
class UControlsWidget;
class UWidgetSwitcher;
class UImage;
/**
 * 
 */
UCLASS()
class PROJECTA_API UMainMenuHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButtonWidget> GameStartButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButtonWidget> ControlsButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButtonWidget> QuitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UControlsWidget> ControlsWidget;




public:
	UMainMenuHUDWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnGameStartButtonClicked();

	UFUNCTION()
	void OnControlsButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

	UFUNCTION()
	void OnControlsBackButtonClicked();

	
};
