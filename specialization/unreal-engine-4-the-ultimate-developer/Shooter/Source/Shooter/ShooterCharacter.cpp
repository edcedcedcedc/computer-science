// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("void AShooterCharacter::BeginPlay()"));

	int myInt{ 42 };
	UE_LOG(LogTemp, Warning, TEXT("My int is: %d"), myInt);

	FString myString{ TEXT("This is a string") };
	UE_LOG(LogTemp, Warning, TEXT("My string is: %s"), *myString);
	
	UE_LOG(LogTemp, Warning, TEXT("The name of this actor is: %s"), *GetName());
}
	

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

