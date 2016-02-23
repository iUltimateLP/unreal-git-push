#pragma once
#include "GitPushPrivatePCH.h"

#include "GitPush.h"

struct GitPushReturn
{
	bool bSuccessful;
	FString consoleReturn;
};

class FGitHelper
{
public:
	/** Execute Windows Shell command **/
	static FString ExecuteWindowsCommand(FString command);
	/** Get all branches of the current repo **/
	static TArray<FString> GetBranches();
	/** Get all remote hosts of the current repo **/
	static TArray<FString> GetRemoteHosts();
	/** Check if a folder is a git repo **/
	static bool IsGitRepo(FString path);
	/** Push Git Commit to repo **/
	static GitPushReturn PushCommit(FString remoteHostName, FString destinationBranch);
};