// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AttackHitNotify.generated.h"

/**
 * 
 */
UCLASS()
class ARPGPROJECT_API UAttackHitNotify : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	TArray<FHitResult> HitResults;
	TArray<AActor*> HitActors;
	TArray<AActor*> ActorsToIgnore;

public:
	// Socket location in pre frame
	FVector PreLocation1;
	FVector PreLocation2;

	class ACharacterBase* Attacker;
	class USkeletalMeshComponent* Weapon;
	
	// Override
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
