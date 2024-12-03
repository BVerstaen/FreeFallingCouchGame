// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/GameDataInstanceSubsystem.h"

const int* UGameDataInstanceSubsystem::GetPlayerIDFromScore(int Score)
{
	return PlayerScoreFromID.FindKey(Score);
}

int UGameDataInstanceSubsystem::GetPlayerScoreFromID(int PlayerID)
{
	if (PlayerScoreFromID.Contains(PlayerID))
	{
		return PlayerScoreFromID[PlayerID];
	}
	return -1;
}

void UGameDataInstanceSubsystem::SetPlayerScoreFromID(int PlayerID, int PlayerScore)
{
	if (!PlayerScoreFromID.Contains(PlayerID))
	{
		PlayerScoreFromID.Add(PlayerID);
	}
	PlayerScoreFromID[PlayerID] = PlayerScore;
}

void UGameDataInstanceSubsystem::AddPlayerScoreFromID(int PlayerID, int PointsToAdd)
{
	if (!PlayerScoreFromID.Contains(PlayerID)) return;
	PlayerScoreFromID[PlayerID] += PointsToAdd;
}

void UGameDataInstanceSubsystem::ResetPlayerScore()
{
	PlayerScoreFromID.Empty();
}
