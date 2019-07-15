// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Player/ShooterCharacterMovement.h"
#include "ShooterAIController.h"

//----------------------------------------------------------------------//
// UPawnMovementComponent
//----------------------------------------------------------------------//
UShooterCharacterMovement::UShooterCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


float UShooterCharacterMovement::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const AShooterCharacter* ShooterCharacterOwner = Cast<AShooterCharacter>(PawnOwner);
	if (ShooterCharacterOwner)
	{
		if (ShooterCharacterOwner->IsTargeting())
		{
			MaxSpeed *= ShooterCharacterOwner->GetTargetingSpeedModifier();
		}
		if (ShooterCharacterOwner->IsRunning())
		{
			MaxSpeed *= ShooterCharacterOwner->GetRunningSpeedModifier();
		}
	}

	return MaxSpeed;
}

bool UShooterCharacterMovement::DoTeleport()
{
	AShooterPlayerController* PC = Cast<AShooterPlayerController>(CharacterOwner->Controller);

	bool bWasTeleporting = false;

	if (PC)
	{
		/* Necessary variables to execute the LineTraceSingleByChannel.
		 * @StartTrace : Ray starting point.
		 * @StartTrace : Ray ending point, only when a collision is not detected otherwise we will use hit.ImpactPoint.
		 * @CameraRotation: camera rotation, in this case we are going to use PlayerController (GetPlayerViewPoint) to get this value.
		 * @ForwardVector : direction that is straight in front of the actor
		 */

		FVector StartTrace = FVector::ZeroVector;
		FVector EndTrace = FVector::ZeroVector;
		FVector ForwardVector = FVector::ZeroVector;
		FRotator CameraRotation = FRotator::ZeroRotator;
		PC->GetPlayerViewPoint(StartTrace, CameraRotation);
		ForwardVector = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::X);

		/*DistanceToTeleport in meters.*/
		EndTrace = StartTrace + (ForwardVector * DistanceToTeleport * 100.0f);

		FCollisionQueryParams TraceParams(TEXT("TeleportTraceParams"), true);

		/* ignore the owner to avoid problems in the calculation when the server performs this action */
		TraceParams.AddIgnoredActor(CharacterOwner);
		FHitResult Hit(ForceInit);

		//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, 10.f, ECC_WorldStatic, 1.f);
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECollisionChannel::ECC_Camera, TraceParams);

		/* and finally we see if there is a collision or not before executing the native teleport function.*/

		if (bHit)
		{
			bWasTeleporting = CharacterOwner->TeleportTo(Hit.ImpactPoint, CharacterOwner->GetActorRotation());
			//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Red, false, 10.f, ECC_WorldStatic, 1.f);
		}
		else
		{
			bWasTeleporting = CharacterOwner->TeleportTo(EndTrace, CharacterOwner->GetActorRotation());
		}
	}

	return bWasTeleporting;
}

void UShooterCharacterMovement::PerformMovement(float DeltaSeconds)
{
	AShooterCharacter* ShooterCharacterOwner = Cast<AShooterCharacter>(PawnOwner);
	ShooterCharacterOwner->CheckTeleportinput();

	Super::PerformMovement(DeltaSeconds);
}

void UShooterCharacterMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	AShooterCharacter* ShooterCharacterOwner = Cast<AShooterCharacter>(PawnOwner);

	if (ShooterCharacterOwner)
	{
		ShooterCharacterOwner->bPressedTeleport = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);

		if (!ShooterCharacterOwner->bPressedTeleport)
		{
			ShooterCharacterOwner->bWasTeleporting = false;
		}

		ShooterCharacterOwner->CheckTeleportinput();
	}
}

