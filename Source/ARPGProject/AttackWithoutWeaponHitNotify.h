// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AttackWithoutWeaponHitNotify.generated.h"

/**
 * 
 */
UCLASS()
class ARPGPROJECT_API UAttackWithoutWeaponHitNotify : public UAnimNotifyState
{
	GENERATED_BODY()
	
private:
	TArray<FHitResult> HitResults;
	TArray<AActor*> HitActors;
	TArray<AActor*> ActorsToIgnore;

public:
	// Socket location in pre frame
	FVector PreLocation;

	class ACharacterBase* Attacker;

	// Override
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
