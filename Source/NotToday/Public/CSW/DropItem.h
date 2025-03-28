// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropItem.generated.h"

class USphereComponent;

UCLASS()
class NOTTODAY_API ADropItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADropItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 180.f;

	UPROPERTY(EditAnywhere)
	FVector MeshLocationOffset;

	void Rotate(float DeltaTime);

	
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
