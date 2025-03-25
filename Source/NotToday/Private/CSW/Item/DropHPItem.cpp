// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Item/DropHPItem.h"

#include "MainPlayer.h"

ADropHPItem::ADropHPItem()
{
    ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/CSW/ItemAssets/Heart/Heart.Heart'"));
    if (TempMesh.Succeeded())
    {
        MeshComp->SetStaticMesh(TempMesh.Object);
    }
    MeshComp->SetRelativeLocation(FVector(0,0,-43));
}

void ADropHPItem::Apply(AMainPlayer* Player)
{
    Player->HP = FMath::Min(Player->HP + 30, Player->HPMax);
    Super::Apply(Player);
}
