#pragma once

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
	/** Push Git Commit to repo **/
	static GitPushReturn PushCommit(FString remoteHostName, FString destinationBranch);
};