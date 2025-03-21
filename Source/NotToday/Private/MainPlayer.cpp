// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnPoint.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(RootComponent);

	// IMC 설정
	ConstructorHelpers::FObjectFinder<UInputMappingContext>TempIMC(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/LJW/Input/IMC_Player.IMC_Player'"));
	if (TempIMC.Succeeded())
	{
		IMC_Player = TempIMC.Object;
	}

	// 플레이어 이동
	ConstructorHelpers::FObjectFinder<UInputAction>TempIA_Move(TEXT("/Script/EnhancedInput.InputAction'/Game/LJW/Input/IA_Move.IA_Move'"));
	if (TempIA_Move.Succeeded())
	{
		IA_Move = TempIA_Move.Object;
	}

	// 마우스 움직임 시점 전환
	ConstructorHelpers::FObjectFinder<UInputAction>TempIA_Mouse(TEXT("/Script/EnhancedInput.InputAction'/Game/LJW/Input/IA_VRMouse.IA_VRMouse'"));
	if (TempIA_Mouse.Succeeded())
	{
		IA_Mouse = TempIA_Mouse.Object;
	}

	// 발사 키 입력
	ConstructorHelpers::FObjectFinder<UInputAction>TempIA_GunShot(TEXT("/Script/EnhancedInput.InputAction'/Game/LJW/Input/IA_GunShot.IA_GunShot'"));
	if (TempIA_GunShot.Succeeded())
	{
		IA_GunShot = TempIA_GunShot.Object;
	}


	// 발사 사운드
	ConstructorHelpers::FObjectFinder<USoundBase>TempSound(TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/Explosion01.Explosion01'"));
	if (TempSound.Succeeded())
	{
		BulletSound = TempSound.Object;
	}
}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayer::OnSpawnPointBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMainPlayer::OnSpawnPointEndOverlap);
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bGunShot)
	{
		if (CurrentTime >= MakeTime)
		{
			GunShot();
			CurrentTime = 0.f;
		}
		CurrentTime += DeltaTime;
		
	}
}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	auto PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		auto LocalPlayer = PC->GetLocalPlayer();
		auto SS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

		if (SS)
		{
			SS->AddMappingContext(IMC_Player, 1);
		}
	}

	auto InputSystem = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (InputSystem)
	{
		InputSystem->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMainPlayer::Move);
		InputSystem->BindAction(IA_Mouse, ETriggerEvent::Triggered, this, &AMainPlayer::Turn);
		InputSystem->BindAction(IA_GunShot, ETriggerEvent::Started, this, &AMainPlayer::GunShotStart);
		InputSystem->BindAction(IA_GunShot, ETriggerEvent::Completed, this, &AMainPlayer::GunShotEnd);
	}
}

void AMainPlayer::Move(const struct FInputActionValue& InputValue)
{
	FVector2D value = InputValue.Get<FVector2D>();
	Direction = VRCamera->GetForwardVector() * value.X + VRCamera->GetRightVector() * value.Y;

	AddMovementInput(Direction);
}

void AMainPlayer::Turn(const struct FInputActionValue& InputValue)
{
	FVector2D Scale = InputValue.Get<FVector2D>();
	AddControllerPitchInput(Scale.Y);
	AddControllerYawInput(Scale.X);
}

void AMainPlayer::GunShotStart(const struct FInputActionValue& InputValue)
{
	bGunShot = true;
}

void AMainPlayer::GunShotEnd(const struct FInputActionValue& InputValue)
{
	bGunShot = false;
	CurrentTime = 1.f;
}

void AMainPlayer::GunShot()
{
	GEngine->AddOnScreenDebugMessage(0, 0.5f, FColor::Red, TEXT("gunshot"));
	FVector StartPoint = VRCamera->GetComponentLocation();
	FVector EndPoint = StartPoint + VRCamera->GetForwardVector() * 1000;

	FHitResult hitInfo;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, StartPoint, EndPoint, ECC_Visibility, params);

	// 발사 사운드
	UGameplayStatics::PlaySound2D(GetWorld(), BulletSound);

	// 발사 이펙트
	FTransform BulletTrans;
	BulletTrans.SetLocation(StartPoint);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletEffactFactory, BulletTrans);

	if (bHit)
	{

	}

	// 선그리기
	DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Red, false, -1, 0, 1);
}

void AMainPlayer::OnSpawnPointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	int32 SlotIndex = CalculateSlotIndex(OtherActor); // 칸 번호 계산 로직
	OverlapMap.Add(SlotIndex, OtherActor); // 해당 칸에 객체 추가
	Spawnpoint = Cast<ASpawnPoint>( FindClosestActorToPlayer() );
	/*Tmp_Spawnpoint = Spawnpoint;
	Spawnpoint->meshcomp->SetVisibility( true );
	Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - FVector( 0.f , 0.f , 70.f ) );
	Spawnpoint->meshcomp->SetWorldRotation( FRotator( 0.f , 90.f , 0.f ) );*/

	if (Spawnpoint && Spawnpoint->meshcomp)
	{
		Spawnpoint->meshcomp->SetVisibility( true );
		Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - FVector( 0.f , 0.f , 70.f ) );
		Spawnpoint->meshcomp->SetWorldRotation( FRotator( 0.f , 90.f , 0.f ) );
		Tmp_Spawnpoint = Spawnpoint;
	}
}

void AMainPlayer::OnSpawnPointEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	int32 SlotIndex = CalculateSlotIndex(OtherActor); // 칸 번호 계산 로직
	OverlapMap.Remove(SlotIndex); // 해당 칸에서 객체 제거

	if (OtherActor == Spawnpoint)
	{
		if (Spawnpoint && Spawnpoint->meshcomp)
		{
			Spawnpoint->meshcomp->SetVisibility( false );
		}
		Spawnpoint = Cast<ASpawnPoint>( FindClosestActorToPlayer() );
		if (Spawnpoint && Spawnpoint->meshcomp)
		{
			Spawnpoint->meshcomp->SetVisibility( true );
			Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - FVector( 0.f , 0.f , 70.f ) );
			Spawnpoint->meshcomp->SetWorldRotation( FRotator( 0.f , 90.f , 0.f ) );
			Tmp_Spawnpoint = Spawnpoint;
		}
	}
}

int32 AMainPlayer::CalculateSlotIndex(AActor* Actor)
{
	FVector Location = Actor->GetActorLocation();
	int32 XIndex = FMath::FloorToInt( Location.X / 100.f );
	int32 YIndex = FMath::FloorToInt( Location.Y / 100.f );
	return XIndex + (YIndex * 11);
}

AActor* AMainPlayer::FindClosestActorToPlayer()
{
	// 플레이어 가져오기
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController || !PlayerController->GetPawn())
	{
		return nullptr; // 플레이어가 없으면 nullptr 반환
	}

	APawn* PlayerPawn = PlayerController->GetPawn();
	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	AActor* ClosestActor = nullptr;
	float MinDistance = TNumericLimits<float>::Max(); // 최대값으로 초기화

	// TMap 순회
	for (const TPair<int32, AActor*>& Pair : OverlapMap)
	{
		AActor* CurrentActor = Pair.Value;
		if (!CurrentActor) continue; // 유효하지 않은 액터는 건너뜀

		FVector ActorLocation = CurrentActor->GetActorLocation();
		float Distance = FVector::Dist(PlayerLocation, ActorLocation);

		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestActor = CurrentActor;
			if (Tmp_Spawnpoint != nullptr)
			{
				Tmp_Spawnpoint->meshcomp->SetVisibility(false);
			}
		}
	}

	return ClosestActor; // 가장 가까운 액터 반환, 없으면 nullptr
}
