// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"

#include "Define.h"
#include "MyGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/StateComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create Targeting Sphere & Set Collision
	TargetingSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TargetingSphere"));
	TargetingSphereComponent->SetupAttachment(GetRootComponent());
	TargetingSphereComponent->SetCollisionObjectType(COLLISION_OBJECT_TARGETING);
	TargetingSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetingSphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// LockOn Widget
	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidgetComponent"));
	LockOnWidgetComponent->SetupAttachment(GetRootComponent());
	LockOnWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	LockOnWidgetComponent->SetDrawSize(FVector2D(30.f, 30.f));
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetVisibility(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<UStateComponent>("State");

	// Bind to OnDeath Delegate
	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);

}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemyCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	float  ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (AttributeComponent) {
		AttributeComponent->TakeDamageAmount(ActualDamage);
		GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Cyan, FString::Printf(TEXT("HP : %f"), AttributeComponent->GetBaseHealth()));
		GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Cyan, FString::Printf(TEXT("Damaged : %f"), ActualDamage));
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

		// ShotDirection
		FVector ShotDirection = PointDamageEvent->ShotDirection;
		// ImpactPoint
		FVector ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
		// ImpactDirection
		FVector ImpactDirection = PointDamageEvent->HitInfo.ImpactNormal;
		// HitLocation
		FVector HitLocation = PointDamageEvent->HitInfo.Location;

		ImpactEffect(ImpactPoint);

		HitReaction(EventInstigator->GetPawn());
	}

	return ActualDamage;
}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::OnDeath() {
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent()) {
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Ragdoll
	if (USkeletalMeshComponent* MeshComp = GetMesh()) {
		MeshComp->SetCollisionProfileName("Ragdoll");
		MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		MeshComp->SetSimulatePhysics(true);
	}
}

void AEnemyCharacter::ImpactEffect(const FVector& Location) {
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location);
	}

	if (ImpactParticle) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location);
	}
}

void AEnemyCharacter::HitReaction(const AActor* Attacker) {
	if (UAnimMontage* HitReactAnimMontage = GetHitReactAnimation(Attacker)) {
		float DelaySeconds = PlayAnimMontage(HitReactAnimMontage);
	}
}

UAnimMontage* AEnemyCharacter::GetHitReactAnimation(const AActor* Attacker) const {
	// Calculate the LookAt rotation (the rotation required for this actor to face the attacker)
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Attacker->GetActorLocation());
	// Calculate the difference between the current actor rotation and the LookAt rotation
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), LookAtRotation);
	// Use only the rotation difference around the Z axis
	const float DeltaZ = DeltaRotation.Yaw;

	EHitDirection HitDirection = EHitDirection::Front;

	if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -45.f, 45.f)) {
		HitDirection = EHitDirection::Front;
		UE_LOG(LogTemp, Log, TEXT("Front"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 45.f, 135.f)) {
		HitDirection = EHitDirection::Left;
		UE_LOG(LogTemp, Log, TEXT("Left"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 135.f, 180.f)
		|| UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -180.f, -135.f)) {
		HitDirection = EHitDirection::Back;
		UE_LOG(LogTemp, Log, TEXT("Back"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -135.f, -45.f)) {
		HitDirection = EHitDirection::Right;
		UE_LOG(LogTemp, Log, TEXT("Right"));
	}

	UAnimMontage* SelectedMontage = nullptr;
	switch (HitDirection) {
	case EHitDirection::Front:
		SelectedMontage = HitReactAnimFront;
		break;
	case EHitDirection::Back:
		SelectedMontage = HitReactAnimBack;
		break;
	case EHitDirection::Left:
		SelectedMontage = HitReactAnimLeft;
		break;
	case EHitDirection::Right:
		SelectedMontage = HitReactAnimRight;
		break;
	}

	return SelectedMontage;
}

void AEnemyCharacter::OnTargeted(bool bTargeted) {
	if (LockOnWidgetComponent) {
		LockOnWidgetComponent->SetVisibility(bTargeted);
	}
}

bool AEnemyCharacter::CanBeTargeted() {
	if (!StateComponent) {
		return false;
	}
	FGameplayTagContainer TagCheck;
	TagCheck.AddTag(MyGameplayTags::Character_State_Death);
	return StateComponent->IsCrrentStateEqualToAny(TagCheck) == false;
}

