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

void UDay_UI::NativeConstruct()
{
	Super::NativeConstruct();
	player = Cast<AMainPlayer>( GetWorld()->GetFirstPlayerController()->GetPawn() );
	GameMode = Cast<AMainGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
	Button_Buy->OnClicked.AddDynamic( this , &UDay_UI::Buy );
	Button_Place->OnClicked.AddDynamic( this , &UDay_UI::Place );
	Button_NextLevel->OnClicked.AddDynamic( this , &UDay_UI::NextLevel );
	Button_WoodenBarricade->OnClicked.AddDynamic( this , &UDay_UI::WoodenBarricade );
	Button_AutoTurret->OnClicked.AddDynamic( this , &UDay_UI::AutoTurret );
}

void UDay_UI::Place()
{
	//player = Cast<AMainPlayer>( GetWorld()->GetFirstPlayerController()->GetPawn() );
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
			else if(!ObjectType && player->BarricadeStoreData > 0)
			{
				player->AutoTurretStoreData -= 1;
				player->StoreData = player->AutoTurretStoreData;
			}
			GameMode->PrintStore();
			player->Spawnpoint->meshcomp->SetVisibility( false );
		}
		else if (player->Spawnpoint && player->Spawnpoint->spawnstate)
		{
			/*if (player->BarricadeObject)
			{
				player->BarricadeObject->Destroy();
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
				
				player->CashData += player->Price;
				GameMode->PrintStore();
				GameMode->PrintCash();
				GameMode->PrintPlace();
				player->Spawnpoint->spawnstate = false;
				player->Spawnpoint->meshcomp->SetVisibility( true );
			}*/

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
	if (player)
	{
		//player->StoreData += 1;
		if (ObjectType)
		{
			GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "b buy" ) );
			player->BarricadeStoreData += 1;
			player->StoreData = player->BarricadeStoreData;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "t buy" ) );
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
	GameMode->ChangeUI();
	player->CombatState = true;
	
	TArray<AActor*> ActorsWithTag;
	FName TagToSearch = TEXT( "Object" ); // 검색할 태그 이름

	// 액터 검색
	UGameplayStatics::GetAllActorsWithTag( GetWorld() , TagToSearch , ActorsWithTag );

	// 결과 출력
	for (AActor* Actor : ActorsWithTag)
	{
		if (Actor)
		{
			auto Object2 = Cast<ADefenseTower>( Actor );
			if (Object2)
			{
				Object2->meshcomp->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );
			}
			auto Object1 = Cast<ABarricade>( Actor );
			if (Object1)
			{
				Object1->meshcomp->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );
			}

			
		}
	}

	TArray<AActor*> ActorWithTag;
	TagToSearch = TEXT( "SpawnPoint" ); // 검색할 태그 이름

	// 액터 검색
	UGameplayStatics::GetAllActorsWithTag( GetWorld() , TagToSearch , ActorWithTag );

	// 결과 출력
	for (AActor* Actor : ActorWithTag)
	{
		if (Actor)
		{
			auto Object = Cast<ASpawnPoint>( Actor );
			if (Object)
			{
				Object->boxcomp->SetCollisionEnabled( ECollisionEnabled::NoCollision);
			}
		}
	}

	if (GameMode)
	{
		GameMode->SetHP( player->HP , player->HPMax );
		GameMode->SetReload( player->Reload , player->ReloadMax );
		GameMode->PrintCash();
		GameMode->PrintScore();
	}

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

void UDay_UI::WoodenBarricade()
{
	ObjectType = true;
	UStaticMesh* Mesh = LoadObject<UStaticMesh>( nullptr , TEXT( "/Script/Engine.StaticMesh'/Game/LJW/Asset/Barricade/source/SM_Barricade.SM_Barricade'" ) );
	player->StoreData = player->BarricadeStoreData;
	player->Price = player->BarricadePrice;
	GameMode->PrintStore();
	GameMode->PrintPrice();

	TArray<AActor*> FoundActors;
	FName TagToSearch = TEXT( "SpawnPoint" ); // 검색할 태그 이름

	// 액터 검색
	UGameplayStatics::GetAllActorsWithTag( GetWorld() , TagToSearch , FoundActors );
	for (AActor* Actors : FoundActors)
	{
		auto barricade = Cast<ASpawnPoint>( Actors );
		if (barricade)
		{
			barricade->meshcomp->SetStaticMesh( Mesh );

			player->SpawnLocation = FVector( 0.f , 0.f , 70.f );
			player->SpawnRotation = FRotator( 0.f , 90.f , 0.f );
			player->SpawnScale = FVector( 0.5 , 0.3 , 0.5 );

			if (player->Spawnpoint)
			{
				player->Spawnpoint->meshcomp->SetWorldLocation( player->Spawnpoint->GetActorLocation() - player->SpawnLocation );
				player->Spawnpoint->meshcomp->SetWorldRotation( player->SpawnRotation );
				player->Spawnpoint->meshcomp->SetRelativeScale3D( player->SpawnScale );
			}
		}
	}
}

void UDay_UI::AutoTurret()
{
	ObjectType = false;
	GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "AutoTurret" ) );
	UStaticMesh* Mesh = LoadObject<UStaticMesh>( nullptr , TEXT( "/Script/Engine.StaticMesh'/Game/LJW/Asset/defence-tower/source/StaticMesh.StaticMesh'" ) );
	
	player->StoreData = player->AutoTurretStoreData;
	player->Price = player->AutoTurretPrice;
	GameMode->PrintStore();
	GameMode->PrintPrice();

	TArray<AActor*> FoundActors;
	FName TagToSearch = TEXT( "SpawnPoint" ); // 검색할 태그 이름

	// 액터 검색
	UGameplayStatics::GetAllActorsWithTag( GetWorld() , TagToSearch , FoundActors );
	for (AActor* Actors : FoundActors)
	{
		auto defensetower = Cast<ASpawnPoint>( Actors );
		if (defensetower)
		{
			defensetower->meshcomp->SetStaticMesh( Mesh );

			player->SpawnLocation = FVector( 0.f , 0.f , 30.f );
			player->SpawnRotation = FRotator( 0.f , 90.f , 0.f );
			player->SpawnScale = FVector( 0.3 , 0.2 , 0.3 );

			if (player->Spawnpoint)
			{
				player->Spawnpoint->meshcomp->SetWorldLocation( player->Spawnpoint->GetActorLocation() - player->SpawnLocation );
				player->Spawnpoint->meshcomp->SetWorldRotation( player->SpawnRotation );
				player->Spawnpoint->meshcomp->SetRelativeScale3D( player->SpawnScale );
			}
		}
	}
}
