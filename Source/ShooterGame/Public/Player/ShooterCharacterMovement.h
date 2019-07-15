// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

/**
 * Movement component meant for use with Pawns.
 */

#pragma once
#include "ShooterCharacterMovement.generated.h"

UCLASS()
class UShooterCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

	/** Distance to teleport ( in meters ) */
	UPROPERTY(Category = "Character Movement: Teleport ", EditAnywhere, BlueprintReadOnly)
	float DistanceToTeleport = 10.0f;
	
	virtual float GetMaxSpeed() const override;

	/* Do Teleport */
	virtual bool DoTeleport();

	/* If you look at the order of events for CharacterMovementComponent, you'll see that this function is the first thing
	 * called for both client and server when performing movement. We use it as a good place to capture the previous
	 * sprinting state. */
	virtual void PerformMovement(float DeltaSeconds) override;

	/* Read the state flags provided by CompressedFlags and trigger the ability on the server */
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	/* A necessary override to make sure that our custom FNetworkPredictionData defined below is used */
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

};
};

