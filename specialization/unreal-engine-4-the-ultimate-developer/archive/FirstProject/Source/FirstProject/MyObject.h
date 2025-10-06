// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FIRSTPROJECT_API UMyObject : public UObject
{
public:
	GENERATED_BODY()
	UMyObject();
	UPROPERTY(BlueprintReadWrite, Category = "MyVariables")
	float MyFloat;
	UFUNCTION(BlueprintCallable, Category = "MyFunctions")
	void MyFunction();
};
