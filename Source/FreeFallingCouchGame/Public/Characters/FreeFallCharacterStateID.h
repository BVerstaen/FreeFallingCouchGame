﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFreeFallCharacterStateID : uint8
{
	None = 0,
	Idle,
	Move,
	Dive,
	Grab,
	PowerUp,
	FastDive,
};

