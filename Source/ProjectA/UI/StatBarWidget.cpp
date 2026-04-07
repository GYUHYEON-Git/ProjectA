// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatBarWidget.h"

#include "Components/ProgressBar.h"

UStatBarWidget::UStatBarWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {}

void UStatBarWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	if (StatBar) {
		StatBar->SetFillColorAndOpacity(FillColorAndOpacity);
	}
}

void UStatBarWidget::SetRatio(float Ratio) const {
	if (StatBar) {
		StatBar->SetPercent(Ratio);
	}
}