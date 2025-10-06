// Fill out your copyright notice in the Description page of Project Settings.


#include "MyObject.h"

UMyObject::UMyObject()
{
	MyFloat = 0.0f;
}
void UMyObject::MyFunction() 
{
	UE_LOG(LogTemp, Warning, TEXT("TEXT"));
}