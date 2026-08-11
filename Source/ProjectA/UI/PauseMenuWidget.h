// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButtonWidget;
class UControlsWidget;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class PROJECTA_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButtonWidget> ResumeButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButtonWidget> ControlsButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButtonWidget> MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UControlsWidget> ControlsWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

public:
	UPauseMenuWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;


protected:
	UFUNCTION()
	void OnResumeButtonClicked();

	UFUNCTION()
	void OnControlsButtonClicked();

	UFUNCTION()
	void OnMainMenuButtonClicked();

	UFUNCTION()
	void OnControlsBackButtonClicked();

};
