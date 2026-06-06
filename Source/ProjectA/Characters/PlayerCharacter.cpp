// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"

#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/AttributeComponent.h"
#include "Components/StateComponent.h"
#include "Components/CombatComponent.h"
#include "Components/TargetingComponent.h"
#include "Components/CapsuleComponent.h"
#include "UI/PlayerHUDWidget.h"
#include "MyGameplayTags.h"
#include "Interfaces/Interact.h"
#include "Equipments/Weapon.h"
#include "Equipments/FistWeapon.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


APlayerCharacter::APlayerCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("StringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("Targeting"));

	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();
	// Create the player HUD
	if (PlayerHUDWidgetClass) {
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (PlayerHUDWidget) {
			PlayerHUDWidget->AddToViewport();
		}
	}
	// Equip the fist weapon
	if (FistWeaponClass) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		AFistWeapon* FistWeapon = GetWorld()->SpawnActor<AFistWeapon>(FistWeaponClass, GetActorTransform(), SpawnParams);
		FistWeapon->EquipItem();
	}
}

void APlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Triggered, this, &ThisClass::Sprinting);
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Canceled, this, &ThisClass::Rolling);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);

		EnhancedInputComponent->BindAction(ToggleCombatAction, ETriggerEvent::Started, this, &ThisClass::ToggleCombat);
		// Automatically switch to Combat state
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AutoToggleCombat);
		// Normal attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &ThisClass::Attack);
		// Special attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ThisClass::SpecialAttack);
		// HeavyAttack
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ThisClass::HeavyAttack);
		EnhancedInputComponent->BindAction(LockOnTargetAction, ETriggerEvent::Started, this, &ThisClass::LockOnTarget);
		EnhancedInputComponent->BindAction(LeftTargetAction, ETriggerEvent::Started, this, &ThisClass::LeftTarget);
		EnhancedInputComponent->BindAction(RightTargetAction, ETriggerEvent::Started, this, &ThisClass::RightTarget);
	}
}

void APlayerCharacter::NotifyControllerChanged() {
	Super::NotifyControllerChanged();
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

float APlayerCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	StateComponent->SetState(MyGameplayTags::Character_State_Hit);
	StateComponent->ToggleMovementInput(false);

	if (AttributeComponent) {
		AttributeComponent->TakeDamageAmount(ActualDamage);
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

void APlayerCharacter::ImpactEffect(const FVector& Location) {
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location);
	}

	if (ImpactParticle) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location);
	}
}

void APlayerCharacter::HitReaction(const AActor* Attacker) {
	check(CombatComponent)
	if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker)) {
		PlayAnimMontage(HitReactAnimMontage);
	}
}

void APlayerCharacter::OnDeath() {
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

void APlayerCharacter::Move(const FInputActionValue& Values) {
	check(StateComponent);
	if (StateComponent->MovementInputEnabled() == false) return;

	FVector2D MovementVector = Values.Get<FVector2D>();
	if (Controller != nullptr) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardVector, MovementVector.Y);
		AddMovementInput(RightVector, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Values) {
	// Block input while in the LockedOn state
	if (TargetingComponent && TargetingComponent->IsLockOn()) {
		return;
	}
	FVector2D LookDirection = Values.Get<FVector2D>();
	if (Controller != nullptr) {
		AddControllerPitchInput(LookDirection.Y);
		AddControllerYawInput(LookDirection.X);
	}
}

bool APlayerCharacter::IsMoving() const {
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement()) {
		return CharacterMovementComponent->Velocity.Size2D() > 3.f && CharacterMovementComponent->GetCurrentAcceleration() != FVector::Zero();
	}
	return false;
}

bool APlayerCharacter::CanToggleCombat() const {
	check(StateComponent);
	if (IsValid(CombatComponent->GetMainWeapon()) == false) return false;
	if (CombatComponent->GetMainWeapon()->GetCombatType() == ECombatType::MeleeFists) return false;

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(MyGameplayTags::Character_State_Attacking);
	CheckTags.AddTag(MyGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(MyGameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(MyGameplayTags::Character_State_Death);
	return StateComponent->IsCrrentStateEqualToAny(CheckTags) == false;
}

bool APlayerCharacter::CanRolling() const {
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(MyGameplayTags::Character_State_Attacking);
	CheckTags.AddTag(MyGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(MyGameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(MyGameplayTags::Character_State_Death);
	return StateComponent->IsCrrentStateEqualToAny(CheckTags) == false;
}

void APlayerCharacter::Sprinting() {
	if (AttributeComponent->GetCurrentStamina() > 5.f && IsMoving()) {
		AttributeComponent->ToggleStaminaRegeneration(false);
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		AttributeComponent->DecreaseStamina(0.1f);
		bSprinting = true;
	}
	else {
		StopSprint();
	}
}

void APlayerCharacter::StopSprint() {
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	AttributeComponent->ToggleStaminaRegeneration(true);
	bSprinting = false;
}

void APlayerCharacter::Rolling() {
	check(AttributeComponent);
	check(StateComponent);
	if (CanRolling() == false) return;
	if (AttributeComponent->CheckHasEnoughStamina(15.f)) {
		AttributeComponent->ToggleStaminaRegeneration(false);
		StateComponent->ToggleMovementInput(false);
		AttributeComponent->DecreaseStamina(15.f);

		FVector InputVector = GetLastMovementInputVector();
		FRotator TargetRotation;
		if (InputVector.IsNearlyZero()) TargetRotation = GetActorRotation();
		else TargetRotation = InputVector.Rotation();
		SetActorRotation(TargetRotation);

		PlayAnimMontage(RollingMontage);
		StateComponent->SetState(MyGameplayTags::Character_State_Rolling);
		AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
	}
}

void APlayerCharacter::Interact() {
	FHitResult OutHit;
	const FVector Start = GetActorLocation();
	const FVector End = Start;
	constexpr float Radius = 100.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_INTERACTION));

	TArray<AActor*> ActorsToIgnore;

	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true);
	if (bHit) {
		if (AActor* HitActor = OutHit.GetActor()) {
			if (IInteract* Interaction = Cast<IInteract>(HitActor)) {
				Interaction->Interact(this);
			}
		}
	}	
}

void APlayerCharacter::ToggleCombat() {
	check(CombatComponent);
	if (CombatComponent) {
		if (const AWeapon* Weapon = CombatComponent->GetMainWeapon()) {
			if (CanToggleCombat()) {
				StateComponent->SetState(MyGameplayTags::Character_State_GeneralAction);
				if (CombatComponent->IsCombatEnabled()) {
					PlayAnimMontage(Weapon->GetMontageForTag(MyGameplayTags::Character_Action_Unequip));
				}
				else {
					PlayAnimMontage(Weapon->GetMontageForTag(MyGameplayTags::Character_Action_Equip));
				}
			}
		}
	}
}

void APlayerCharacter::AutoToggleCombat() {
	if (CombatComponent) {
		if (!CombatComponent->IsCombatEnabled()) {
			ToggleCombat();
		}
	}
}

void APlayerCharacter::Attack() {
	const FGameplayTag AttackTypeTag = GetAttackPerform();
	if (CanPerformAttack(AttackTypeTag)) {
		ExecuteComboAttack(AttackTypeTag);
	}
}

void APlayerCharacter::SpecialAttack() {
	const FGameplayTag AttackTypeTag = MyGameplayTags::Character_Attack_Special;
	if (CanPerformAttack(AttackTypeTag)) {
		ExecuteComboAttack(AttackTypeTag);
	}
}

void APlayerCharacter::HeavyAttack() {
	AutoToggleCombat();
	const FGameplayTag AttackTypeTag = MyGameplayTags::Character_Attack_Heavy;
	if (CanPerformAttack(AttackTypeTag)) {
		ExecuteComboAttack(AttackTypeTag);
	}
}

void APlayerCharacter::LockOnTarget() {
	TargetingComponent->ToggleLockOn();
}

void APlayerCharacter::LeftTarget() {
	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Left);
}

void APlayerCharacter::RightTarget() {
	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Right);
}

FGameplayTag APlayerCharacter::GetAttackPerform() const {
	if (IsSprinting()) {
		return MyGameplayTags::Character_Attack_Running;
	}
	return MyGameplayTags::Character_Attack_Light;
}

bool APlayerCharacter::CanPerformAttack(const FGameplayTag& AttackWeaponTag) const {
	check(StateComponent);
	check(CombatComponent);
	check(AttributeComponent);
	if (IsValid(CombatComponent->GetMainWeapon()) == false) {
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(MyGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(MyGameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(MyGameplayTags::Character_State_Hit);
	CheckTags.AddTag(MyGameplayTags::Character_State_Death);

	const float StaminaCost = CombatComponent->GetMainWeapon()->GetStaminaCost(AttackWeaponTag);
	return StateComponent->IsCrrentStateEqualToAny(CheckTags) == false
		&& CombatComponent->IsCombatEnabled()
		&& AttributeComponent->CheckHasEnoughStamina(StaminaCost);
}

void APlayerCharacter::DoAttack(const FGameplayTag& AttackTypeTag) {
	check(StateComponent);
	check(AttributeComponent);
	check(CombatComponent);

	if (const AWeapon* Weapon = CombatComponent->GetMainWeapon()) {
		if (Weapon->HasValidMontage(AttackTypeTag) == false) {
			return;
		}

		StateComponent->SetState(MyGameplayTags::Character_State_Attacking);
		StateComponent->ToggleMovementInput(false);
		CombatComponent->SetLastAttackType(AttackTypeTag);

		AttributeComponent->ToggleStaminaRegeneration(false);

		UAnimMontage* Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		if (!Montage) {
			// Combo limit reached
			ComboCounter = 0;
			Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		}
		PlayAnimMontage(Montage);
		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
	}
}

void APlayerCharacter::ExecuteComboAttack(const FGameplayTag& AttackTypeTag) {
	if (StateComponent->GetCurrentState() != MyGameplayTags::Character_State_Attacking) {
		if (bComboSequenceRunning && bCanComboInput == false) {
			// The animation has ended, but the combo sequence is still valid - allow additional input
			ComboCounter++;
			UE_LOG(LogTemp, Warning, TEXT("Additional input : Combo Counter = %d"), ComboCounter);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT(">>> ComboSequence Started <<<"));
			ResetCombo();
			bComboSequenceRunning = true;

		}
		DoAttack(AttackTypeTag);
		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	}
	else if (bCanComboInput) {
		// The combo window is open - optimal timing for the next input
		bSavedComboInput = true;
	}
}

void APlayerCharacter::ResetCombo() {
	UE_LOG(LogTemp, Warning, TEXT("Combo Reset"));

	bComboSequenceRunning = false;
	bCanComboInput = false;
	bSavedComboInput = false;
	ComboCounter = 0;
}

void APlayerCharacter::EnableComboWindow() {
	bCanComboInput = true;
	UE_LOG(LogTemp, Warning, TEXT("Combo Window Opened : Combo ounter = %d"), ComboCounter);
}

void APlayerCharacter::DisableComboWindow() {
	check(CombatComponent);
	bCanComboInput = false;
	
	if (bSavedComboInput) {
		bSavedComboInput = false;
		ComboCounter++;
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed : Advancing to next combo = %d"), ComboCounter);
		DoAttack(CombatComponent->GetLastAttackType());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed : No input reeived"));
	}
}

void APlayerCharacter::AttackFinished(const float ComboResetDelay) {
	UE_LOG(LogTemp, Warning, TEXT("AttackFinished"));
	if (StateComponent) {
		StateComponent->ToggleMovementInput(true);
	}
	// End the combo sequence after ComboResetDelay
	GetWorld()->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &ThisClass::ResetCombo, ComboResetDelay, false);
}

void APlayerCharacter::ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) {
	if (CombatComponent) {
		CombatComponent->GetMainWeapon()->ActivateCollision(WeaponCollisionType);
	}
}

void APlayerCharacter::DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) {
	if (CombatComponent) {
		CombatComponent->GetMainWeapon()->DeactivateCollision(WeaponCollisionType);
	}
}
