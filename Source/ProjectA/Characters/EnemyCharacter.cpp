// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"

#include "Define.h"
#include "MyGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/StateComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CombatComponent.h"
#include "Equipments/Weapon.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "PlayerCharacter.h"

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
	StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));

	// Bind to OnDeath Delegate
	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);

}

void AEnemyCharacter::BeginPlay() {
	Super::BeginPlay();

	// ¹«±â ÀåÂø.
	if (DefaultWeaponClass) {
		FActorSpawnParameters Params;
		Params.Owner = this;
		
		AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass, GetActorTransform(), Params);
		CombatComponent->SetCombatEnabled(true);
		Weapon->EquipItem();
	}
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemyCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

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
	check(CombatComponent)
	if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker)) {
		PlayAnimMontage(HitReactAnimMontage);
	}
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

void AEnemyCharacter::ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) {
	if (CombatComponent) {
		CombatComponent->GetMainWeapon()->ActivateCollision(WeaponCollisionType);
	}
}

void AEnemyCharacter::DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) {
	if (CombatComponent) {
		CombatComponent->GetMainWeapon()->DeactivateCollision(WeaponCollisionType);
	}
}

void AEnemyCharacter::PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate) {
	check(StateComponent)
	check(AttributeComponent)
	check(CombatComponent)

	if (const AWeapon* Weapon = CombatComponent->GetMainWeapon()) {
		StateComponent->SetState(MyGameplayTags::Character_State_Attacking);
		CombatComponent->SetLastAttackType(AttackTypeTag);
		AttributeComponent->ToggleStaminaRegeneration(false);
		if (UAnimMontage* Montage = Weapon->GetRandomMontageForTag(AttackTypeTag)) {
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance()) {
				AnimInstance->Montage_Play(Montage);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}
		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
	}
}

