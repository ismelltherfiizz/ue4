// Fill out your copyright notice in the Description page of Project Settings.


#include "BallonDefault.h"
#include <Components/InputComponent.h>
#include "GameFramework/Controller.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Fire.h"
#include <PaperBox.h>
#include "Engine/World.h"
#include <UserWidget.h>
#include <Chest.h>
#include "GasCanister.h"
#include <GameFramework/PlayerController.h>
#include <Kismet/GameplayStatics.h>

ABalloonDefault::ABalloonDefault()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	ChestsCollectedCount = 0;

	BalloonFire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BalloonFire"));
	BalloonFire->SetupAttachment(VisibleComponent);

	CameraSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpring"));
	CameraSpring->SetupAttachment(RootComponent);
	CameraSpring->bUsePawnControlRotation = true; 

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Ñamera"));
	Camera->SetupAttachment(CameraSpring, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void ABalloonDefault::BeginPlay()
{
	Super::BeginPlay();

	CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass)->AddToViewport();

	WriteMessageToScreen("Collect 3 chests!");

	OnActorBeginOverlap.AddUniqueDynamic(this, &ABalloonDefault::OnOverlapBegin);

}

void ABalloonDefault::Move_XAxis(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void ABalloonDefault::Move_YAxis(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);
	}
}

void ABalloonDefault::JumpStart()
{
	if (GasOutburstCount > 0)
	{
		GasOutburstCount--;
		bIsJumping = true;
		BalloonFire->Activate();
		GetWorldTimerManager().SetTimer(JumpTimerHandle, this, &ABalloonDefault::JumpEnd, 1.0f, false);
	}
	else
	{
		WriteMessageToScreen("No gas left!");
	}
}

void ABalloonDefault::JumpEnd()
{
	BalloonFire->Deactivate();
	bIsJumping = false;
}



void ABalloonDefault::AllowMovement()
{
	WriteMessageToScreen("Now you can move!");
	GetMovementComponent()->Activate(true);
}

void ABalloonDefault::OnWin()
{
	MovementComponent->Deactivate();
	BalloonFire->Activate();
	InputComponent->ClearActionBindings();
	bIsJumping = true;
	GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->bShowMouseCursor = true;
	CreateWidget<UUserWidget>(GetWorld(), WinMenuWidgetClass)->AddToViewport();
}

void ABalloonDefault::CleanMessageToScreen()
{
	MessageToScreen = "";
}

void ABalloonDefault::WriteMessageToScreen(FString Message)
{
	MessageToScreen = Message;
	GetWorldTimerManager().SetTimer(MessageTimerHandle, this, &ABalloonDefault::CleanMessageToScreen, 2.0f, false);
}

void ABalloonDefault::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor->IsA<AChest>())
	{
		OtherActor->Destroy();
		if (++ChestsCollectedCount == ChestsToWin)
		{
			OnWin();
		}
	}

	if (OtherActor->IsA<AGasCanister>())
	{
		GasOutburstCount++;
		OtherActor->Destroy();
	}

	if (OtherActor->IsA<AFire>())
	{
		Destroy();
	}
	if (OtherActor->IsA<APaperBox>())
	{
		GetMovementComponent()->Deactivate();
		GetWorldTimerManager().SetTimer(AllowMovementTimerHandle, this, &ABalloonDefault::AllowMovement, 3.0f, false);
		OtherActor->Destroy();
		WriteMessageToScreen("You're disabled!");
	}
}

void ABalloonDefault::Destroyed()
{
	GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->bShowMouseCursor = true;
	CreateWidget<UUserWidget>(GetWorld(), DeathMenuWidgetClass)->AddToViewport();
}

void ABalloonDefault::Tick(float DeltaTime)
{
	FRotator NewRotation = GetActorRotation();
	FVector NewLocation = GetActorLocation();

	NewLocation.Z -= DeltaTime * 200.0f;
	float DeltaRotation = DeltaTime * 20.0f;
	NewRotation.Yaw += DeltaRotation;

	SetActorLocationAndRotation(NewLocation, NewRotation, true, nullptr, ETeleportType::None);

	if (bIsJumping)
	{
		NewLocation.Z += DeltaTime * 500.0f;
		DeltaRotation = DeltaTime * 40.0f;
		NewRotation.Yaw += DeltaRotation;

		SetActorLocationAndRotation(NewLocation, NewRotation, true, nullptr, ETeleportType::None);
			
	}
}

// Called to bind functionality to input
void ABalloonDefault::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ABalloonDefault::JumpStart);
	InputComponent->BindAxis("MoveX", this, &ABalloonDefault::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &ABalloonDefault::Move_YAxis);
	InputComponent->BindAxis("Turn", this, &ADefaultPawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ADefaultPawn::AddControllerPitchInput);
}