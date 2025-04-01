// Fill out your copyright notice in the Description page of Project Settings.


#include "Day_UI.h"
#include "Components/Button.h"
#include "MainPlayer.h"
#include "MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnPoint.h"
#include "Barricade.h"
#include "Components/BoxComponent.h"
#include "DefenseTower.h"
#include "MainGameStateBase.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TextBlock.h"
#include "Components/PanelSlot.h"

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
	if (player)
	{
		if (player->Spawnpoint && !player->Spawnpoint->spawnstate )
		{
			player->SpawnObject();
			if (ObjectType && player->BarricadeStoreData > 0)
			{
				player->BarricadeStoreData -= 1;
				player->StoreData = player->BarricadeStoreData;
			}
			else if(!ObjectType && player->AutoTurretStoreData > 0)
			{
				player->AutoTurretStoreData -= 1;
				player->StoreData = player->AutoTurretStoreData;
			}
			GameMode->PrintStore();
			player->Spawnpoint->meshcomp->SetVisibility( false );
		}
		else if (player->Spawnpoint && player->Spawnpoint->spawnstate)
		{
			if (player->OverlapActor)
			{
				if (player->OverlapActor->ActorHasTag( TEXT( "Object" ) ))
				{
					player->OverlapActor->Destroy();
					if (ObjectType)
					{
						player->BarricadeStoreData += 1;
						player->StoreData = player->BarricadeStoreData;
					}
					else
					{
						player->AutoTurretStoreData += 1;
						player->StoreData = player->AutoTurretStoreData;
					}

					GameMode->PrintStore();
					GameMode->PrintPlace();
					player->Spawnpoint->spawnstate = false;
					player->Spawnpoint->meshcomp->SetVisibility( true );
				}
			}
		}
	}
}

void UDay_UI::Buy()
{
	if (player && (player->CashData - player->Price) > 0)
	{
		if (ObjectType)
		{
			player->BarricadeStoreData += 1;
			player->StoreData = player->BarricadeStoreData;
		}
		else
		{
			player->AutoTurretStoreData += 1;
			player->StoreData = player->AutoTurretStoreData;
		}
		
		player->CashData -= player->Price;
		if (player->Spawnpoint)
		{
			player->OverlapEvent( player->Spawnpoint , player );
		}

		if (GameMode)
		{
			GameMode->PrintPlace();
			GameMode->PrintStore();
			GameMode->PrintCash();
		}
	}
}

void UDay_UI::NextLevel()
{

	auto GameState = GetWorld()->GetGameState();
	if (GameState)
	{
		auto MainGameState = Cast<AMainGameStateBase>(GameState);
		if (MainGameState)
		{
			MainGameState->SetDayNightState(EDayNightState::Night);
		}
	}
}

void UDay_UI::AnimateTextBoxPositions( float Duration , bool bMoveLeft )
{
	if (!BarricadeText || !TurretText) return;

	if (GetWorld()->GetTimerManager().IsTimerActive( TimerHandle )) {
		GetWorld()->GetTimerManager().ClearTimer( TimerHandle );
	}

	// 초기 및 목표 위치 정의
	FVector2D BarricadeStart = bMoveLeft ? FVector2D( 0.5f , -0.5f ) : FVector2D( -1.5f , -0.5f );
	FVector2D BarricadeEnd = bMoveLeft ? FVector2D( -1.5f , -0.5f ) : FVector2D( 0.5f , -0.5f );

	FVector2D TurretStart = bMoveLeft ? FVector2D( -1.5f , -0.5f ) : FVector2D( 0.5f , -0.5f );
	FVector2D TurretEnd = bMoveLeft ? FVector2D( 0.5f , -0.5f ) : FVector2D( -1.5f , -0.5f );

	// 애니메이션 타이머 설정
	float TimeElapsed = 0.f;

	GetWorld()->GetTimerManager().SetTimer( TimerHandle , [= , this]() mutable {
		TimeElapsed += GetWorld()->GetDeltaSeconds();

		// Alpha 값 계산
		float Alpha = FMath::Clamp( TimeElapsed / Duration , 0.f , 1.f );
		UE_LOG( LogTemp , Warning , TEXT( "Alpha: %f" ) , Alpha );

		// Easing 함수 적용
		float EasedAlpha = EaseOutQuint( Alpha );
		UE_LOG( LogTemp , Warning , TEXT( "EasedAlpha: %f" ) , EasedAlpha );

		// 텍스트 박스 위치 업데이트
		UpdateTextBoxPosition( BarricadeText , BarricadeStart , BarricadeEnd , EasedAlpha );
		UpdateTextBoxPosition( TurretText , TurretStart , TurretEnd , EasedAlpha );

		// 애니메이션 종료 시 타이머 정지
		if (Alpha >= 1.f) {
			GetWorld()->GetTimerManager().ClearTimer( TimerHandle );
			UE_LOG( LogTemp , Warning , TEXT( "Animation finished!" ) );
		}
} , 0.01f , true );
}

void UDay_UI::UpdateTextBoxPosition( UTextBlock* TextBox , FVector2D StartPosition , FVector2D EndPosition , float Alpha )
{
	if (!TextBox || !TextBox->Slot) {
		UE_LOG( LogTemp , Error , TEXT( "Invalid TextBox or Slot!" ) );
		return;
	}

	UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>( TextBox->Slot );
	if (!slot) {
		UE_LOG( LogTemp , Error , TEXT( "Failed to cast to UCanvasPanelSlot!" ) );
		return;
	}

	FVector2D NewAlignment;
	NewAlignment.X = FMath::Lerp( StartPosition.X , EndPosition.X , Alpha );
	NewAlignment.Y = FMath::Lerp( StartPosition.Y , EndPosition.Y , Alpha );

	// Alignment 업데이트
	slot->SetAlignment( NewAlignment );
	if (Alpha >= 1.f)
	{
		slot->SetAlignment( EndPosition );
	}
	UE_LOG( LogTemp , Warning , TEXT( "Updated Alignment: (%f, %f)" ) , NewAlignment.X , NewAlignment.Y );
}

float UDay_UI::EaseOutQuint( float x )
{
	return 1 - FMath::Pow( 1 - x , 5 );
}
