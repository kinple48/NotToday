// Fill out your copyright notice in the Description page of Project Settings.


#include "Night_UI.h"
#include "MainPlayer.h"
#include "MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"

void UNight_UI::NativeConstruct()
{
	player = Cast<AMainPlayer>( GetWorld()->GetFirstPlayerController()->GetPawn() );
	GameMode = Cast<AMainGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
}

void UNight_UI::SetHP( float Cur , float Max )
{
	player->NightUIInstance->HPBar->SetPercent( Cur / Max );
}

void UNight_UI::SetReload( float Cur , float Max )
{
	
	player->NightUIInstance->ReloadBar->SetPercent( Cur / Max );
}

