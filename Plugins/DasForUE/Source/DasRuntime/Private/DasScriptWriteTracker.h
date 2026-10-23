#pragma once
#include "CoreMinimal.h"

// Hot reload without re-reading sources on every call: a platform-file layer on top of the chain notices every
// write, delete, move or copy of a .das file made in this process (FFileHelper, IFileManager, editors, tests) and
// queues it. The runtime takes the queue on the game thread and marks the sessions that read those files
// (FDasRuntimeModule::NotifyScriptsChanged); edits by other programs are caught by the poll interval instead.
void DasInstallScriptWriteTracker();
void DasRemoveScriptWriteTracker();
// Takes the queued writes: files, and directories deleted, copied or moved as a whole. False when nothing was
// queued (one atomic load). bAddedOrRemoved: a file appeared or disappeared (ScriptPath must look again).
bool DasTakeScriptWrites(TArray<FString>& Files, TArray<FString>& Directories, bool& bAddedOrRemoved);
