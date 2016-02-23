// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "GitPushStyle.h"

class FGitPushCommands : public TCommands<FGitPushCommands>
{
public:

	FGitPushCommands()
		: TCommands<FGitPushCommands>(TEXT("GitPush"), NSLOCTEXT("Contexts", "GitPush", "GitPush Plugin"), NAME_None, FGitPushStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
