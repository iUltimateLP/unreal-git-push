// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

#include "GitHelper.h"

class FToolBarBuilder;
class FMenuBuilder;

class FGitPushModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Function to generate all the Content of the Toolbar dropdown **/
	TSharedRef<SWidget> ToolbarContent();

	/** All branches - collected at begin of editor startup **/
	TArray<FString> branches;

	/** All remote hosts - also collected at begin of editor startup **/
	TArray<FString> remoteHosts;

	/**  **/
	void PopulateRemoteEntries(FMenuBuilder& MenuBuilder, FString branch);

	/** Remote Entry was clicked -> PUSH **/
	void RemoteEntryClicked(FString branch, FString host);

	/** Reload plugin **/
	void ReInitPlugin();

private:

	void AddToolbarExtension(FToolBarBuilder& Builder);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};