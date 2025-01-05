// Copyright Epic Games, Inc. All Rights Reserved.

#include "RefsFixedUpModule.h"

#include "RefsFixedUpEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "RefsFixedUpModule"

void FRefsFixedUpModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FRefsFixedUpEditorModeCommands::Register();
}

void FRefsFixedUpModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FRefsFixedUpEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRefsFixedUpModule, RefsFixedUp)