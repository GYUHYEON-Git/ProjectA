// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define COLLISION_OBJECT_INTERACTION ECC_GameTraceChannel1
#define COLLISION_OBJECT_TARGETING ECC_GameTraceChannel2

UENUM(BlueprintType)
enum class EAttributeType : uint8 {
	Stamina,
	Health,
};

UENUM(BlueprintType)
enum class EHitDirection : uint8 {
    Front,
    Back,
    Left,
    Right,
};

UENUM(BlueprintType)
enum class ESwitchingDirection : uint8 {
    None,
    Left,
    Right,
};

};