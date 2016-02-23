// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "GitPushPrivatePCH.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "GitPushStyle.h"
#include "GitPushCommands.h"

#include "LevelEditor.h"

#include <string>
#include <iostream>
#include <cstdio>
#include <memory>

static const FName GitPushTabName("GitPush");

#define LOCTEXT_NAMESPACE "FGitPushModule"

FString currentRemoteHost = "";

#pragma region Module Stuff

void FGitPushModule::StartupModule()
{
	FGitPushStyle::Initialize();
	FGitPushStyle::ReloadTextures();

	FGitPushCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FGitPushCommands::Get().PluginAction,
		NULL,
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FGitPushModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	//Initially collect branches + remote hosts
	branches = FGitHelper::GetBranches();
	remoteHosts = FGitHelper::GetRemoteHosts();
}

void FGitPushModule::ShutdownModule()
{
	FGitPushStyle::Shutdown();
	FGitPushCommands::Unregister();
}

#pragma endregion

#pragma region Toolbar Stuff

void FGitPushModule::PopulateRemoteEntries(FMenuBuilder& MenuBuilder, FString branch)
{	
	MenuBuilder.BeginSection("GitPushRemote", LOCTEXT("GitPushSelectRemoteTitle", "Select remote host"));

	for (int32 i = 0; i < remoteHosts.Num(); i++)
	{
		MenuBuilder.AddMenuEntry(
			FText::Format(LOCTEXT("GitPushRemoteLabel", "Host: {0}"), FText::FromString(remoteHosts[i])),
			FText::Format(LOCTEXT("GitPushRemoteTooltip", "Push the commit from branch {0} to remote host {1}"), FText::FromString(branch), FText::FromString(remoteHosts[i])),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FGitPushModule::RemoteEntryClicked, branch, remoteHosts[i])),
			NAME_None,
			EUserInterfaceActionType::Button,
			NAME_None
			);
	}

	MenuBuilder.EndSection();
}

void FGitPushModule::RemoteEntryClicked(FString branch, FString host)
{
	UE_LOG(LogWindows, Log, TEXT("Pushing latest commit from %s to %s"), *branch, *host);
	GitPushReturn result = FGitHelper::PushCommit(host, branch);

	FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("GitPushPushedMessage", "{0}\n\nGit Output:\n{1}"),
		(result.bSuccessful ? LOCTEXT("GitPushPushSuccess", "The commit was succesfully pushed!") : LOCTEXT("GitPushPushFail", "The commit could not be pushed :(")),
		FText::FromString(result.consoleReturn)));
}

TSharedRef<SWidget> FGitPushModule::ToolbarContent()
{
	TSharedPtr<FUICommandList> CommandList;

	const bool bClose = true;
	FMenuBuilder MenuBuilder = FMenuBuilder(bClose, CommandList);

	MenuBuilder.BeginSection("GitPush", LOCTEXT("GitPushBranches", "Select Branch"));

	if (branches.Num() != 0)
	{
		for (int32 i = 0; i < branches.Num(); i++)
		{
			FSlateIcon icon = (i == branches.Num() - 1) ? FSlateIcon("GitPushStyle", "GitPush.MasterBranch") : FSlateIcon("GitPushStyle", "GitPush.OtherBranch");

			MenuBuilder.AddSubMenu(
				FText::Format(LOCTEXT("GitPushBranchesLabel", "Branch: {0}"), FText::FromString(branches[i])),
				FText::Format(LOCTEXT("GitPushBranchesTooltip", "Push the commit to {0}"), FText::FromString(branches[i])),
				FNewMenuDelegate::CreateRaw(this, &FGitPushModule::PopulateRemoteEntries, branches[i]),
				false,
				icon);
		}
	}
	else
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("GitPushBranchesError", "Error: No branches or no git repo found!"),
			LOCTEXT("GitPushBranchesError", "Error: No branches or no git repo found!"),
			FSlateIcon("GitPushStyle", "GitPush.ErrorIcon"),
			FUIAction(),
			NAME_None,
			EUserInterfaceActionType::Button,
			NAME_None);
	}

	FSlateIcon icon = FSlateIcon("GitPushStyle", "GitPush.MasterBranch");
	//MenuBuilder.AddSubMenu(LOCTEXT("GitPushBranchesSubMenuLabel", "Test"), LOCTEXT("GitPushBranchesSubMenuDesc", "test"), NULL, false, icon);

	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void FGitPushModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	FSlateIcon icon = FSlateIcon("GitPushStyle", "GitPush.PluginAction");
	Builder.AddComboButton(FUIAction(), FOnGetContent::CreateRaw(this, &FGitPushModule::ToolbarContent), LOCTEXT("GitPushButton", "GitPush"), LOCTEXT("GitPushButtonToolip", "Opens GitPush"), icon);
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGitPushModule, GitPush)