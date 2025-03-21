// Fill out your copyright notice in the Description page of Project Settings.


#include "Day_UI.h"
#include "Components/Button.h"

void UDay_UI::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Buy->OnClicked.AddDynamic( this , &UDay_UI::Buy );
	Button_Place->OnClicked.AddDynamic( this , &UDay_UI::Place );
}

void UDay_UI::Place()
{

}

void UDay_UI::Buy()
{

}
