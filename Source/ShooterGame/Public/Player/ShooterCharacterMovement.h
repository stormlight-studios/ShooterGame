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

	/* Distance (in meters) that a Teleport travels */
	UPROPERTY(Category = "Character Movement: Teleport ", EditAnywhere, BlueprintReadOnly)
	float DistanceToTeleport = 10.0f;
	
	virtual float GetMaxSpeed() const override;

	/* Executes the Teleport ability */
	virtual bool DoTeleport();

	/* This override extends UCharacterMovementComponent::PerformMovement to include the triggering of our Teleport ability */
	virtual void PerformMovement(float DeltaSeconds) override;

	/* Unpack and process flags from an FSavedMove_ShooterCharacter so that the server knows to trigger the Teleport ability */
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	/* Required so that FNetworkPredictionData_Client objects used are of our FNetworkPredictionData_Client_ShooterCharacter subclass */
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

};

/* This subclass extends FSavedMove_Character so that it includes and handles data related to the Teleport ability */
class FSavedMove_ShooterCharacter : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	/** This flag is used to replicate a Teleport via the compressed flags */
	bool bPressedTeleport;

	/* Sets the default values for the saved move */
	virtual void Clear() override;

	/* Packs state data into a set of compressed flags. This is undone in UpdateFromCompressedFlags */
	virtual uint8 GetCompressedFlags() const override;

	/* Checks if this move can be combined with a new one when replicating and keeping the same results */
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	/* Populates the FSavedMove fields from the corresponding character movement controller variables. This is used when
	 * making a new SavedMove and the data will be used when playing back saved moves in the event that a correction needs to happen.*/
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

	/* This is used when the server plays back our saved move(s). This basically does the exact opposite of what
	 * SetMoveFor does. Here we set the character movement controller variables from the data in FSavedMove. */
	virtual void PrepMoveFor(class ACharacter* Character) override;
};

/*
 * This subclass of FNetworkPredictionData_Client_Character is used to override the AllocateNewMove method,
 * so that the moves replicated to the server use our FSavedMove_ShooterCharacter class.
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

