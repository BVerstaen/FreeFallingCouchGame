// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EDiveLayersID : uint8
{
	None = 0,
	Bottom,
	Middle,
	Top
};

UENUM(BlueprintType)
enum class EDiveLayerBoundsID : uint8
{
	Middle = 0,
	Up,
	Down,
};
