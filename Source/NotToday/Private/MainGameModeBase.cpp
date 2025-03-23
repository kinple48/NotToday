// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameModeBase.h"
#include "Day_UI.h"
#include "Components/TextBlock.h"

void AMainGameModeBase::BeginPlay()
{
	Super::BeginPlay();
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
		Day_UI->StoreDataText->SetText( FText::AsNumber( CurrentScore ) );
	}
}

void AMainGameModeBase::PrintCash()
{
	if (Day_UI)
	{
		Day_UI->CashDataText->SetText( FText::AsNumber( CurrentScore ) );
	}
}

void AMainGameModeBase::PrintPrice()
{
	if (Day_UI)
	{
		Day_UI->PriceDataText->SetText( FText::AsNumber( CurrentScore ) );
	}
}
