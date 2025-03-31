// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameModeBase.h"
#include "Day_UI.h"
#include "Components/TextBlock.h"
#include "MainPlayer.h"
#include "Night_UI.h"
#include "MainGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "DefenseTower.h"
#include "Barricade.h"
#include "SpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

void AMainGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<AMainGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
	auto gs = Cast<AMainGameStateBase>( GetWorld()->GetGameState() );
	if (!gs)
	{
		return;
	}
	gs->OnGameClear.AddDynamic( this , &AMainGameModeBase::EndGame );
	player = Cast<AMainPlayer>( GetWorld()->GetFirstPlayerController()->GetPawn() );
}

void AMainGameModeBase::PrintStore()
{
	if (player)
	{
		DayUIInstance = Cast<UDay_UI>( player->WidgetComp->GetUserWidgetObject() );
		if (DayUIInstance)
		{
			DayUIInstance->StoreDataText->SetText( FText::AsNumber( player->StoreData ) );
		}
	}
}

void AMainGameModeBase::PrintCash()
{
	
	if (player && !player->CombatState)
	{
		if (player->DayUIInstance)
		{
			player->DayUIInstance->CashDataText->SetText( FText::AsNumber( player->CashData ) );
		}
	}

	
	if (player && player->CombatState)
	{
		if (player->NightUIInstance)
		{
			player->NightUIInstance->CashDataText->SetText( FText::AsNumber( player->CashData ) );
		}
	}
}

void AMainGameModeBase::PrintScore()
{
	if (player)
	{
		if (player->NightUIInstance)
		{
			player->NightUIInstance->ScoreDataText->SetText( FText::AsNumber( player->ScoreData ) );
		}	
	}
}

void AMainGameModeBase::PrintPrice()
{
	if (player)
	{
		if (player->DayUIInstance)
		{
			player->DayUIInstance->PriceDataText->SetText( FText::AsNumber( player->Price ) );
		}
	}
}

void AMainGameModeBase::PrintPlace()
{
	/*if (player)
	{
		if (player->DayUIInstance)
		{
			player->DayUIInstance->PlaceText->SetText( FText::FromString( TEXT( "PLACE" ) ) );
		}
	}*/
}

void AMainGameModeBase::PrintRemove()
{
	/*if (player)
	{
		if (player->DayUIInstance)
		{
			player->DayUIInstance->PlaceText->SetText( FText::FromString( TEXT( "REMOVE" ) ) );
		}
	}*/
}

void AMainGameModeBase::PrintElse()
{
	/*if (player)
	{
		if (player->DayUIInstance)
		{
			player->DayUIInstance->PlaceText->SetText( FText::FromString( TEXT( "" ) ) );
		}
	}*/
}

void AMainGameModeBase::EndGame()
{
	UKismetSystemLibrary::QuitGame( GetWorld() , GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit , false );
}

void AMainGameModeBase::SetHP( float Cur , float Max )
{
	if (player->NightUIInstance)
	{
		player->NightUIInstance->SetHP( Cur , Max );
	}
}

void AMainGameModeBase::SetReload( float Cur , float Max )
{
	if (player->NightUIInstance)
	{
		player->NightUIInstance->SetReload(Cur , Max);
	}
}
