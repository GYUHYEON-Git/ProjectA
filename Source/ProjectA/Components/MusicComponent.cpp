// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MusicComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

UMusicComponent::UMusicComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UMusicComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UMusicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMusicComponent::StartMusic() {
	if (MusicAsset) {
		if (!bStartedMusic) {
			bStartedMusic = true;
			Music = UGameplayStatics::SpawnSound2D(this, MusicAsset);
			Music->SetVolumeMultiplier(Volume);
			Music->FadeIn(1.f);
		}
	}
}

void UMusicComponent::StopMusic() {
	if (IsValid(Music)) {
		bStartedMusic = false;
		Music->FadeOut(2.f, 0);
	}
}