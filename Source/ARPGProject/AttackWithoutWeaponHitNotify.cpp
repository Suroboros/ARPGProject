// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackWithoutWeaponHitNotify.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterBase.h"

void UAttackWithoutWeaponHitNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Attacker = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (Attacker)
	{
		// Add self to ignore
		ActorsToIgnore.Add(MeshComp->GetOwner());
		// Add same type characters to ignore
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClass(Attacker->GetWorld(), ACharacterBase::StaticClass(), foundActors);
		for (int i = 0; i < foundActors.Num(); i++)
		{
			ACharacterBase* character = Cast<ACharacterBase>(foundActors[i]);
			if (character->CharaType == Attacker->CharaType)
			{
				ActorsToIgnore.Add(foundActors[i]);
			}
		}

		PreLocation = MeshComp->GetSocketLocation("HitCheckTracer");
	}
}

void UAttackWithoutWeaponHitNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (Attacker)
	{
		FVector currentLocation = MeshComp->GetSocketLocation("HitCheckTracer");
		// Hit check
		UKismetSystemLibrary::BoxTraceMulti(Attacker->GetWorld(), PreLocation, currentLocation, FVector(5, 8, 2), MeshComp->GetComponentRotation(),
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
					attackee->Hitted(30.0f);
				}
			}
		}

		// Restore pre socket location
		PreLocation = currentLocation;
	}
}

void UAttackWithoutWeaponHitNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	HitActors.Empty();
	ActorsToIgnore.Empty();
}

