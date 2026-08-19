// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MusicComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTA_API UMusicComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMusicComponent();

protected:
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundWave> MusicAsset;

	UPROPERTY()
	TObjectPtr<UAudioComponent> Music;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Volume = 0.5f;

	bool bStartedMusic = false;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void StartMusic();
	void StopMusic();
		
};
