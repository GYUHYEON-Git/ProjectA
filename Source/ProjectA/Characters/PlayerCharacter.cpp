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
#include "UI/PlayerHUDWidget.h"
#include "MyGameplayTags.h"
#include "Interfaces/Interact.h"
#include "Equipments/Weapon.h"
#include "Equipments/FistWeapon.h"

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
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();
	// Player HUD를 생성
	if (PlayerHUDWidgetClass) {
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (PlayerHUDWidget) {
			PlayerHUDWidget->AddToViewport();
		}
	}
	// 주먹 무기 장착
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
		// Combat 상태로 자동 전환
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AutoToggleCombat);
		// 일반 공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &ThisClass::Attack);
		// 특수 공격
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
	// LockedOn 상태에서는 입력 차단.
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
	return StateComponent->IsCrrentStateEqualToAny(CheckTags) == false;
}

void APlayerCharacter::Sprinting() {
	if (AttributeComponent->GetBaseStamina() > 5.f && IsMoving()) {
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
	if (StateComponent->GetCurrentState() == MyGameplayTags::Character_State_Rolling) return;

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
		EDrawDebugTrace::ForDuration,
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
		StateComponent->SetState(MyGameplayTags::Character_State_Attacking);
		StateComponent->ToggleMovementInput(false);
		CombatComponent->SetLastAttackType(AttackTypeTag);

		AttributeComponent->ToggleStaminaRegeneration(false);

		UAnimMontage* Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		if (!Montage) {
			// 콤보 한계 도달
			ComboCounter = 0;
			Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		}
		PlayAnimMontage(Montage);

		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
	}
}

void APlayerCharacter::ExecuteComboAttack(const FGameplayTag& AttackTypeTag) {
	if (StateComponent->GetCurrentState() != MyGameplayTags::Character_State_Attacking) {
		if (bComboSequenceRunning && bCanComboInput == false) {
			// 애니메이션은 끝났지만 아직 콤보 시퀀스가 유효할 때 - 추가 입력 기회
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
		// 콤보 윈도우가 열려 있을 때 - 최적의 타이밍
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
	// ComboResetDelay 후에 콤보 시퀀스 종료
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
