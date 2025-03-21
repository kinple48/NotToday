// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "MainPlayer.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxcomp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxcomp"));
	boxcomp->SetupAttachment(RootComponent);
	//boxcomp->OnComponentBeginOverlap.AddDynamic(this, &ASpawnPoint::OnPlayerBeginOverlap);
	//boxcomp->OnComponentEndOverlap.AddDynamic(this, &ASpawnPoint::OnPlayerEndOverlap);

	meshcomp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshcomp"));
	meshcomp->SetupAttachment(boxcomp);
	meshcomp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshcomp->SetVisibility(false);

	// StaticMesh ¼³Á¤
	ConstructorHelpers::FObjectFinder<UStaticMesh>Temp_Object(TEXT("/Script/Engine.StaticMesh'/Game/LJW/Asset/Barricade/source/SM_Barricade.SM_Barricade'"));
	if (Temp_Object.Succeeded())
	{
		meshcomp->SetStaticMesh(Temp_Object.Object);
	}

	
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AMainPlayer::StaticClass());
	if (actor)
	{
		player = Cast<AMainPlayer>(actor);
	}
}

// Called every frame
void ASpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void ASpawnPoint::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	FString s = FString::SanitizeFloat(player->state);
//	GEngine->AddOnScreenDebugMessage(0, 0.5f, FColor::Blue, *s);
//	auto player = Cast<AMainPlayer>(OtherActor);
//	//if (player->Spawnpoint == nullptr)
//	//{
//		//player->Spawnpoint = this;
//		player->setspawnpoint_begin();
//		if (state)
//		{
//			meshcomp->SetVisibility(true);
//			meshcomp->SetWorldLocation(this->GetActorLocation() - FVector(0.f, 0.f, 70.f));
//			meshcomp->SetWorldRotation(FRotator(0.f, 90.f, 0.f));
//		}
//	//}
//}
//
//void ASpawnPoint::OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	player->setspawnpoint_end();
//	meshcomp->SetVisibility(false);
//}