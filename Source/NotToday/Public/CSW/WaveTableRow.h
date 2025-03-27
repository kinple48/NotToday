#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ZombieBase.h"
#include "WaveTableRow.generated.h"

USTRUCT()
struct FWaveTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 웨이브 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	FName ID;
	
	// 좀비 종류 및 개수를 문자열로 입력 (예: "Zombie1:5, Zombie2:3")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	FString Zombies;
	
	// 최대 돈
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 Money = 0;
	
	
	TArray<TPair<TSubclassOf<AZombieBase>, int32>> GetParsedZombies() const;
};
