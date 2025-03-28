// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ZombieAnim.h"

#include "CSW/ZombieBase.h"
#include "CSW/ZombieFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UZombieAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	auto ZombieBase = Cast<AZombieBase>( TryGetPawnOwner());
	if (ZombieBase)
	{
		MovementComponent = ZombieBase->GetCharacterMovement();
	}
}

void UZombieAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (MovementComponent)
	{
		GroundSpeed = MovementComponent->Velocity.Length();
	}
}

void UZombieAnim::AnimNotify_AttackEnd()
{
	auto ZombieBase = Cast<AZombieBase>( TryGetPawnOwner());
	if (ZombieBase)
	{
		ZombieBase->FSM->bAttackPlaying = false;
	}
}
