// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/NormalZombie.h"

#include "Components/BoxComponent.h"


ANormalZombie::ANormalZombie()
{
	MeleeHitBox->SetupAttachment(GetMesh(), FName(TEXT("hand_l")));
	MeleeHitBox->SetRelativeLocation(FVector(9.124971f,-0.145558f,-0.f));
}
