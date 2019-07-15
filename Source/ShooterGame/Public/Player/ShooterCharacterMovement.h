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

/*
 */

class FSavedMove_ShooterCharacter : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	/** These flags are used to replicate teleport via the compressed flags*/
	bool bPressedTeleport;

	/* Sets the default values for the saved move */
	virtual void Clear() override;

	/* Packs state data into a set of compressed flags. This is undone above in UpdateFromCompressedFlags */
	virtual uint8 GetCompressedFlags() const override;

	/* Checks if an old move can be combined with a new move for replication purposes (are they different or the same) */
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	/* Populates the FSavedMove fields from the corresponding character movement controller variables. This is used when
	 * making a new SavedMove and the data will be used when playing back saved moves in the event that a correction needs to happen.*/
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

	/* This is used when the server plays back our saved move(s). This basically does the exact opposite of what
	 * SetMoveFor does. Here we set the character movement controller variables from the data in FSavedMove. */
	virtual void PrepMoveFor(class ACharacter* Character) override;
};

/*
 * This subclass of FNetworkPredictionData_Client_Character is used to create new copies of
 * our custom FSavedMove_ShooterCharacter class defined above.
 */

class FNetworkPredictionData_Client_ShooterCharacter : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	FNetworkPredictionData_Client_ShooterCharacter(const UShooterCharacterMovement& CharacterMovement);
	~FNetworkPredictionData_Client_ShooterCharacter();

	/* Allocates a new copy of our custom saved move */
	virtual FSavedMovePtr AllocateNewMove() override;
};

