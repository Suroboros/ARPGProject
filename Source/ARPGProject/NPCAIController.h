// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "CharacterBase.h"
#include "NPCAIController.generated.h"

/**
 * 
 */
UCLASS()
class ARPGPROJECT_API ANPCAIController : public AAIController
{
	GENERATED_BODY()

public:
	ANPCAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void BeginPlay() override;
	//void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	UNavigationSystemV1* NavMesh=nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AI Config")
		class UAISenseConfig_Sight* AISenseSight;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Parameter)
		ACharacterBase* ControlledCharacter;

	UPROPERTY(EditAnywhere, Category = Parameter)
		float ReachablePointRadius = 800.0f;
	UPROPERTY(EditAnywhere, Category = Parameter)
		float ReachableRadius = 300.0f;

	UPROPERTY(EditAnywhere, Category = Parameter)
		UBehaviorTree* BT = nullptr;

	UPROPERTY(EditAnywhere, Category = Component)
		UAIPerceptionComponent* AIPerception;

	// Move
	void MoveToTarget(const APawn* target);
	void MoveToDestination(const FVector destination);
};