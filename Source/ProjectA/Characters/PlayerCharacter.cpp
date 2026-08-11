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
#include "Components/SphereComponent.h"
#include "UI/PlayerHUDWidget.h"
#include "MyGameplayTags.h"
#include "Interfaces/Interact.h"
#include "Equipments/Weapon.h"
#include "Equipments/FistWeapon.h"
#include "Equipments/Shield.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Items/PickupItem.h"
#include "Animations/MyAnimInstance.h"
#include "PlayerControllers/PC_InGame.h"


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

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(300.f);
	SphereComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(COLLISION_OBJECT_INTERACTION, ECR_Overlap);

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("Targeting"));

	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);

	StaminaMap.Add("Sprinting", 10.f);
	StaminaMap.Add("Rolling", 15.f);
	StaminaMap.Add("Blocking", 20.f);
	StaminaMap.Add("Parrying", 10.f);
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();
	// Equip the fist weapon
	if (FistWeaponClass) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		AFistWeapon* FistWeapon = GetWorld()->SpawnActor<AFistWeapon>(FistWeaponClass, GetActorTransform(), SpawnParams);
		FistWeapon->EquipItem();
	}
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
}

void APlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (InteractableItems.Num() > 1) {
		GetClosestItem();
	}
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
		// 방어 자세
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &ThisClass::Blocking);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &ThisClass::BlockingEnd);
		// 패링
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &ThisClass::Parrying);
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

bool APlayerCharacter::IsDeath() const {
	check(StateComponent);
	
	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(MyGameplayTags::Character_State_Death);

	return StateComponent->IsCrrentStateEqualToAny(CheckTags);
}

float APlayerCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	check(AttributeComponent);
	check(StateComponent);

	if (!CanReceiveDamage()) {
		UE_LOG(LogTemp, Warning, TEXT("Rolling IFrames"));
		return ActualDamage;
	}

	FVector Forward = GetActorForwardVector();
	FVector ToTarget = EventInstigator->GetPawn()->GetActorLocation() - GetActorLocation();

	Forward.Z = 0.f;
	ToTarget.Z = 0.f;

	Forward.Normalize();
	ToTarget.Normalize();

	// 적과 대치중인 방향인지?
	bFacingEnemy = FVector::DotProduct(Forward, ToTarget) >= -0.1f;

	// 패링
	if (ParriedAttackSucceed()) {
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(EventInstigator->GetPawn())) {
			CombatInterface->Parried();
			AWeapon* MainWeapon = CombatComponent->GetMainWeapon();
			if (IsValid(MainWeapon)) {
				FVector Location = MainWeapon->GetActorLocation();
				ShieldBlockingEffect(Location);
			}
		}
		return ActualDamage;
	}

	StateComponent->ToggleMovementInput(false);

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		FVector ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
		ImpactEffect(ImpactPoint);
		HitReaction(EventInstigator->GetPawn());

		// 방패 방어가 가능한지?
		if (CanPerformAttackBlocking()) {
			AttributeComponent->ToggleStaminaRegeneration(false);
			AttributeComponent->DecreaseStamina(GetStamina("Blocking"));
			AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
		}
		else {
			AttributeComponent->TakeDamageAmount(ActualDamage);
			StateComponent->SetState(MyGameplayTags::Character_State_Hit);
		}
	}
	return ActualDamage;
}

void APlayerCharacter::ImpactEffect(const FVector& Location) {
	if (CanPerformAttackBlocking()) {
		ShieldBlockingEffect(Location);
	}
	else {
		if (ImpactSound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location);
		}
		if (ImpactParticle) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location);
		}
	}
}

void APlayerCharacter::ShieldBlockingEffect(const FVector& Location) const {
	if (BlockingSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BlockingSound, Location);
	}

	if (BlockingParticle) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlockingParticle, Location);
	}
}

void APlayerCharacter::HitReaction(const AActor* Attacker) {
	check(CombatComponent);

	if (CanPerformAttackBlocking()) {
		if (UAnimMontage* BlockingMontage = CombatComponent->GetMainWeapon()->GetMontageForTag(MyGameplayTags::Character_Action_BlockingHit)) {
			PlayAnimMontage(BlockingMontage);
		}
	}
	else {
		if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker)) {
			PlayAnimMontage(HitReactAnimMontage);
		}
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
		GetCharacterMovement()->DisableMovement();
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
	check(AttributeComponent);
	check(CombatComponent);
	if (CombatComponent->IsBlockingEnable()) {
		return;
	}

	if (AttributeComponent->GetCurrentStamina() > 5.f && IsMoving()) {
		AttributeComponent->ToggleStaminaRegeneration(false);
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		AttributeComponent->DecreaseStamina(GetStamina("Sprinting") * DeltaTime);
		bSprinting = true;
	}
	else {
		StopSprint();
	}
}

void APlayerCharacter::StopSprint() {
	check(AttributeComponent);
	check(CombatComponent);
	if (CombatComponent->IsBlockingEnable()) {
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	AttributeComponent->ToggleStaminaRegeneration(true);
	bSprinting = false;
}

void APlayerCharacter::Rolling() {
	check(AttributeComponent);
	check(StateComponent);
	if (CanRolling() == false) return;
	if (AttributeComponent->CheckHasEnoughStamina(GetStamina("Rolling"))) {
		AttributeComponent->ToggleStaminaRegeneration(false);
		StateComponent->ToggleMovementInput(false);
		AttributeComponent->DecreaseStamina(GetStamina("Rolling"));

		FVector InputVector = GetLastMovementInputVector();
		FRotator TargetRotation;
		if (InputVector.IsNearlyZero()) TargetRotation = GetActorRotation();
		else TargetRotation = InputVector.Rotation();
		SetActorRotation(TargetRotation);

		if (const AWeapon* Weapon = CombatComponent->GetMainWeapon()) {
			UAnimMontage* Montage = Weapon->GetMontageForTag(MyGameplayTags::Character_Action_Rolling, 0);
			if (!Montage) return;
			StateComponent->SetState(MyGameplayTags::Character_State_Rolling);
			PlayAnimMontage(Montage);
			AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
		}

	}
}

void APlayerCharacter::Interact() {
	TArray<FHitResult> OutHits;
	const FVector Start = GetActorLocation();
	const FVector End = Start;
	float Radius = SphereComponent->GetScaledSphereRadius();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_INTERACTION));

	TArray<AActor*> ActorsToIgnore;

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true);
	if (bHit) {
		for (const FHitResult& HitResult : OutHits) {
			AActor* HitActor = HitResult.GetActor();
			if (HitActor != GetClosestItem()) continue;
			if (IInteract* Interaction = Cast<IInteract>(HitActor)) {
				Interaction->Interact(this);
				break;
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

void APlayerCharacter::Blocking() {
	check(CombatComponent);
	check(StateComponent);
	if (CombatComponent->GetMainWeapon()) {
		if (CanPlayerBlockStance()) {
			GetCharacterMovement()->MaxWalkSpeed = BlockingSpeed;
			CombatComponent->SetBlockingEnabled(true);
			if (UMyAnimInstance* AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance())) {
				AnimInstance->UpdateBlocking(true);
				StateComponent->SetState(MyGameplayTags::Character_State_Blocking);
			}
		}
	}
}

void APlayerCharacter::BlockingEnd() {
	check(CombatComponent);
	check(StateComponent);
	CombatComponent->SetBlockingEnabled(false);
	if (UMyAnimInstance* AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance())) {
		AnimInstance->UpdateBlocking(false);
		StateComponent->ClearState();
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void APlayerCharacter::Parrying() {
	check(CombatComponent);
	check(StateComponent);
	check(AttributeComponent);
	if (CanPerformParry()) {
		if (const AWeapon* MainWeapon = CombatComponent->GetMainWeapon()) {
			UAnimMontage* ParryingMontage = MainWeapon->GetMontageForTag(MyGameplayTags::Character_Action_Parrying);
			StateComponent->ToggleMovementInput(false);
			AttributeComponent->ToggleStaminaRegeneration(false);
			AttributeComponent->DecreaseStamina(GetStamina("Parrying"));
			PlayAnimMontage(ParryingMontage);
			AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
		}
	}
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
	CheckTags.AddTag(MyGameplayTags::Character_State_Blocking);

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
		AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
	}
}

void APlayerCharacter::ExecuteComboAttack(const FGameplayTag& AttackTypeTag) {
	if (StateComponent->GetCurrentState() != MyGameplayTags::Character_State_Attacking) {
		if (bComboSequenceRunning && bCanComboInput == false) {
			// The animation has ended, but the combo sequence is still valid - allow additional input
			ComboCounter++;
			UE_LOG(LogTemp, Warning, TEXT("Additional input : Combo Counter = %d"), ComboCounter);
			GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT(">>> ComboSequence Started <<<"));
			ResetCombo();
			bComboSequenceRunning = true;

		}
		DoAttack(AttackTypeTag);
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

bool APlayerCharacter::CanPlayerBlockStance() const {
	check(StateComponent);
	check(CombatComponent);
	check(AttributeComponent);

	if (IsSprinting()) {
		return false;
	}
	AWeapon* Weapon = CombatComponent->GetMainWeapon();
	if (!IsValid(Weapon)) {
		return false;
	}
	AShield* Shield = CombatComponent->GetShield();
	if (!IsValid(Shield)) {
		return false;
	}
	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(MyGameplayTags::Character_State_Attacking);
	CheckTags.AddTag(MyGameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(MyGameplayTags::Character_State_Hit);
	CheckTags.AddTag(MyGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(MyGameplayTags::Character_State_Death);
	
	return StateComponent->IsCrrentStateEqualToAny(CheckTags) == false &&
		Weapon->GetCombatType() == ECombatType::SwordShield &&
		AttributeComponent->CheckHasEnoughStamina(1.f);
}

bool APlayerCharacter::CanPerformAttackBlocking() const {
	check(CombatComponent);
	check(AttributeComponent);

	return bFacingEnemy && CombatComponent->IsBlockingEnable() &&
		AttributeComponent->CheckHasEnoughStamina(1.f);
}

bool APlayerCharacter::CanPerformParry() const {
	check(StateComponent);
	check(CombatComponent);
	check(AttributeComponent);
	AWeapon* MainWeapon = CombatComponent->GetMainWeapon();
	if (!IsValid(MainWeapon)) return false;
	
	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(MyGameplayTags::Character_State_Attacking);
	CheckTags.AddTag(MyGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(MyGameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(MyGameplayTags::Character_State_Hit);
	CheckTags.AddTag(MyGameplayTags::Character_State_Blocking);
	CheckTags.AddTag(MyGameplayTags::Character_State_Death);
	CheckTags.AddTag(MyGameplayTags::Character_State_Parrying);

	return StateComponent->IsCrrentStateEqualToAny(CheckTags) == false &&
		MainWeapon->GetCombatType() == ECombatType::SwordShield &&
		AttributeComponent->CheckHasEnoughStamina(GetStamina("Parrying"));
}

bool APlayerCharacter::ParriedAttackSucceed() const {
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(MyGameplayTags::Character_State_Parrying);

	return StateComponent->IsCrrentStateEqualToAny(CheckTags) && bFacingEnemy;
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

void APlayerCharacter::ToggleIFrames(const bool bEnabled) {
	bEnabledIFrames = bEnabled;
}

void APlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (APickupItem* Item = Cast<APickupItem>(OtherActor)) {
		if (!GetHUDWidget()) return;
		InteractableItems.AddUnique(Item);
		GetClosestItem();
	}
}

void APlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (APickupItem* Item = Cast<APickupItem>(OtherActor)) {
		if (!GetHUDWidget()) return;
		InteractableItems.Remove(Item);
		GetClosestItem();
	}
}

UPlayerHUDWidget* APlayerCharacter::GetHUDWidget() const {
	if (APC_InGame* PC = Cast<APC_InGame>(GetController())) {
		return PC->GetPlayerHUDWidget();
	}
	return nullptr;
}

AActor* APlayerCharacter::GetClosestItem() {
	APickupItem* ClosestItem = nullptr;
	float MinDist = FLT_MAX;
	if (InteractableItems.IsEmpty()) {
		GetHUDWidget()->SetTextWidgetVisiblity(false);
		return nullptr;
	}
	for (APickupItem* Item : InteractableItems) {
		if (!IsValid(Item))	continue;
		FString Name = Item->GetName();
		float Dist = FVector::DistSquared(GetActorLocation(), Item->GetActorLocation());
		if (Dist < MinDist) {
			MinDist = Dist;
			ClosestItem = Item;
		}
	}
	FString CurrentInteractItem = ClosestItem->GetItemName();
	GetHUDWidget()->SetTextBlock(CurrentInteractItem);
	GetHUDWidget()->SetTextWidgetVisiblity(true);

	return ClosestItem;
}
