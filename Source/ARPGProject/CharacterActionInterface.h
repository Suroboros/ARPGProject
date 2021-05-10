// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterActionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UCharacterActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARPGPROJECT_API ICharacterActionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintImplementableEvent, Category = CharacterAction)
		void Squat(bool enable);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = CharacterAction)
		void SetCombatMove(bool bWeaponDrawn);

	/*UFUNCTION(Category = CharacterAction)
		void RunOn(bool enable);

	UFUNCTION(Category = CharacterAction)
		void WalkOn(bool enable);

	UFUNCTION(Category = CharacterAction)
		void SquatOn(bool enable);*/

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = CharacterAction)
		void MovementEnable(bool enable);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = CharacterAction)
		void AttackSwing(float strength);
};
