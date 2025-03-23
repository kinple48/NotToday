// Fill out your copyright notice in the Description page of Project Settings.


#include "Day_UI.h"
#include "Components/Button.h"
#include "MainPlayer.h"
#include "MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnPoint.h"
#include "Barricade.h"

void UDay_UI::NativeConstruct()
{
	Super::NativeConstruct();
	player = Cast<AMainPlayer>( GetWorld()->GetFirstPlayerController()->GetPawn() );
	GameMode = Cast<AMainGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
	Button_Buy->OnClicked.AddDynamic( this , &UDay_UI::Buy );
	Button_Place->OnClicked.AddDynamic( this , &UDay_UI::Place );
	Button_NextLevel->OnClicked.AddDynamic( this , &UDay_UI::NextLevel );
}

void UDay_UI::Place()
{
	//player = Cast<AMainPlayer>( GetWorld()->GetFirstPlayerController()->GetPawn() );
	if (player)
	{
		if (player->Spawnpoint && !player->Spawnpoint->spawnstate )
		{
			player->SpawnObject();
			player->Spawnpoint->meshcomp->SetVisibility( false );
		}
		else if (player->Spawnpoint && player->Spawnpoint->spawnstate)
		{
			if (player->BarricadeObject)
			{
				player->BarricadeObject->Destroy();
				player->BarricadeStoreData += 1;
				player->CashData += player->BarricadePrice;
				GameMode->PrintStore();
				GameMode->PrintCash();
				GameMode->PrintPlace();
				player->Spawnpoint->spawnstate = false;
				player->Spawnpoint->meshcomp->SetVisibility( true );
			}
		}
	}
}

void UDay_UI::Buy()
{
	if (player)
	{
		player->BarricadeStoreData += 1;
		if (player->Spawnpoint)
		{
			player->OverlapEvent( player->Spawnpoint , player );
		}

		if (GameMode)
		{
			GameMode->PrintPlace();
			GameMode->PrintStore();
		}
	}
}

void UDay_UI::NextLevel()
{
	GameMode->ChangeUI();
	player->CombatState = true;
	
}
