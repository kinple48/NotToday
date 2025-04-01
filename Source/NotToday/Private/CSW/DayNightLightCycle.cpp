// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/DayNightLightCycle.h"

#include "EngineUtils.h"
#include "MainGameStateBase.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Kismet/GameplayStatics.h"
#include "NotToday/NotToday.h"

ADayNightLightCycle::ADayNightLightCycle()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADayNightLightCycle::BeginPlay()
{
	Super::BeginPlay();
	
	for(TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		Directional = *It;
		break;
	}
	
	// 타임라인 세팅
	if (LightCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateLighting"));
		
		// TimelineCallback에 바인딩 된 함수들에 LightCurve (float)값이 전달된다!
		LightTimeline.AddInterpFloat(LightCurve, TimelineCallback);
	}

	auto GS = UGameplayStatics::GetGameState(GetWorld());
	if (GS)
	{
		auto MainGS = Cast<AMainGameStateBase>(GS);
		if (MainGS)
		{
			MainGS->OnDayStarted.AddDynamic(this, &ADayNightLightCycle::DayLight);
			MainGS->OnNightStarted.AddDynamic(this, &ADayNightLightCycle::NightLight);
		}
	}
}

void ADayNightLightCycle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	if (LightTimeline.IsPlaying())
	{
		LightTimeline.TickTimeline(DeltaTime);
	}
}

void ADayNightLightCycle::UpdateLighting(float Value)
{
	// Intensity 값 변경
	Directional->SetBrightness(Value * 8.f);
}

void ADayNightLightCycle::DayLight()
{
	LightTimeline.PlayFromStart();
}

void ADayNightLightCycle::NightLight()
{
	LightTimeline.ReverseFromEnd();
}
