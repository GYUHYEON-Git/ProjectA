// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DiedWidget.h"

#include "Kismet/GameplayStatics.h"
#include "UI/ButtonWidget.h"
#include "Components/Button.h"

UDiedWidget::UDiedWidget(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer) {}

void UDiedWidget::NativeConstruct() {
	if (MainMenuButton) {
		MainMenuButton->SetText(FText::FromString(TEXT("MainMenu")), 36.f);
		MainMenuButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnMainMenuButtonClicked);
	}
}

void UDiedWidget::OnMainMenuButtonClicked() {
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::OpenLevel(this, FName("Lv_MainMenu"));
}