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
#include "Components/RotationComponent.h"
#include "Components/MusicComponent.h"
#include "Equipments/Weapon.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"
#include "PlayerCharacter.h"
#include "UI/StatBarWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Damage.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameModes/GM_InGame.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	TargetingSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TargetingSphere"));
	TargetingSphereComponent->SetupAttachment(GetRootComponent());
	TargetingSphereComponent->SetCollisionObjectType(COLLISION_OBJECT_TARGETING);
	TargetingSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);	
	TargetingSphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidgetComponent"));
	LockOnWidgetComponent->SetupAttachment(GetRootComponent());
	LockOnWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	LockOnWidgetComponent->SetDrawSize(FVector2D(30.f, 30.f));
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetVisibility(false);

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	HealthBarWidgetComponent->SetDrawSize(FVector2D(100.f, 5.f));
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetVisibility(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	RotationComponent = CreateDefaultSubobject<URotationComponent>(TEXT("Rotation"));
	MusicComponent = CreateDefaultSubobject<UMusicComponent>(TEXT("Music"));

	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);
	AttributeComponent->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
}

void AEnemyCharacter::BeginPlay() {
	Super::BeginPlay();

	// Equip weapon.
	if (DefaultWeaponClass) {
		FActorSpawnParameters Params;
		Params.Owner = this;
		
		AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass, GetActorTransform(), Params);
		CombatComponent->SetCombatEnabled(true);
		Weapon->EquipItem();
	}
	// Set up the health bar
	SetupHealthBar();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AEnemyCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (AttributeComponent) {
		AttributeComponent->TakeDamageAmount(ActualDamage);
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		FVector ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
		FVector HitLocation = PointDamageEvent->HitInfo.Location;

		// Notify the AI so that it can perceive the damage.
		UAISense_Damage::ReportDamageEvent(GetWorld(), this, EventInstigator->GetPawn(), ActualDamage, HitLocation, HitLocation);

		ImpactEffect(ImpactPoint);

		HitReaction(EventInstigator->GetPawn());
	}

	return ActualDamage;
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	GetWorld()->GetTimerManager().ClearTimer(ParriedDelayTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(StunnedDelayTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyCharacter::OnDeath() {
	// Stop BehaviorTree
	if (AAIController* AIController = Cast<AAIController>(GetController())) {
		AIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	}
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent()) {
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	// Handles death using ragdoll physics.
	if (USkeletalMeshComponent* MeshComp = GetMesh()) {
		MeshComp->SetCollisionProfileName("Ragdoll");
		MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		MeshComp->SetSimulatePhysics(true);
		GetCharacterMovement()->DisableMovement();
	}
	if (AGM_InGame* GameMode = Cast<AGM_InGame>(UGameplayStatics::GetGameMode(GetWorld()))) {
		// Tracks the number of enemy characters placed in the level through the GameMode.
		GameMode->OnEnemyDied();
	}
}

void AEnemyCharacter::SetCombatUIAndAudioActive(bool bIsActive) {
	if (bIsActive) {
		ToggleHealthBarVisibility(true);
		StartMusic();
	}
	else {
		ToggleHealthBarVisibility(false);
		StopMusic();
	}
}

void AEnemyCharacter::OnAttributeChanged(EAttributeType AttributeType, float InValue) {
	if (AttributeType == EAttributeType::Health) {
		if (HealthBarWidgetComponent) {
			if (const UStatBarWidget* StatBar = Cast<UStatBarWidget>(HealthBarWidgetComponent->GetWidget())) {
				StatBar->SetRatio(InValue);
			}
		}
	}
}

void AEnemyCharacter::SetupHealthBar() {
	if (HealthBarWidgetComponent) {
		if (UStatBarWidget* StatBar = Cast<UStatBarWidget>(HealthBarWidgetComponent->GetWidget())) {
			StatBar->FillColorAndOpacity = FLinearColor::Red;
		}
	}
	if (AttributeComponent) {
		// Updates the progress bar with the current health value.
		AttributeComponent->BroadcastAttributeChanged(EAttributeType::Health);
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
	check(CombatComponent);
	float StunnedDelay = 0.f;
	// Stuns the enemy based on the configured stun probability. The stun duration is a random value within the specified range.
	if (StunnedRate >= FMath::RandRange(1, 100)) {
		StateComponent->SetState(MyGameplayTags::Character_State_Stunned);
		StunnedDelay = FMath::FRandRange(0.5f, 3.f);
	}
	// Clears the stun state after the hit animation duration plus the stun duration has elapsed.
	if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker)) {
		const float DelaySeconds = PlayAnimMontage(HitReactAnimMontage) + StunnedDelay;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]() 
			{
				// Clears the stun state.
				FGameplayTagContainer CheckTags;
				CheckTags.AddTag(MyGameplayTags::Character_State_Stunned);
				if (StateComponent->IsCurrentStateEqualToAny(CheckTags)) {
					StateComponent->ClearState();
				}
			});
		GetWorld()->GetTimerManager().SetTimer(StunnedDelayTimerHandle, TimerDelegate, DelaySeconds, false);
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
	return !StateComponent->IsCurrentStateEqualToAny(TagCheck);
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
	check(StateComponent);
	check(AttributeComponent);
	check(CombatComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(MyGameplayTags::Character_State_Stunned);
	CheckTags.AddTag(MyGameplayTags::Character_State_Parried);
	if (StateComponent->IsCurrentStateEqualToAny(CheckTags)) {
		return;
	}
	if (const AWeapon* Weapon = CombatComponent->GetMainWeapon()) {
		StateComponent->SetState(MyGameplayTags::Character_State_Attacking);
		CombatComponent->SetLastAttackType(AttackTypeTag);
		// Plays a random attack animation instead of using a fixed sequence like the player.
		if (UAnimMontage* Montage = Weapon->GetRandomMontageForTag(AttackTypeTag)) {
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance()) {
				AnimInstance->Montage_Play(Montage);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}
	}
}

void AEnemyCharacter::Parried() {
	check(StateComponent);
	check(CombatComponent);

	StopAnimMontage();
	StateComponent->SetState(MyGameplayTags::Character_State_Parried);

	if (const AWeapon* MainWeapon = CombatComponent->GetMainWeapon()) {
		// Stops the current animation and plays the ParriedHit animation.
		UAnimMontage* ParriedAnimMontage = MainWeapon->GetMontageForTag(MyGameplayTags::Character_Action_ParriedHit);
		const float Delay = PlayAnimMontage(ParriedAnimMontage) + 1.f;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]() {
			// Resets the state only if the character is not dead.
			FGameplayTagContainer CheckTags;
			CheckTags.AddTag(MyGameplayTags::Character_State_Death);
			if (!StateComponent->IsCurrentStateEqualToAny(CheckTags)) {
				StateComponent->ClearState();
			}
			});
		GetWorld()->GetTimerManager().SetTimer(ParriedDelayTimerHandle, TimerDelegate, Delay, false);
	}
}

void AEnemyCharacter::ToggleHealthBarVisibility(bool bVisibility) {
	if (HealthBarWidgetComponent) {
		HealthBarWidgetComponent->SetVisibility(bVisibility);
	}
}

void AEnemyCharacter::StartMusic() {
	MusicComponent->StartMusic();
}

void AEnemyCharacter::StopMusic() {
	MusicComponent->StopMusic();
}

