// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Notify/ZombieAttackNotifyState.h"

#include "Components/BoxComponent.h"
#include "CSW/ZombieBase.h"


void UZombieAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (AZombieBase* Zombie = Cast<AZombieBase>(MeshComp->GetOwner()))
	{
		Zombie->MeleeHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void UZombieAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (AZombieBase* Zombie = Cast<AZombieBase>(MeshComp->GetOwner()))
	{
		Zombie->MeleeHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}}
