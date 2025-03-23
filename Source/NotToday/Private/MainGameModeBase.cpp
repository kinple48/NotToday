// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameModeBase.h"
#include "Day_UI.h"
#include "Components/TextBlock.h"
#include "MainPlayer.h"
#include "Night_UI.h"

void AMainGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	player = Cast<AMainPlayer>( GetWorld()->GetFirstPlayerController()->GetPawn() );

	if (DayWidget)
	{
		Day_UI = CreateWidget<UDay_UI>( GetWorld() , DayWidget );
		if (Day_UI)
		{
			Day_UI->AddToViewport();
			PrintStore();
			PrintCash();
			PrintPrice();

			auto* pc = GetWorld()->GetFirstPlayerController();
			if (pc)
			{
				pc->SetShowMouseCursor( true );
				pc->SetInputMode( FInputModeGameAndUI() );
			}
		}

	}
}

void AMainGameModeBase::PrintStore()
{
	if (Day_UI)
	{
		if (player)
		{
			Day_UI->StoreDataText->SetText( FText::AsNumber( player->BarricadeStoreData ) );
		}
	}
}

void AMainGameModeBase::PrintCash()
{
	if (Day_UI)
	{
		if (player)
		{
			Day_UI->CashDataText->SetText( FText::AsNumber(player->CashData));
		}
	}
	if (Night_UI)
	{
		if (player)
		{
			Night_UI->CashDataText->SetText( FText::AsNumber( player->CashData ) );
		}
	}
}

void AMainGameModeBase::PrintScore()
{
	if (Night_UI)
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

			Day_UI->PriceDataText->SetText( FText::AsNumber( player->BarricadePrice ) );
		}
	}
}

void AMainGameModeBase::PrintPlace()
{
	if (Day_UI)
	{
		Day_UI->PlaceText->SetText( FText::FromString( TEXT( "PLACE" ) ) );
	}
}

void AMainGameModeBase::PrintRemove()
{
	if (Day_UI)
	{
		Day_UI->PlaceText->SetText( FText::FromString( TEXT( "REMOVE" ) ) );
	}
}

void AMainGameModeBase::PrintElse()
{
	if (Day_UI)
	{
		Day_UI->PlaceText->SetText( FText::FromString( TEXT( "" ) ) );
	}
}

void AMainGameModeBase::ChangeUI()
{
	if (NightWidget)
	{
		Day_UI->RemoveFromParent();
		Night_UI = CreateWidget<UNight_UI>( GetWorld() , NightWidget );
		if (Night_UI)
		{
			Night_UI->AddToViewport();
		}
	}
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
