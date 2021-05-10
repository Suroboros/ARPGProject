// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHitNotify.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CharacterBase.h"

void UAttackHitNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Attacker = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (Attacker)
	{
		ActorsToIgnore.Add(MeshComp->GetOwner());
		if (Attacker->ActorHasTag("Player"))
		{
			Weapon = Attacker->Weapon;
			if (Weapon)
			{
				PreLocation1 = Weapon->GetSocketLocation("HitCheckTracer1");
				PreLocation2 = Weapon->GetSocketLocation("HitCheckTracer2");
			}
		}
	}
}

void UAttackHitNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (Attacker && Weapon)
	{
		FVector currentLocation1 = Weapon->GetSocketLocation("HitCheckTracer1");
		FVector currentLocation2 = Weapon->GetSocketLocation("HitCheckTracer2");
		// Hit check
		// First socket
		UKismetSystemLibrary::BoxTraceMulti(Attacker->GetWorld(), PreLocation1, currentLocation1, FVector(5, 3, 30), Weapon->GetComponentRotation(),
			ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);
		for (int i = 0; i < HitResults.Num(); i++)
		{
			AActor* hitActor = HitResults[i].GetActor();
			// Do hit process if this attackee is not in HitActors array
			if (!HitActors.Contains(hitActor))
			{
				HitActors.Add(hitActor);
				ACharacterBase* attackee = Cast<ACharacterBase>(hitActor);
				if (attackee)
				{
					attackee->Hitted(Attacker->CalculateDamage());
				}
			}
		}

		// Second socket
		UKismetSystemLibrary::BoxTraceMulti(Attacker->GetWorld(), PreLocation2, currentLocation2, FVector(5, 3, 30), Weapon->GetComponentRotation(),
			ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);
		for (int i = 0; i < HitResults.Num(); i++)
		{
			AActor* hitActor = HitResults[i].GetActor();
			// Do hit process if this attackee is not in HitActors array
			if (!HitActors.Contains(hitActor))
			{
				HitActors.Add(hitActor);
				ACharacterBase* attackee = Cast<ACharacterBase>(hitActor);
				if (attackee)
				{
					attackee->Hitted(Attacker->CalculateDamage());
				}
			}
		}

		// Restore pre socket location
		PreLocation1 = currentLocation1;
		PreLocation2 = currentLocation2;
	}
}

void UAttackHitNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	HitActors.Empty();
	ActorsToIgnore.Empty();
}
