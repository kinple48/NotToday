// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ZombieAnim.generated.h"

enum class EZombieState : uint8;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class NOTTODAY_API UZombieAnim : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  Category = FSM)
	float GroundSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	EZombieState State;

	UPROPERTY( EditDefaultsOnly , BlueprintReadWrite , Category = FSM )
	TObjectPtr<UAnimMontage> ZombieMontage;

	UFUNCTION()
	void AnimNotify_AttackEnd();
};
