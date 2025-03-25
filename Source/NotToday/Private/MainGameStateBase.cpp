// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameStateBase.h"

void AMainGameStateBase::SetDayNightState(EDayNightState NewState)
{
	if (State == NewState) return;
	
	State = NewState;

	// GameState클래스가 아닌 부분에서는
	// OnDayNightChanged.AddDynamic을 한다!!
	OnDayNightChanged.Broadcast(NewState);

	// GameState 클래스 내부 처리
	if (NewState == EDayNightState::Day)
	{
		// 낮
		
	}
	else
	{
		// 밤

		// a) 현재 웨이브의 
	}
}

void AMainGameStateBase::CheckAllKill()
{
	
}
