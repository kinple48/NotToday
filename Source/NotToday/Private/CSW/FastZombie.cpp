// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/FastZombie.h"

#include "Components/BoxComponent.h"

AFastZombie::AFastZombie()
{
	MeleeHitBox->SetupAttachment(GetMesh(), FName(TEXT("RightHand")));
	MeleeHitBox->SetRelativeLocation(FVector(0.973401f,-10.733109f,-6.358037f));
}
