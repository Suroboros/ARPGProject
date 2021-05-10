// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCAIController.h"
#include "NavigationSystem.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "CharacterBase.h"

ANPCAIController::ANPCAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	AISenseSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISight"));
	if (AIPerception)
	{
		// Sight config

		AISenseSight->SightRadius = 3000.0f;
		AISenseSight->LoseSightRadius = 3500.0f;
		AISenseSight->DetectionByAffiliation.bDetectEnemies = true;
		AISenseSight->DetectionByAffiliation.bDetectNeutrals = true;
		AISenseSight->DetectionByAffiliation.bDetectFriendlies = true;

		AIPerception->ConfigureSense(*AISenseSight);
	}
}

void ANPCAIController::BeginPlay()
{
	Super::BeginPlay();
	/*NavMesh = UNavigationSystemV1::GetCurrent(this);

	MoveToTarget(GetWorld()->GetFirstPlayerController()->GetPawn());*/

	ControlledCharacter = Cast<ACharacterBase>(GetPawn());


	RunBehaviorTree(BT);

}

//void ANPCAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
//{
//	Super::OnMoveCompleted(RequestID, Result);
//
//	MoveToTarget(GetWorld()->GetFirstPlayerController()->GetPawn());
//}

void ANPCAIController::MoveToTarget(const APawn* target)
{
	if (NavMesh)
	{
		FVector targetLocation = target->GetActorLocation();
		FNavLocation randomPoint;
		NavMesh->GetRandomReachablePointInRadius(targetLocation, ReachablePointRadius, randomPoint);

		MoveToDestination(randomPoint);
	}
}

void ANPCAIController::MoveToDestination(const FVector destination)
{
	FNavLocation randomPoint;
	EPathFollowingRequestResult::Type moveResult = EPathFollowingRequestResult::Type::Failed;
	while (moveResult == EPathFollowingRequestResult::Type::Failed)
	{
		NavMesh->GetRandomReachablePointInRadius(destination, ReachableRadius, randomPoint);

		moveResult = MoveToLocation(randomPoint, ReachableRadius,false);
	}
}
