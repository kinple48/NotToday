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
#include "GameFramework/SpringArmComponent.h"
#include "Barricade.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainGameModeBase.h"
#include "CSW/ZombieBase.h"
#include "CSW/ZombieFSMComponent.h"
#include "CSW/Item/DropItem.h"

AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "GunMesh" ));
	GunMesh->SetupAttachment( GetMesh() , TEXT( "RightHandSocket" ) );

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGun( TEXT( "/Script/Engine.SkeletalMesh'/Game/LJW/Asset/BLK18/SKM_BLK18.SKM_BLK18'" ) );
	if (TempGun.Succeeded())
	{
		GunMesh->SetSkeletalMesh( TempGun.Object );
		GunMesh->SetRelativeLocationAndRotation( FVector(-135.f, 0.f, 725.f ) , FRotator( 80.f, 85.f, 270.f ) );
	}

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>( TEXT( "SpringArmComp" ) );
	SpringArmComp->SetupAttachment( RootComponent );
	SpringArmComp->bUsePawnControlRotation = false;

	VRCamera = CreateDefaultSubobject<UCameraComponent>( TEXT( "VRCamera" ) );
	VRCamera->SetupAttachment( SpringArmComp );
	VRCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	HP = HPMax;
	Reload = ReloadMax;
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayer::OnSpawnPointBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMainPlayer::OnSpawnPointEndOverlap);
	GameMode = Cast<AMainGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayer::OnBeginOverlap);
}

void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CombatState)
	{
		RotateToMouseCursor();
		CurrentTime += DeltaTime;
		if (bGunShot && Reload > 0 && CurrentTime >= MakeTime)
		{
			GunShot();
			CurrentTime = 0.f;
		}
		if (Reload <= 0 && CurrentTime >= ReloadTime)
		{
			Reload = ReloadMax;
			GameMode->SetReload( Reload , ReloadMax );
			CurrentTime = 0.f;
		}
	}

	Direction = FTransform( GetControlRotation() ).TransformVector( Direction );
	// Direction 초기화 (다음 Tick에서 입력이 없으면 멈춤)

	AddMovementInput( Direction );
	Direction = FVector::ZeroVector;
}

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
		InputSystem->BindAction(IA_GunShot, ETriggerEvent::Started, this, &AMainPlayer::GunShotStart);
		InputSystem->BindAction(IA_GunShot, ETriggerEvent::Completed, this, &AMainPlayer::GunShotEnd);
	}
}

void AMainPlayer::Move(const struct FInputActionValue& InputValue)
{
	FVector2D value = InputValue.Get<FVector2D>();

	Direction.X = value.X;
	Direction.Y = value.Y;
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
	RotateToMouseCursor();
	//GEngine->AddOnScreenDebugMessage(0, 0.5f, FColor::Red, TEXT("gunshot"));
	FVector StartPoint = GunMesh->GetComponentLocation();
	FVector ForwardVector;
	APlayerController* PlayerController = Cast<APlayerController>( GetController() );
	if (PlayerController)
	{
		FHitResult HitResult;

		// 마우스 포인터가 가리키는 위치를 가져오기
		if (PlayerController->GetHitResultUnderCursor( ECC_Visibility , false , HitResult ))
		{
			FVector MouseLocation = HitResult.Location;
			FVector TargetLocation = GunMesh->GetComponentLocation();
			FVector LookAtDirection = (MouseLocation - TargetLocation).GetSafeNormal2D();
			float TargetYaw = FMath::Atan2( LookAtDirection.Y , LookAtDirection.X ) * 180.0f / PI;
			FRotator CurrentRotation = GetActorRotation();
			CurrentRotation.Yaw = TargetYaw;
			ForwardVector = CurrentRotation.Vector(); // Forward 방향 벡터 생성
		}
	}

	FVector EndPoint = StartPoint + ForwardVector * 700;

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
		AActor* HitActor = hitInfo.GetActor();
		AZombieBase* zombie = Cast<AZombieBase>( HitActor );
		if (zombie)
		{
			zombie->FSM->OnDamage(5);
		}
	}
	SetReload();
	// 선그리기
	DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::White, false, 1, 0, 1);
}

void AMainPlayer::OnSpawnPointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag( TEXT( "Object" ) ))
	{
		GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "Can't Spawn" ) );
		GameMode->PrintRemove();
		BarricadeObject = Cast<ABarricade>( OtherActor );
	}
	else if (OtherActor->ActorHasTag( TEXT("SpawnPoint")))
	{
		Spawnpoint = Cast<ASpawnPoint>( OtherActor );
		if (Tmp_Spawnpoint)
		{
			int32 Tmp_SlotIndex = CalculateSlotIndex( Tmp_Spawnpoint );
			OverlapMap.Remove( Tmp_SlotIndex );
			Tmp_Spawnpoint->meshcomp->SetVisibility( false );

			int32 SlotIndex = CalculateSlotIndex( Spawnpoint ); // 칸 번호 계산 로직
			OverlapMap.Add( SlotIndex , Spawnpoint ); // 해당 칸에 객체 추가
			Spawnpoint = Cast<ASpawnPoint>( FindClosestActorToPlayer() );

			if (Spawnpoint && !Spawnpoint->spawnstate)
			{
				if (BarricadeStoreData > 0)
				{
					GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "spawnable" ) );
					GameMode->PrintPlace();
					Spawnpoint->meshcomp->SetVisibility( true );
					Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - FVector( 0.f , 0.f , 70.f ) );
					Spawnpoint->meshcomp->SetWorldRotation( FRotator( 0.f , 90.f , 0.f ) );
					Tmp_Spawnpoint = Spawnpoint;
				}
				else
				{
					GameMode->PrintElse();
				}
			}
		}
		else {
			OverlapEvent(Spawnpoint,this);
		}
	}
}

void AMainPlayer::OnSpawnPointEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag( TEXT( "SpawnPoint" ) ))
	{
		Spawnpoint = Cast<ASpawnPoint>( OtherActor );
		int32 SlotIndex = CalculateSlotIndex(Spawnpoint);
		OverlapMap.Remove( SlotIndex );
		if (Spawnpoint)
		{
			Spawnpoint->meshcomp->SetVisibility( false );
		}

		// 새로운 Spawnpoint 찾기
		Spawnpoint = Cast<ASpawnPoint>( FindClosestActorToPlayer() );
		
		if (Spawnpoint && !Spawnpoint->spawnstate)
		{
			if (BarricadeStoreData > 0)
			{
				//GameMode->PrintPlace();
				Spawnpoint->meshcomp->SetVisibility( true );
				Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - FVector( 0.f , 0.f , 70.f ) );
				Spawnpoint->meshcomp->SetWorldRotation( FRotator( 0.f , 90.f , 0.f ) );
				Tmp_Spawnpoint = Spawnpoint;
			}
			else
			{
				//GameMode->PrintElse();
			}
		}
	}
	/*else if (OtherActor->ActorHasTag( TEXT( "Object" ) ))
	{
			GameMode->PrintElse();
	}*/
}


int32 AMainPlayer::CalculateSlotIndex(ASpawnPoint* Actor)
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
			/*if (Tmp_Spawnpoint != nullptr)
			{
				Tmp_Spawnpoint->meshcomp->SetVisibility( false );
			}*/
		}
	}

	return ClosestActor; // 가장 가까운 액터 반환, 없으면 nullptr
}

void AMainPlayer::OverlapEvent( ASpawnPoint* spawnpoint, AMainPlayer* player )
{
	int32 SlotIndex = CalculateSlotIndex( spawnpoint ); // 칸 번호 계산 로직
	OverlapMap.Add( SlotIndex , spawnpoint ); // 해당 칸에 객체 추가
	Spawnpoint = Cast<ASpawnPoint>( FindClosestActorToPlayer() );

	if (spawnpoint && !spawnpoint->spawnstate)
	{
		if (BarricadeStoreData > 0)
		{
			GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "spawnable" ) );
			GameMode->PrintPlace();
			spawnpoint->meshcomp->SetVisibility( true );
			spawnpoint->meshcomp->SetWorldLocation( spawnpoint->GetActorLocation() - FVector( 0.f , 0.f , 70.f ) );
			spawnpoint->meshcomp->SetWorldRotation( FRotator( 0.f , 90.f , 0.f ) );
			player->Tmp_Spawnpoint = spawnpoint;
		}
		else
		{
			GameMode->PrintElse();
		}
	}
}

void AMainPlayer::SpawnObject()
{
	if (Spawnpoint && BarricadeStoreData > 0)
	{
		Spawnpoint->spawnstate = true;
		BarricadeStoreData -= 1;
		CashData -= BarricadePrice;
		GameMode->PrintStore();
		GameMode->PrintCash();
		FVector ActorLocation = Spawnpoint->GetActorLocation() - FVector( 0.f , 0.f , 70.f );
		FTransform SpawnTransform;
		SpawnTransform.SetLocation( ActorLocation );
		ABarricade* Barricade = GetWorld()->SpawnActor<ABarricade>( BarricadeFactory , SpawnTransform );
	}
}

void AMainPlayer::SetDamage( int32 damage )
{
	HP -= damage;
	if (GameMode)
	{
		GameMode->SetHP( HP , HPMax );
	}
}

void AMainPlayer::SetReload( )
{
	Reload -= 1;
	if (GameMode)
	{
		GameMode->SetReload( Reload , ReloadMax );
	}
}

void AMainPlayer::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto item = Cast<ADropItem>(OtherActor);
	if (item)
	{
		item->Apply(this);
	}
}

void AMainPlayer::RotateToMouseCursor()
{
	APlayerController* PlayerController = Cast<APlayerController>( GetController() );
	if (PlayerController)
	{
		FHitResult HitResult;

		// 마우스 포인터가 가리키는 위치를 가져오기
		if (PlayerController->GetHitResultUnderCursor( ECC_Visibility , false , HitResult ))
		{
			FVector MouseLocation = HitResult.Location;  // 마우스가 가리키는 월드 좌표
			FVector CharacterLocation = GetActorLocation();

			// 캐릭터와 마우스 위치 간의 방향 벡터 계산 (2D 평면에서만)
			FVector LookAtDirection = (MouseLocation - CharacterLocation).GetSafeNormal2D();

			// LookAtDirection을 기반으로 Yaw 값 계산
			float TargetYaw = FMath::Atan2( LookAtDirection.Y , LookAtDirection.X ) * 180.0f / PI;

			// 현재 캐릭터 회전값 가져오기
			FRotator CurrentRotation = GetActorRotation();

			// Z축(Yaw)만 변경하고 X(Pitch)와 Y(Roll)는 유지
			CurrentRotation.Yaw = TargetYaw;

			// 캐릭터 회전 적용
			SetActorRotation( CurrentRotation );
		}
	}
}
