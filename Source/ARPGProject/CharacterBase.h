// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterActionInterface.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "CharacterBase.generated.h"

DECLARE_DELEGATE(FunctionDelegate);

UENUM(BlueprintType)
enum class CharacterType : uint8 {
	Player = 0  UMETA(DisplayName = "PLAYER"),
	Zombie = 1  UMETA(DisplayName = "ZOMBIE"),
};

enum class AttackType : uint8 {
	Light = 0,
	Heavy = 1,
};

UCLASS()
class ARPGPROJECT_API ACharacterBase : public ACharacter, public ICharacterActionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	// Movement
	void MoveForward(float Value);
	void MoveRight(float Value);

	// Dash
	void Dash();
	void CancelDash();

	// Walk/Run
	void ChangeWalkRun();

	// Sneak
	void Squat();

	// Look around and look up/down
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// Combat
	void DrawWeapon();
	UFUNCTION(BlueprintCallable, Category = Combat)
	void LightAttack();
	void HeavyAttack();
	void Block();
	void CancelBlock();
	void LockOn();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Character type
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		CharacterType CharaType;

	// Weapon
	USkeletalMeshComponent* Weapon;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float MaxHP;

	float hp;

	UPROPERTY(BlueprintReadOnly, Category = Character)
	bool bDead;

	UPROPERTY(EditAnywhere, Category = Character)
		float Atk;

	// Speed
	UPROPERTY(EditAnywhere, Category = Speed)
		float DefaultWalkSpeed = 300.0f;

	// Search target in this range
	UPROPERTY(EditAnywhere, Category = Speed)
		float TargetSearchRange = 750.0f;

	// Base turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	// Base look up/down rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	// Dash flag
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		bool Dashing;

	// Walk Flag
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		bool Walking;

	// Squat Flag
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		bool Squating;

	// WeaponEquiped Flag
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Character)
		bool bWeaponDrawn;

	// LockOn Flag
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		bool bLockOn;

	// Had seen character Flag
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		bool bSeen;

	// Combo
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat)
		int lightAttackCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat)
		int heavyAttackCount;
	UPROPERTY(EditAnywhere, Category = Combat)
		float LightAttackComboWaitTime;
	UPROPERTY(EditAnywhere, Category = Combat)
		float LightAttackComboBonusRate;
	UPROPERTY(EditAnywhere, Category = Combat)
		float HeavyAttackComboBonusRate;
	float CalculateDamage();

	// Block
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat)
		bool bBlocking;

	// AI
	UPROPERTY(EditAnywhere, Category = Component)
		UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSrc;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Event
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Event)
		void DrawWeaponEvent(bool bDraw);
	UFUNCTION(BlueprintImplementableEvent, Category = Event)
		void LightAttackEvent(bool bWeaponDraw, int comboCount);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Event)
		void HeavyAttackEvent(bool bWeaponDraw, int comboCount);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Event)
		void BlockingEvent(bool bBlock);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Event)
		void HittedEvent(bool bBlock);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Event)
		void DeadEvent();
	UFUNCTION(BlueprintImplementableEvent, Category = Event)
		void UpdateHPUIEvent(float hpPercent);

	// Interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = CharacterAction)
		void MovementEnable(bool enable);
	virtual void MovementEnable_Implementation(bool enable) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = CharacterAction)
		void AttackSwing(float strength);
	virtual void AttackSwing_Implementation(float strength) override;

	// Combat
	void Hitted(float damage);

	UFUNCTION(BlueprintCallable, Category = CharacterAction)
		void LockOn(AActor* actor);

	// Utility
	UFUNCTION(BlueprintCallable, Category = CharacterUtility)
		bool IsEnemy();

	UFUNCTION(BlueprintCallable, Category = CharacterUtility)
		void SetSeen(bool seen);

	UFUNCTION(BlueprintCallable, Category = CharacterUtility)
		bool GetSeen();

private:
	bool bEnableMovement = true;
	void WalkOn();
	void DashOn(bool enable);
	void SquatOn(bool enable);

	// Target
	AActor* target;

	FTimerHandle comboResetTimerHandle;
	void ResetComboCounter();

	// Search target in range;
	bool SearchTarget();
	// Look at target
	void LookAtTarget(float deltaTime);

	AttackType attackType;

	/*void UpdateStatus();*/
};
