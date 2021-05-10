// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterActionInterface.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Turn setting
	GetCharacterMovement()->bOrientRotationToMovement = true;
	this->bUseControllerRotationYaw = false;

	// Turn rate
	BaseTurnRate = 180.f;
	BaseLookUpRate = 180.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = true;

	// AI
	AIPerceptionStimuliSrc = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSource"));

	// Combo wait time
	LightAttackComboWaitTime = 1.2f;

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Set status
	hp = MaxHP;

	// UpdateUI
	UpdateHPUIEvent(hp / MaxHP);

	// Get weapon if is player
	if (CharaType == CharacterType::Player)
	{
		TArray<UActorComponent*> weapons = GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "Weapon");
		if (weapons.Num() > 0)
		{
			Weapon = Cast<USkeletalMeshComponent>(weapons[0]);
		}
	}
	WalkOn();
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LookAtTarget(DeltaTime);
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Move
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	// Dash
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ACharacterBase::Dash);
	PlayerInputComponent->BindAction("Dash", IE_Released, this, &ACharacterBase::CancelDash);

	// Walk/Run
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &ACharacterBase::ChangeWalkRun);

	// Sneak
	PlayerInputComponent->BindAction("Squat", IE_Pressed, this, &ACharacterBase::Squat);

	// Turn
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);

	// Combat
	PlayerInputComponent->BindAction("DrawWeapon", IE_Pressed, this, &ACharacterBase::DrawWeapon);
	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &ACharacterBase::LightAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &ACharacterBase::HeavyAttack);
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &ACharacterBase::Block);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &ACharacterBase::CancelBlock);
	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &ACharacterBase::LockOn);
}

void ACharacterBase::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && bEnableMovement)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && bEnableMovement)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::Dash()
{
	Dashing = true;
	Walking = false;
	Squating = false;
	SquatOn(false);
	GetCharacterMovement()->MaxWalkSpeed = 900.f;
}

void ACharacterBase::CancelDash()
{
	Dashing = false;
	Walking = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void ACharacterBase::ChangeWalkRun()
{
	Walking = !Walking;

	WalkOn();
}

void ACharacterBase::Squat()
{
	if (bDead) return;

	Squating = !Squating;
	SquatOn(Squating);
}

void ACharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::DrawWeapon()
{
	if (bDead) return;

	bWeaponDrawn = !bWeaponDrawn;
	DrawWeaponEvent(bWeaponDrawn);
	if (GetMesh()->GetAnimInstance()->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
	{
		ICharacterActionInterface::Execute_SetCombatMove(GetMesh()->GetAnimInstance(), bWeaponDrawn);
	}
}

void ACharacterBase::LightAttack()
{
	if (bDead) return;

	heavyAttackCount = 0;
	lightAttackCount %= 3;
	attackType = AttackType:: Light;
	GetWorldTimerManager().ClearTimer(comboResetTimerHandle);
	LightAttackEvent(bWeaponDrawn, lightAttackCount);
	GetWorldTimerManager().SetTimer(comboResetTimerHandle, this, &ACharacterBase::ResetComboCounter, LightAttackComboWaitTime, false);
}

void ACharacterBase::HeavyAttack()
{
	if (bDead) return;

	lightAttackCount = 0;
	heavyAttackCount %= 3;
	attackType = AttackType::Heavy;
	GetWorldTimerManager().ClearTimer(comboResetTimerHandle);
	HeavyAttackEvent(bWeaponDrawn, heavyAttackCount);
	GetWorldTimerManager().SetTimer(comboResetTimerHandle, this, &ACharacterBase::ResetComboCounter, 1.f, false);
}

void ACharacterBase::Block()
{
	if (bDead) return;

	bBlocking = true;
	BlockingEvent(bBlocking);
}

void ACharacterBase::CancelBlock()
{
	if (bDead) return;

	bBlocking = false;
	BlockingEvent(bBlocking);
	// Set movement enable cause animation interrupt
	bEnableMovement = true;
}

void ACharacterBase::LockOn()
{
	if (bDead) return;

	if (bLockOn)
	{
		bLockOn = false;
		target = nullptr;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else
	{
		if (SearchTarget())
		{
			bLockOn = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
}

void ACharacterBase::Hitted(float damage)
{
	if (bDead) return;

	// If is not blocking, take the damage
	if (!bBlocking)
	{
		hp -= damage;
		UpdateHPUIEvent(hp / MaxHP);

		if (hp <= 0)
		{
			hp = 0;
			bDead = true;
			bEnableMovement = false;
			bLockOn = false;
			target = nullptr;
			GetCharacterMovement()->bOrientRotationToMovement = !bLockOn;
			DeadEvent();
			return;
		}
	}

	// Don`t play hitted animation if is attacking
	if (bEnableMovement)
	{
		HittedEvent(bBlocking);
	}
}

void ACharacterBase::LockOn(AActor* actor)
{
	if (bDead) return;

	target = actor;
	bLockOn = (actor != nullptr);
	GetCharacterMovement()->bOrientRotationToMovement = !bLockOn;
}

bool ACharacterBase::IsEnemy()
{
	return CharaType != CharacterType::Player;
}

void ACharacterBase::SetSeen(bool seen)
{
	bSeen = seen;
}

bool ACharacterBase::GetSeen()
{
	return bSeen;
}

void ACharacterBase::MovementEnable_Implementation(bool enable)
{
	bEnableMovement = enable;
}

void ACharacterBase::AttackSwing_Implementation(float strength)
{
	FVector forward = GetCapsuleComponent()->GetForwardVector();
	GetCharacterMovement()->AddImpulse(forward * strength, true);
}

void ACharacterBase::WalkOn()
{
	if (Walking)
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void ACharacterBase::DashOn(bool enable)
{
	if (enable)
	{
		Dashing = true;
		Walking = false;
		SquatOn(false);
		GetCharacterMovement()->MaxWalkSpeed = 900.f;
	}
	else
	{
		Dashing = false;
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void ACharacterBase::SquatOn(bool enable)
{
	if (GetMesh()->GetAnimInstance()->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
	{
		ICharacterActionInterface::Execute_Squat(GetMesh()->GetAnimInstance(), enable);
	}

	if (enable)
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		this->Crouch();
	}
	else
	{
		WalkOn();
		this->UnCrouch();
	}
}

void ACharacterBase::ResetComboCounter()
{
	lightAttackCount = 0;
	heavyAttackCount = 0;
}

bool ACharacterBase::SearchTarget()
{
	FVector eyeLocation;
	FRotator eyeRotation;
	GetActorEyesViewPoint(eyeLocation, eyeRotation);
	eyeRotation = GetActorRotation();
	FVector lineEnd = eyeLocation + eyeRotation.Vector() * TargetSearchRange;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery;
	TArray<AActor*> actorsToIgnore;
	TArray<AActor*> allCharacterBaseActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterBase::StaticClass(), allCharacterBaseActors);
	actorsToIgnore.Add(this);
	// Ignore the same type character
	for (int i = 0; i < allCharacterBaseActors.Num(); i++)
	{
		ACharacterBase* chara = Cast<ACharacterBase>(allCharacterBaseActors[i]);
		if(chara && chara->CharaType == CharaType)
		{
			actorsToIgnore.Add(allCharacterBaseActors[i]);
		}
	}
	FHitResult hit;
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%f %f"), eyeLocation, eyeRotation));
	//DrawDebugLine(GetWorld(), eyeLocation, lineEnd, FColor::Yellow,false,100.0f);
	// Pawn
	objectTypeQuery.Add(ObjectTypeQuery3);
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), eyeLocation, lineEnd, TargetSearchRange, objectTypeQuery, false, actorsToIgnore, EDrawDebugTrace::Type::ForDuration, hit, true))
	{
		FCollisionQueryParams collisionQueryParams;
		collisionQueryParams.bTraceComplex = true;
		collisionQueryParams.AddIgnoredActor(this);
		FHitResult lineTracehit;
		FVector targetLocation = hit.Actor->GetActorLocation();
		lineEnd = FVector(targetLocation.X, targetLocation.Y, eyeLocation.Z);
		// Whether sight is blocked
		if (!GetWorld()->LineTraceSingleByChannel(lineTracehit, eyeLocation, lineEnd, ECC_Visibility, collisionQueryParams))
		{
			//DrawDebugLine(GetWorld(), eyeLocation, lineEnd, FColor::Red, false, 100.f);
			target = Cast<AActor>(hit.Actor);
			DrawDebugSphere(GetWorld(), targetLocation, 20.f, 12, FColor::Blue, false, 10.f);
			return true;
		}
	}
	return false;
}

void ACharacterBase::LookAtTarget(float deltaTime)
{
	if (!bLockOn || target == nullptr)
	{
		return;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), deltaTime));
	FRotator rotateToTarget = FRotationMatrix::MakeFromX(target->GetActorLocation() - GetActorLocation()).Rotator();
	FRotator rotator = FMath::RInterpConstantTo(GetActorRotation(), rotateToTarget, deltaTime, GetCharacterMovement()->RotationRate.Yaw);
	SetActorRotation(rotator);
}

float ACharacterBase::CalculateDamage()
{
	float damage = 0;
	switch (attackType)
	{
	case AttackType::Light:
		damage = Atk * FMath::Pow(LightAttackComboBonusRate, lightAttackCount);
		break;
	case AttackType::Heavy:
		damage = Atk * FMath::Pow(HeavyAttackComboBonusRate, heavyAttackCount);
		break;
	}
	return damage;
}

//void ACharacterBase::UpdateStatus()
//{
//}
