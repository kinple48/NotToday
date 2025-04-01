// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Item/DropMoneyItem.h"

#include "MainGameModeBase.h"
#include "MainPlayer.h"
#include "CSW/Subsystem/SoundManagerSubsystem.h"

class USoundManagerSubsystem;

ADropMoneyItem::ADropMoneyItem() 
{
    ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/CSW/ItemAssets/Money/SM_Money.SM_Money'"));
    if (TempMesh.Succeeded())
    {
        MeshComp->SetStaticMesh(TempMesh.Object);
    }
    MeshComp->SetRelativeLocation(FVector(-396, 149, -75));
    MeshComp->SetWorldScale3D(FVector(1.5f));
}

void ADropMoneyItem::Apply(AMainPlayer* Player)
{
    Player->CashData += 300.f;
    auto GameMode = GetWorld()->GetAuthGameMode();
    if (GameMode)
    {
        auto MainGameMode = Cast<AMainGameModeBase>(GameMode);
        if (MainGameMode)
        {
            MainGameMode->PrintCash();
        }
    }
    //SFX
    // SFX
    auto SM = GetGameInstance()->GetSubsystem<USoundManagerSubsystem>();
    if (SM)
    {
        SM->PlaySoundAtLocation(ESoundType::MoneyPickUp, GetActorLocation());
    }
    Destroy();
}
