// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// 매크로) 로그에 카테고리 선언
DECLARE_LOG_CATEGORY_EXTERN(CSW, Log, All);

// 매크로) 출력할 정보인 CALLINFO 와 PRING_LOG 
// __FUNCTION__ 함수명
// __LINE__ 몇 번째 줄
#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

#define PRINT_CALLINFO(Category) UE_LOG(Category, Warning, TEXT("%s"), *CALLINFO)

// fmt포멧을 만들어서 매크로 호출 시, 전달한 인자들이 해당 포멧에 사용됨.
#define PRINT_LOG(Category, fmt, ...) UE_LOG(Category, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))
