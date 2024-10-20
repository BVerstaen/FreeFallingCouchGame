// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayer/Public/LocalMultiplayerSettings.h"

bool FLocalMultiplayerProfileData::ContainsKey(const FKey& Key, ELocalMultiplayerInputMappingType MappingType) const
{
	UInputMappingContext* CurrentMappingContext = GetIMCFromType(MappingType);
	if (CurrentMappingContext != nullptr)	
	{
		TArray<FEnhancedActionKeyMapping> MappingsList = CurrentMappingContext->GetMappings();

		for(FEnhancedActionKeyMapping Mapping : MappingsList)
		{
			if (Mapping.Key == Key)
				return true;
		}
	}
	
	return false;
}

UInputMappingContext* FLocalMultiplayerProfileData::GetIMCFromType(ELocalMultiplayerInputMappingType MappingType) const
{
	switch (MappingType)
	{
	case InGame:
		return IMCInGame;

	case Menu:
		return IMCMenu;

	default:
			return nullptr;
	};
}

int ULocalMultiplayerSettings::GetNbKeyboardProfiles() const
{
	return KeyboardProfilesData.Num();
}

int ULocalMultiplayerSettings::FindKeyboardProfileIndexFromKey(const FKey& Key, ELocalMultiplayerInputMappingType MappingType) const
{
	for(int i = 0; i < KeyboardProfilesData.Num(); i++)
	{
		if (KeyboardProfilesData[i].ContainsKey(Key, MappingType))
			return i;
	}
	
	return -1;
}
