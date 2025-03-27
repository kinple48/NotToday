// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropItem.generated.h"

class AMainPlayer;
class USphereComponent;
class USoundCue;

UCLASS(Abstract)
class NOTTODAY_API ADropItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADropItem();
	virtual void Tick(float DeltaTime) override;
	
	virtual void Apply(AMainPlayer* Player); // UE추상클래스의 순수가상함수

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 180.f;

	UPROPERTY(EditAnywhere)
	FVector MeshLocationOffset;

	void Rotate(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TObjectPtr<USoundCue> PickUpSound;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TSubclassOf<UUserWidget> PickUpUIClass;
	
};
