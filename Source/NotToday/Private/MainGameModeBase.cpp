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

void AMainGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<AMainGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
	auto gs = Cast<AMainGameStateBase>( GetWorld()->GetGameState() );
	if (!gs)
	{
		return;
	}
	gs->OnDayStarted.AddDynamic( this , &AMainGameModeBase::ChangeNtoD );
	gs->OnNightStarted.AddDynamic( this , &AMainGameModeBase::ChangeDtoN );
	gs->OnGameClear.AddDynamic( this , &AMainGameModeBase::EndGame );
	player = Cast<AMainPlayer>( GetWorld()->GetFirstPlayerController()->GetPawn() );
	//ChangeDtoN();
	Day_UI = CreateWidget<UDay_UI>( GetWorld() , DayWidget );
	Night_UI = CreateWidget<UNight_UI>( GetWorld() , NightWidget );
	player->HP = player->HPMax;
	player->Reload = player->ReloadMax;
	SetHP( player->HP , player->HPMax );
	SetReload( player->Reload , player->ReloadMax );
	PrintCash();
	Night_UI->AddToViewport();
}

void AMainGameModeBase::PrintStore()
{
	if (Day_UI && Day_UI->IsValidLowLevel())
	{
		if (player)
		{
			Day_UI->StoreDataText->SetText( FText::AsNumber( player->StoreData ) );
		}
	}
}

void AMainGameModeBase::PrintCash()
{
	if (Day_UI && Day_UI->IsValidLowLevel())
	{
		if (player)
		{
			if (player->CashData == 0) Day_UI->CashDataText->SetText( FText::FromString( TEXT( "0" ) ) );

			else Day_UI->CashDataText->SetText( FText::AsNumber( player->CashData ) );
		}
	}

	if (Night_UI && Night_UI->IsValidLowLevel())
	{
		if (player)
		{
			if (player->CashData == 0) Night_UI->CashDataText->SetText( FText::FromString( TEXT( "0") ) );

			else Night_UI->CashDataText->SetText( FText::AsNumber( player->CashData ) );
		}
	}
}

void AMainGameModeBase::PrintScore()
{
	if (Night_UI && Night_UI->IsValidLowLevel())
	{
		if (player)
		{
			Night_UI->ScoreDataText->SetText( FText::AsNumber( player->ScoreData ) );
		}
	}
}

void AMainGameModeBase::PrintPrice()
{
	if (Day_UI)
	{
		if (player)
		{
			Day_UI->PriceDataText->SetText( FText::AsNumber( player->Price ) );
		}
	}
}

void AMainGameModeBase::PrintPlace()
{
	if (Day_UI && Day_UI->IsValidLowLevel())
	{
		Day_UI->PlaceText->SetText( FText::FromString( TEXT( "PLACE" ) ) );
	}
}

void AMainGameModeBase::PrintRemove()
{
	if (Day_UI && Day_UI->IsValidLowLevel())
	{
		if (player->CombatState) return;
		Day_UI->PlaceText->SetText( FText::FromString( TEXT( "REMOVE" ) ) );
	}
}

void AMainGameModeBase::PrintElse()
{
	if (Day_UI && Day_UI->IsValidLowLevel())
	{
		Day_UI->PlaceText->SetText( FText::FromString( TEXT( "" ) ) );
	}
}

void AMainGameModeBase::ChangeDtoN()
{
	if (NightWidget)
	{	
		GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "DtoN" ) );
		
		if (Day_UI)
		{
			Day_UI->RemoveFromParent();
		}
		Night_UI = CreateWidget<UNight_UI>( GetWorld() , NightWidget );
		if (Night_UI)
		{
			Night_UI->AddToViewport();
		}

		player->HP = player->HPMax;
		player->Reload = player->ReloadMax;
		SetHP( player->HP , player->HPMax );
		SetReload( player->Reload , player->ReloadMax );
		PrintCash();
		PrintScore();
	}
}

void AMainGameModeBase::ChangeNtoD()
{
	if (DayWidget)
	{
		GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "NtoD" ) );
		if (Night_UI)
		{
			Night_UI->RemoveFromParent();
		}
		Day_UI = CreateWidget<UDay_UI>( GetWorld() , DayWidget );
		if (Day_UI)
		{
			Day_UI->AddToViewport();
		}
		//player->Price = player->BarricadePrice;
		//player->StoreData = player->BarricadeStoreData;
		PrintPrice();
		PrintStore();
		PrintCash();

		auto* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			pc->SetShowMouseCursor( true );
			pc->SetInputMode( FInputModeGameAndUI() );
		}
	}
}

void AMainGameModeBase::EndGame()
{
	UKismetSystemLibrary::QuitGame( GetWorld() , GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit , false );
}

void AMainGameModeBase::SetHP( float Cur , float Max )
{
	if (!Night_UI) return;
	Night_UI->SetHP( Cur , Max );
}

void AMainGameModeBase::SetReload( float Cur , float Max )
{
	if (!Night_UI) return;
	Night_UI->SetReload( Cur , Max );
}
