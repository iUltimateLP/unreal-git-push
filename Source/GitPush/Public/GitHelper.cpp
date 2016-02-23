/** GitHelper **/
#include "GitPushPrivatePCH.h"

#include "GitHelper.h"

#include <string>
#include <iostream>
#include <cstdio>
#include <memory>

FString FGitHelper::ExecuteWindowsCommand(FString command)
{
	const char *cmd = TCHAR_TO_ANSI(*command);

	std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe)
	{
		return FString(TEXT("An error happend!"));
	}
	char buffer[128];
	std::string result = "";
	FString result2 = "";
	while (!feof(pipe.get()))
	{
		if (fgets(buffer, 128, pipe.get()) != NULL)
		{
			result += buffer;
		}
	}

	FString res(result.c_str());
	return res;
}

TArray<FString> FGitHelper::GetBranches()
{
	FString workingDir = FPaths::GetPath(FPaths::GetProjectFilePath());
	FString workingDrive = FString().AppendChar(workingDir.GetCharArray()[0]);

	//FString command = FString::Printf(TEXT("cd \"%s\" && git branch --list 2>&1"), *workingDir, *tempPath);
	FString command = FString::Printf(TEXT("%s: && cd \"%s\" && git branch --list 2>&1"), *workingDrive, *workingDir);
	FString result = ExecuteWindowsCommand(command);

	TArray<FString> outArray;
	result.ParseIntoArrayLines(outArray, true);
	for (int32 i = 0; i < outArray.Num(); i++)
	{
		outArray[i].RemoveAt(0, 2);
	}

	UE_LOG(LogWindows, Log, TEXT("GitPush: Got %d branches."), outArray.Num());

	return outArray;
}

TArray<FString> FGitHelper::GetRemoteHosts()
{
	FString workingDir = FPaths::GetPath(FPaths::GetProjectFilePath());
	FString workingDrive = FString().AppendChar(workingDir.GetCharArray()[0]);

	FString command = FString::Printf(TEXT("%s: && cd \"%s\" && git remote 2>&1"), *workingDrive, *workingDir);
	FString result = ExecuteWindowsCommand(command);

	TArray<FString> outArray;
	result.ParseIntoArrayLines(outArray, true);

	UE_LOG(LogWindows, Log, TEXT("GitPush: Got %d remote hosts"), outArray.Num());

	return outArray;
}

GitPushReturn FGitHelper::PushCommit(FString remoteHostName, FString destinationBranch)
{
	// git push source:dest --porcelain
	FString workingDir = FPaths::GetPath(FPaths::GetProjectFilePath());
	FString workingDrive = FString().AppendChar(workingDir.GetCharArray()[0]);

	FString command = FString::Printf(TEXT("%s: && cd \"%s\" && git push %s master:%s --porcelain 2>&1"), *workingDrive, *workingDir, *remoteHostName, *destinationBranch);
	FString result = FGitHelper::ExecuteWindowsCommand(command);

	GitPushReturn output;
	//Posible return stuff: "fatal", "error", "Everything up-to-date"
	output.bSuccessful = !(result.Contains(TEXT("fatal")) || result.Contains(TEXT("error")));
	output.consoleReturn = result;

	return output;
}

