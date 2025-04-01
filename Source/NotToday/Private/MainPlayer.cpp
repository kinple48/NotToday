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
#include "Components/SphereComponent.h"
#include "Day_UI.h"
#include "DefenseTower.h"
#include "CSW/Item/DropItem.h"
#include "MainGameStateBase.h"
#include "Components/BoxComponent.h"
#include "NotToday/NotToday.h"
#include "MotionControllerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/WidgetComponent.h"
#include "Night_UI.h"
#include "UObject/UObjectGlobals.h"
#include "PlayerAnimInstance.h"
#include "CSW/Subsystem/SoundManagerSubsystem.h"

class USoundManagerSubsystem;

AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ZombieTarget, ECR_Block); // ZombieTarget 채널
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ZombieHitBox, ECR_Overlap);

	LeftHand = CreateDefaultSubobject<UMotionControllerComponent>( TEXT( "LeftHand" ) );
	LeftHand->SetTrackingMotionSource( TEXT( "Left" ) );
	LeftHand->SetupAttachment( RootComponent );

	RightHand = CreateDefaultSubobject<UMotionControllerComponent>( TEXT( "RightHand" ) );
	RightHand->SetTrackingMotionSource( TEXT( "Right" ) );
	RightHand->SetupAttachment( RootComponent );

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "GunMesh" ));
	GunMesh->SetupAttachment( GetMesh() , TEXT( "RightHandSocket" ) );

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGun( TEXT( "/Script/Engine.SkeletalMesh'/Game/LJW/Asset/BLK18/SKM_BLK18.SKM_BLK18'" ) );
	if (TempGun.Succeeded())
	{
		GunMesh->SetSkeletalMesh( TempGun.Object );
		GunMesh->SetRelativeLocationAndRotation( FVector( 1.f , -17.f , 7.f ) , FRotator( 79.f , 90.f , -86.f ) );
	}

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>( TEXT( "SpringArmComp" ) );
	SpringArmComp->SetupAttachment( RootComponent );
	SpringArmComp->bUsePawnControlRotation = false;

	VRCamera = CreateDefaultSubobject<UCameraComponent>( TEXT( "VRCamera" ) );
	VRCamera->SetupAttachment( SpringArmComp );
	VRCamera->bUsePawnControlRotation = false;

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>( TEXT( "WidgetComp" ) );
	WidgetComp->AttachToComponent( VRCamera , FAttachmentTransformRules::KeepWorldTransform );
	FVector DirectionUI = VRCamera->GetForwardVector();
	WidgetComp->SetWorldRotation( FRotationMatrix::MakeFromX( -DirectionUI ).Rotator() );

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;


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

	CrosshairComp = CreateDefaultSubobject<UChildActorComponent>( TEXT( "CrosshairComp" ) );
	CrosshairComp->SetupAttachment( RootComponent );
	ConstructorHelpers::FClassFinder<AActor> TempCrosshair( TEXT( "/Script/Engine.Blueprint'/Game/LJW/Blueprint/BP_CrossHair.BP_CrossHair_C'" ) );
	if (TempCrosshair.Succeeded())
	{
		CrosshairComp->SetChildActorClass( TempCrosshair.Class );
	}

	ConstructorHelpers::FObjectFinder<UInputAction>TempIA_Buy( TEXT( "/Script/EnhancedInput.InputAction'/Game/LJW/Input/IA_Buy.IA_Buy'" ) );
	if (TempIA_Buy.Succeeded())
	{
		IA_Buy = TempIA_Buy.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction>TempIA_NextLevel( TEXT( "/Script/EnhancedInput.InputAction'/Game/LJW/Input/IA_NextLevel.IA_NextLevel'" ) );
	if (TempIA_NextLevel.Succeeded())
	{
		IA_NextLevel = TempIA_NextLevel.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction>TempIA_Place( TEXT( "/Script/EnhancedInput.InputAction'/Game/LJW/Input/IA_Place.IA_Place'" ) );
	if (TempIA_Place.Succeeded())
	{
		IA_Place = TempIA_Place.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction>TempIA_Remove( TEXT( "/Script/EnhancedInput.InputAction'/Game/LJW/Input/IA_Remove.IA_Remove'" ) );
	if (TempIA_Remove.Succeeded())
	{
		IA_Remove = TempIA_Remove.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction>TempIA_Object( TEXT( "/Script/EnhancedInput.InputAction'/Game/LJW/Input/IA_Object.IA_Object'" ) );
	if (TempIA_Object.Succeeded())
	{
		IA_Object = TempIA_Object.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction>TempIA_HiddenGame( TEXT( "/Script/EnhancedInput.InputAction'/Game/LJW/Input/IA_HiddenGame.IA_HiddenGame'" ) );
	if (TempIA_HiddenGame.Succeeded())
	{
		IA_HiddenGame = TempIA_HiddenGame.Object;
	}

}

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	CombatState = true;
	UClass* WidgetClass = LoadObject<UClass>( nullptr , TEXT( "/Script/UMGEditor.WidgetBlueprint'/Game/LJW/UI/BP_Night_UI.BP_Night_UI_C'" ) );
	if (WidgetClass)
	{
		WidgetComp->SetWidgetClass( WidgetClass );
		WidgetComp->InitWidget();
		NightUIInstance = Cast<UNight_UI>( WidgetComp->GetUserWidgetObject() );
		WidgetComp->SetVisibility( true );
		WidgetComp->SetDrawSize( FVector2D( 1280 , 720 ) );
		WidgetComp->SetWorldRotation( WidgetComp->GetComponentRotation() + FRotator(0.f , 0.f , 0.f));
		WidgetComp->SetRelativeLocation( VRCamera->GetForwardVector() * 1000 + FVector( 0.f , 0.f , 400.f ));
	}
	HP = HPMax;
	Reload = ReloadMax;
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayer::OnSpawnPointBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMainPlayer::OnSpawnPointEndOverlap);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayer::OnDropItemBeginOverlap);
	
	GameMode = Cast<AMainGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );

	auto gs = Cast<AMainGameStateBase>( GetWorld()->GetGameState() );
	if (!gs)
	{
		return;
	}

	gs->OnDayStarted.AddDynamic( this , &AMainPlayer::DayState );
	gs->OnNightStarted.AddDynamic( this , &AMainPlayer::NightState );
}

void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//if (CombatState)
	//{
	//	//RotateToMouseCursor();
	//	DrawCrossHair();
	//	CurrentTime += DeltaTime;
	//	if (bGunShot && Reload > 0 && CurrentTime >= MakeTime)
	//	{
	//		GunShot();
	//		CurrentTime = 0.f;
	//	}
	//	else if (Reload <= 0 && CurrentTime >= ReloadTime)
	//	{
	//		Reload = ReloadMax;
	//		GameMode->SetReload( Reload , ReloadMax );
	//		CurrentTime = 0.f;
	//	}
	//}

	if (CombatState)
	{
		DrawCrossHair();
		CurrentTime += DeltaTime;
		if (Reload > 0)
		{
			if (bGunShot && CurrentTime >= MakeTime)
			{
				Reload -= 1;
				GunShot();
				CurrentTime = 0.f;
			}
		}
		else
		{
			Reload = ReloadMax;
			GameMode->SetReload( Reload , ReloadMax );
			CurrentTime = 0.f;

			// Reload SFX
			auto SM = GetGameInstance()->GetSubsystem<USoundManagerSubsystem>();
			if (SM)
			{
				SM->PlaySoundAtLocation(ESoundType::Reload, GetActorLocation());
			}

			if (CurrentTime >= ReloadTime)
			{
				Reload = ReloadMax;
				GameMode->SetReload( Reload , ReloadMax );
				CurrentTime = 0.f;
			}
		}
	
	
	//Direction = FTransform( GetControlRotation() ).TransformVector( Direction );
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
		InputSystem->BindAction( IA_Turn , ETriggerEvent::Triggered , this , &AMainPlayer::Turn );
		InputSystem->BindAction( IA_Buy , ETriggerEvent::Started , this , &AMainPlayer::Buy );
		InputSystem->BindAction( IA_NextLevel , ETriggerEvent::Started , this , &AMainPlayer::NextLevel );
		InputSystem->BindAction( IA_Place , ETriggerEvent::Started , this , &AMainPlayer::Place );
		InputSystem->BindAction( IA_Remove , ETriggerEvent::Started , this , &AMainPlayer::Remove );
		InputSystem->BindAction( IA_Object , ETriggerEvent::Started , this , &AMainPlayer::Object );
		InputSystem->BindAction( IA_HiddenGame , ETriggerEvent::Started , this , &AMainPlayer::HiddenGame );
	}
}

void AMainPlayer::Move(const struct FInputActionValue& InputValue)
{
	FVector2D value = InputValue.Get<FVector2D>();

	Direction.X = value.X;
	Direction.Y = value.Y;
}

void AMainPlayer::Turn( const struct FInputActionValue& InputValue )
{
	// 1. Thumbstick X와 Y 값 가져오기
	FVector2D value = InputValue.Get<FVector2D>();

	// 2. Swizzle 처리: Unreal 좌표계에 맞게 X와 Y를 교환
	float X = value.X; // Y축을 X축으로 사용
	float Y = value.Y; // X축을 Y축으로 사용

	// 4. Swizzled 값을 기반으로 Z축(Yaw) 각도 계산
	float TargetYaw = FMath::Atan2( X , Y ) * 180.0f / PI;

	// 5. Unreal 좌표계 보정 (-180° ~ 180° 범위로 제한)
	TargetYaw = FMath::Fmod( TargetYaw + 360.0f , 360.0f );
	if (TargetYaw > 180.0f) TargetYaw -= 360.0f;

	// 6. 현재 캐릭터 회전값 가져오기
	FRotator CurrentRotation = GetActorRotation();

	// 7. 부드러운 회전 적용 (Yaw 값만 변경)
	FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, FRotator( 0.0f , TargetYaw , 0.0f ), GetWorld()->GetDeltaSeconds(), RotationSpeed);

	// 8. 새로운 회전값 적용
	SetActorRotation( SmoothRotation );
}

void AMainPlayer::GunShotStart(const struct FInputActionValue& InputValue)
{
	bGunShot = true;
}

void AMainPlayer::GunShotEnd(const struct FInputActionValue& InputValue)
{
	bGunShot = false;
}

void AMainPlayer::GunShot()
{
	FVector StartPoint = GunMesh->GetComponentLocation();
	FVector EndPoint = StartPoint + GunMesh->GetRightVector()* 1000;
	FHitResult hitInfo;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, StartPoint, EndPoint, ECC_Visibility, params);

	// 발사 사운드
	UGameplayStatics::PlaySound2D(GetWorld(), BulletSound);

	// 발사 이펙트
	FTransform BulletTrans;
	BulletTrans.SetLocation(GunMesh->GetComponentLocation());
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

void AMainPlayer::DrawCrossHair()
{
	FVector StartPos = GunMesh->GetComponentLocation();
	FVector EndPos = StartPos + GunMesh->GetRightVector() * 1000;
	FHitResult hitInfo;
	FCollisionQueryParams params;
	params.AddIgnoredActor( this );

	bool bHit = GetWorld()->LineTraceSingleByChannel( hitInfo , StartPos , EndPos , ECC_Visibility , params );

	AActor* HitActor = hitInfo.GetActor();
	AZombieBase* zombie = Cast<AZombieBase>( HitActor );

	if (bHit && zombie)
	{
		CrosshairComp->SetWorldLocation( hitInfo.Location );
		//DrawDebugLine( GetWorld() , StartPos , hitInfo.Location , FColor::Red , false , 1 , 0 , 1 );
	}
	else
	{
		CrosshairComp->SetWorldLocation( EndPos );
		//DrawDebugLine( GetWorld() , StartPos , EndPos , FColor::Red , false , 1 , 0 , 1 );
	}
}

void AMainPlayer::OnSpawnPointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag( TEXT( "Object" ) ))
	{
		GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "Can't Spawn" ) );
		GameMode->PrintRemove();
		OverlapActor = Cast<AActor>( OtherActor );
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
				if (StoreData > 0)
				{
					GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "spawnable" ) );
					GameMode->PrintPlace();
					Spawnpoint->meshcomp->SetVisibility( true );
					Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - SpawnLocation);
					Spawnpoint->meshcomp->SetWorldRotation( SpawnRotation );
					Spawnpoint->meshcomp->SetRelativeScale3D( SpawnScale );
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
			if (StoreData > 0)
			{
				//GameMode->PrintPlace();
				Spawnpoint->meshcomp->SetVisibility( true );
				Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - SpawnLocation );
				Spawnpoint->meshcomp->SetWorldRotation( SpawnRotation );
				Spawnpoint->meshcomp->SetRelativeScale3D( SpawnScale );
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

void AMainPlayer::OnDropItemBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 아이템, 재화 획득 처리 함수
	auto item = Cast<ADropItem>(OtherActor);
	if (item)
	{
		item->Apply(this);
	}
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
		}
	}

	return ClosestActor; // 가장 가까운 액터 반환, 없으면 nullptr
}

void AMainPlayer::OverlapEvent( ASpawnPoint* spawnpoint, AMainPlayer* player )
{
	int32 SlotIndex = CalculateSlotIndex( spawnpoint ); // 칸 번호 계산 로직
	OverlapMap.Add( SlotIndex , spawnpoint ); // 해당 칸에 객체 추가
	Spawnpoint = Cast<ASpawnPoint>( FindClosestActorToPlayer() );

	if (Spawnpoint && !Spawnpoint->spawnstate)
	{
		if (StoreData > 0)
		{
			GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "spawnable" ) );
			GameMode->PrintPlace();
			Spawnpoint->meshcomp->SetVisibility( true );
			Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - SpawnLocation );
			Spawnpoint->meshcomp->SetWorldRotation( SpawnRotation );
			Spawnpoint->meshcomp->SetRelativeScale3D( SpawnScale );
			player->Tmp_Spawnpoint = Spawnpoint;
		}
		else
		{
			GameMode->PrintElse();
		}
	}
}

void AMainPlayer::SpawnObject()
{
	if (Spawnpoint && StoreData > 0)
	{
		Spawnpoint->spawnstate = true;
		FVector ActorLocation = Spawnpoint->GetActorLocation() - FVector( 0.f , 0.f , 70.f );
		FTransform SpawnTransform;
		SpawnTransform.SetLocation( ActorLocation );
		AActor* SpawnedActor = nullptr;

		if (ObjectType)
		{
			SpawnedActor = GetWorld()->SpawnActor<ABarricade>( BarricadeFactory , SpawnTransform );
			USoundBase* SoundType = LoadObject<USoundBase>( nullptr , TEXT( "/Script/Engine.SoundWave'/Game/LJW/Sound/wood-smash-1-170410.wood-smash-1-170410'" ) );
			ObjectSound = SoundType;
		}
		else
		{
			SpawnedActor = GetWorld()->SpawnActor<ADefenseTower>( AutoTurretFactory , SpawnTransform );
			USoundBase* SoundType = LoadObject<USoundBase>( nullptr , TEXT( "/Script/Engine.SoundWave'/Game/LJW/Sound/falling-of-heavy-object-291096.falling-of-heavy-object-291096'" ) );
			ObjectSound = SoundType;
		}
		



		if (SpawnedActor)
		{
			ApplyBouncingEffect( SpawnedActor , ActorLocation , 400.0f , 0.3f );
		}
		UGameplayStatics::PlaySound2D( GetWorld() , ObjectSound );
		FTransform EffectTrans;
		EffectTrans.SetLocation( Spawnpoint->GetActorLocation() );
		UGameplayStatics::SpawnEmitterAtLocation( GetWorld() , ObjectEffectFactory , EffectTrans );
	}
}

void AMainPlayer::SetDamage( int32 damage )
{
	if (HP > 0)
	{
		HP -= damage;
		if (GameMode)
		{
			GameMode->SetHP( HP , HPMax );
		}
	}

	else
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		auto anim = Cast<UPlayerAnimInstance> (AnimInstance);
		if (AnimInstance)
		{
			anim->deadstate = true;
			APlayerController* playercontroller = GetWorld()->GetFirstPlayerController();
			playercontroller->GetPawn()->DisableInput( playercontroller );
		}

		// 좀비의 공격 타겟팅 Ignore
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ZombieTarget, ECR_Ignore);
        GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ZombieHitBox, ECR_Ignore);

		// 일정시간 후 게임 종료
		FTimerHandle GameQuitTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(GameQuitTimerHandle,
			[this]() {GameMode->EndGame();},
			5.f, false);
	}
}

void AMainPlayer::SetReload( )
{
	//Reload -= 1;
	if (GameMode)
	{
		GameMode->SetReload( Reload , ReloadMax );
	}
}

void AMainPlayer::DayState()
{
	GEngine->AddOnScreenDebugMessage( 0 , 5.f , FColor::Red , TEXT( "DayStateCall" ) );
	CombatState = false;
	CrosshairComp->SetVisibility( false );

	UClass* WidgetClass = LoadObject<UClass>( nullptr , TEXT( "/Script/UMGEditor.WidgetBlueprint'/Game/LJW/UI/BP_Day_UI.BP_Day_UI_C'" ) );
	if (WidgetClass)
	{
		WidgetComp->SetWidgetClass( WidgetClass );
		WidgetComp->InitWidget();
		DayUIInstance = Cast<UDay_UI>( WidgetComp->GetUserWidgetObject() );
		if (DayUIInstance)
		{
			GameMode->PrintCash();
			GameMode->PrintStore();
			GameMode->PrintPrice();
		}
		WidgetComp->SetVisibility( true );
		WidgetComp->SetDrawSize( FVector2D( 1280 , 720 ) );
		WidgetComp->SetWorldRotation( WidgetComp->GetComponentRotation());
		WidgetComp->SetRelativeLocation( VRCamera->GetForwardVector() * 1000 + FVector( 0.f , 0.f , 400.f ) );
	}

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
				Object2->meshcomp->SetCollisionEnabled( ECollisionEnabled::NoCollision );
				Object2->meshcomp->SetSimulatePhysics( true );
			}
			auto Object1 = Cast<ABarricade>( Actor );
			if (Object1)
			{
				Object1->meshcomp->SetCollisionEnabled( ECollisionEnabled::NoCollision );
				Object1->meshcomp->SetSimulatePhysics( true );
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
				Object->boxcomp->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );
			}
		}
	}
	WoodenBarricade();
}

void AMainPlayer::NightState()
{
	CombatState = true;
	CrosshairComp->SetVisibility( true );

	UClass* WidgetClass = LoadObject<UClass>( nullptr , TEXT( "/Script/UMGEditor.WidgetBlueprint'/Game/LJW/UI/BP_Night_UI.BP_Night_UI_C'" ) );
	if (WidgetClass)
	{
		WidgetComp->SetWidgetClass( WidgetClass );
		WidgetComp->InitWidget();
		NightUIInstance = Cast<UNight_UI>( WidgetComp->GetUserWidgetObject() );
		if (NightUIInstance)
		{
			HP = HPMax;
			Reload = ReloadMax;
			GameMode->SetHP( HP, HPMax );
			GameMode->SetReload( Reload, ReloadMax );
			GameMode->PrintCash();
			GameMode->PrintScore();
		}
		WidgetComp->SetVisibility( true );
		WidgetComp->SetDrawSize( FVector2D( 1280 , 720 ) );
		WidgetComp->SetWorldRotation( WidgetComp->GetComponentRotation());
		WidgetComp->SetRelativeLocation( VRCamera->GetForwardVector() * 1000 + FVector( 0.f , 0.f , 400.f ) );
	}

	TArray<AActor*> ActorsWithTag;
	FName TagToSearch = TEXT( "Object" );
	GameMode->SetHP( HP , HPMax );
	GameMode->SetReload( Reload , ReloadMax );

	UGameplayStatics::GetAllActorsWithTag( GetWorld() , TagToSearch , ActorsWithTag );
	for (AActor* Actor : ActorsWithTag)
	{
		if (Actor)
		{
			auto Object2 = Cast<ADefenseTower>( Actor );
			if (Object2)
			{
				Object2->meshcomp->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );
				Object2->meshcomp->SetSimulatePhysics( false );
			}
			auto Object1 = Cast<ABarricade>( Actor );
			if (Object1)
			{
				Object1->meshcomp->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );
				Object1->meshcomp->SetSimulatePhysics( false );
			}


		}
	}

	TArray<AActor*> ActorWithTag;
	TagToSearch = TEXT( "SpawnPoint" );

	UGameplayStatics::GetAllActorsWithTag( GetWorld() , TagToSearch , ActorWithTag );

	for (AActor* Actor : ActorWithTag)
	{
		if (Actor)
		{
			auto Object = Cast<ASpawnPoint>( Actor );
			if (Object)
			{
				Object->boxcomp->SetCollisionEnabled( ECollisionEnabled::NoCollision );
			}
		}
	}
}

void AMainPlayer::Buy( const struct FInputActionValue& InputValue )
{
	if (!CombatState)
	{
		if ((CashData - Price) >= 0)
		{
			if (ObjectType)
			{
				BarricadeStoreData += 1;
				StoreData = BarricadeStoreData;
			}
			else
			{
				AutoTurretStoreData += 1;
				StoreData = AutoTurretStoreData;
			}

			CashData -= Price;
			if (Spawnpoint)
			{
				OverlapEvent( Spawnpoint , this );
			}

			if (GameMode)
			{
				GameMode->PrintPlace();
				GameMode->PrintStore();
				GameMode->PrintCash();
			}
		}
	}
}

void AMainPlayer::NextLevel( const struct FInputActionValue& InputValue )
{
	if (!CombatState)
	{
		auto GameState = GetWorld()->GetGameState();
		if (GameState)
		{
			auto MainGameState = Cast<AMainGameStateBase>( GameState );
			if (MainGameState)
			{
				MainGameState->SetDayNightState( EDayNightState::Night );
			}
		}
	}
	
}

void AMainPlayer::Place( const struct FInputActionValue& InputValue )
{
	if (!CombatState)
	{
		if (Spawnpoint && !Spawnpoint->spawnstate)
		{
			SpawnObject();
			if (ObjectType && BarricadeStoreData > 0)
			{
				BarricadeStoreData -= 1;
				StoreData = BarricadeStoreData;
			}
			else if (!ObjectType && AutoTurretStoreData > 0)
			{
				AutoTurretStoreData -= 1;
				StoreData = AutoTurretStoreData;
			}
			GameMode->PrintStore();
			Spawnpoint->meshcomp->SetVisibility( false );
		}
		else if (Spawnpoint && Spawnpoint->spawnstate)
		{
			if (OverlapActor)
			{
				if (OverlapActor->ActorHasTag( TEXT( "Object" ) ))
				{
					OverlapActor->Destroy();
					if (ObjectType)
					{
						BarricadeStoreData += 1;
						StoreData = BarricadeStoreData;
					}
					else
					{
						AutoTurretStoreData += 1;
						StoreData = AutoTurretStoreData;
					}

					GameMode->PrintStore();
					GameMode->PrintPlace();
					Spawnpoint->spawnstate = false;
					Spawnpoint->meshcomp->SetVisibility( true );
				}
			}
		}
	}
}

void AMainPlayer::Remove( const struct FInputActionValue& InputValue )
{
	if (!CombatState)
	{
		GEngine->AddOnScreenDebugMessage( -1 , 5.f , FColor::Green , TEXT( "Remove" ) );
	}
}

void AMainPlayer::WoodenBarricade()
{
	ObjectType = true;
	UStaticMesh* MeshType = LoadObject<UStaticMesh>( nullptr , TEXT( "/Script/Engine.StaticMesh'/Game/LJW/Asset/Barricade/source/SM_Barricade.SM_Barricade'" ) );
	StoreData = BarricadeStoreData;
	Price = BarricadePrice;
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
			barricade->meshcomp->SetStaticMesh( MeshType );

			SpawnLocation = FVector( 0.f , 0.f , 70.f );
			SpawnRotation = FRotator( 0.f , 90.f , 0.f );
			SpawnScale = FVector( 0.5 , 0.3 , 0.5 );

			if (Spawnpoint)
			{
				Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - SpawnLocation );
				Spawnpoint->meshcomp->SetWorldRotation( SpawnRotation );
				Spawnpoint->meshcomp->SetRelativeScale3D( SpawnScale );
			}
		}
	}
}

void AMainPlayer::AutoTurret()
{
	ObjectType = false;
	GEngine->AddOnScreenDebugMessage( 0 , 0.5f , FColor::Red , TEXT( "AutoTurret" ) );
	UStaticMesh* MeshType = LoadObject<UStaticMesh>( nullptr , TEXT( "/Script/Engine.StaticMesh'/Game/LJW/Asset/defence-tower/source/StaticMesh.StaticMesh'" ) );

	StoreData = AutoTurretStoreData;
	Price = AutoTurretPrice;
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
			defensetower->meshcomp->SetStaticMesh( MeshType );

			SpawnLocation = FVector( 0.f , 0.f , 30.f );
			SpawnRotation = FRotator( 0.f , 90.f , 0.f );
			SpawnScale = FVector( 0.3 , 0.2 , 0.3 );

			if (Spawnpoint)
			{
				Spawnpoint->meshcomp->SetWorldLocation( Spawnpoint->GetActorLocation() - SpawnLocation );
				Spawnpoint->meshcomp->SetWorldRotation( SpawnRotation );
				Spawnpoint->meshcomp->SetRelativeScale3D( SpawnScale );
			}
		}
	}
}

void AMainPlayer::Object( const struct FInputActionValue& InputValue )
{
	/*if (!CombatState)
	{
		if (ObjectType)
		{
			AutoTurret();
			if (UDay_UI* WidgetInstance = Cast<UDay_UI>( WidgetComp->GetUserWidgetObject() )) {
				WidgetInstance->AnimateTextBoxPositions( 1.0f, true );
			}
		}

		else if(!ObjectType)
		{
			WoodenBarricade();
			if (UDay_UI* WidgetInstance = Cast<UDay_UI>( WidgetComp->GetUserWidgetObject() )) {
				WidgetInstance->AnimateTextBoxPositions( 1.0f, false );
			}
		}
	}*/

	
	if (!CombatState && WidgetComp) { // WidgetComp 유효성 검사 추가
		if (UDay_UI* WidgetInstance = Cast<UDay_UI>( WidgetComp->GetUserWidgetObject() )) {
			if (ObjectType) {
				AutoTurret();
				WidgetInstance->AnimateTextBoxPositions( 1.0f , true );
			}
			else {
				WoodenBarricade();
				WidgetInstance->AnimateTextBoxPositions( 1.0f , false );
			}
		}
	}
	
}

float AMainPlayer::EaseOutBounce( float x )
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1 / d1)
	{
		return n1 * x * x;
	}
	else if (x < 2 / d1)
	{
		x -= 1.5f / d1;
		return n1 * x * x + 0.98f;
	}
	else if (x < 2.5f / d1)
	{
		x -= 2.25f / d1;
		return n1 * x * x + 0.9875f;
	}
	else
	{
		x -= 2.625f / d1;
		return n1 * x * x + 0.984375f;
	}
}

float AMainPlayer::FlippedEaseOutBounce( float x )
{
	return 1.0f - EaseOutBounce( x );
}

void AMainPlayer::ApplyBouncingEffect( AActor* TargetActor , FVector Location , float BounceHeight , float Duration )
{
	if (!TargetActor) return;

	float StartTime = GetWorld()->GetTimeSeconds();

	GetWorld()->GetTimerManager().SetTimer( TimerHandle , [this , TargetActor , StartTime , Duration , Location , BounceHeight]()
	{
		float Elapsed = GetWorld()->GetTimeSeconds() - StartTime;
		float t = Elapsed / Duration; 

		if (t > 1.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer( TimerHandle );
			return;
		}

		float BounceOffset = FlippedEaseOutBounce( t ) * BounceHeight;

		FVector NewLocation = Location + FVector( 0 , 0 , BounceOffset );
		TargetActor->SetActorLocation( NewLocation );

	} , 0.016f , true );
}

void AMainPlayer::HiddenGame( const struct FInputActionValue& InputValue )
{
	//GEngine->AddOnScreenDebugMessage( -1 , 5.f , FColor::Black , TEXT( "hiddengame" ));
	TArray<AActor*> ActorWithTag;
	FName TagToSearch = TEXT( "SpawnPoint" );

	UGameplayStatics::GetAllActorsWithTag( GetWorld() , TagToSearch , ActorWithTag );

	for (AActor* Actor : ActorWithTag)
	{
		if (Actor)
		{
			auto Object = Cast<ASpawnPoint>( Actor );
			if (Object)
			{
				if (hiddengamecheck)
				{
					Object->boxcomp->SetHiddenInGame( true );
				}
				else
				{
					Object->boxcomp->SetHiddenInGame( false );
				}
			}
		}
	}
	if (hiddengamecheck)
	{
		hiddengamecheck = false;
	}
	else
	{
		hiddengamecheck = true;
	}
}
