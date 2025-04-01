// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "DayNightLightCycle.generated.h"

class ASkyLight;
class ADirectionalLight;

UCLASS()
class NOTTODAY_API ADayNightLightCycle : public AActor
{
	GENERATED_BODY()
	
public:	
	ADayNightLightCycle();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:	

	// 타임라인
	UFUNCTION()
	void UpdateLighting(float Value);

    UPROPERTY(EditAnywhere)
    TObjectPtr<UCurveFloat> LightCurve;

	UFUNCTION()
	void DayLight();

	UFUNCTION()
	void NightLight();
    
private:
    UPROPERTY(EditAnywhere)
	TObjectPtr<ADirectionalLight> Directional;

	//UPROPERTY(EditAnywhere)
	//TObjectPtr<ASkyLight> SkyLight;

	FTimeline LightTimeline;
};
