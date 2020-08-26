// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameModeBase.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include  "ToonTanks/Pawns/PawnTurret.h"
#include "Kismet/GameplayStatics.h"
#include  "ToonTanks/PlayerControllers/PlayerControllerBase.h"
#include "ToonTanks/Pawns/AI/STrackerBot.h"
#include "ToonTanks/GameWidget.h"





void ATankGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ChangeMenuWidget(StartingWidgetClass);

	((UGameWidget*)CurrentWidget)->Load();
	((UGameWidget*)CurrentWidget)->SetText(Score);



	HandleGameStart();
}

void ATankGameModeBase::ActorDied(AActor * DeadActor)
{

	if (DeadActor == PlayerTank)
	{
		PlayerTank->HandleDestruction();
		HandleGameOver(false);

		if (PlayerControllerRef)
		{
			PlayerControllerRef->SetPlayerEnabledState(false);
		}

	 }
	else if (APawnTurret* DestroyedTurret = Cast<APawnTurret>(DeadActor))
	{
		DestroyedTurret->HandleDestruction();
		ChangeScore(50);

		if (--TargetTurrets == 0)
		{
			HandleGameOver(true);
		}
	}
	else if (ASTrackerBot* TrackerBot = Cast<ASTrackerBot>(DeadActor))
	{
		//TrackerBot->HandleDestructionTracker();
		UE_LOG(LogTemp, Warning, TEXT("TrackerDestroyed"));
		ChangeScore(150);

	}

}

void ATankGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);

		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void ATankGameModeBase::ChangeScore(int value)
{
	Score += value;
	((UGameWidget*)CurrentWidget)->SetText(Score);

}


int32 ATankGameModeBase::GetTargetTurretCount()
{
	TArray<AActor*> TurretActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawnTurret::StaticClass(), TurretActors);

	return TurretActors.Num();
}

void ATankGameModeBase::HandleGameStart()
{

	TargetTurrets = GetTargetTurretCount();

	PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	PlayerControllerRef = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));

	GameStart();

	if (PlayerControllerRef)
	{
		PlayerControllerRef->SetPlayerEnabledState(false);

		FTimerHandle PlayerEnableHandle;
		FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerControllerRef, &APlayerControllerBase::SetPlayerEnabledState, true);

		GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, StartDelay, false);
	}
}

void ATankGameModeBase::HandleGameOver(bool PlayerWon)
{
	GameOver(PlayerWon);
}
