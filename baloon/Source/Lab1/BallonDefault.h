// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include "BallonDefault.generated.h"

/**
 * 
 */
UCLASS()
class LAB1_API ABalloonDefault : public ADefaultPawn
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	FVector Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraSpring;

	FTimerHandle JumpTimerHandle;
	FTimerHandle AllowMovementTimerHandle;
	FTimerHandle MessageTimerHandle;

	bool bIsJumping;

	FVector JumpStartLocation;

public:

	ABalloonDefault();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* BalloonFire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Screen)
	int ChestsToWin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Screen)
	int GasOutburstCount;

	UPROPERTY(BlueprintReadOnly, Category = Screen)
	int ChestsCollectedCount;

	UPROPERTY(BlueprintReadOnly, Category = Screen)
	FString MessageToScreen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TSubclassOf<UUserWidget> DeathMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TSubclassOf<UUserWidget> WinMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TSubclassOf<UUserWidget> HUDWidgetClass;




protected:
	virtual void BeginPlay() override;

	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void JumpStart();
	void JumpEnd();
	void AllowMovement();
	
	void OnWin();

	void CleanMessageToScreen();
	void WriteMessageToScreen(FString Message);

	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void Destroyed() override;

	UPROPERTY(EditAnywhere)
	USceneComponent* VisibleComponent;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

};
