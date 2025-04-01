// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Item/DropHPItem.h"

#include "MainGameModeBase.h"
#include "MainPlayer.h"
#include "Night_UI.h"
#include "CSW/Subsystem/SoundManagerSubsystem.h"
#include "NotToday/NotToday.h"

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
    auto GameMode = GetWorld()->GetAuthGameMode();
    if (GameMode)
    {
        auto MainGameMode = Cast<AMainGameModeBase>(GameMode);
        if (MainGameMode)
        {
            //MainGameMode->Night_UI->SetHP(Player->HP, Player->HPMax);
            Player->NightUIInstance->SetHP( Player->HP , Player->HPMax );
        }
    }
    // SFX
    auto SM = GetGameInstance()->GetSubsystem<USoundManagerSubsystem>();
    if (SM)
    {
        SM->PlaySoundAtLocation(ESoundType::HealthPickUp, GetActorLocation());
    }

    
    Super::Apply(Player);
}
