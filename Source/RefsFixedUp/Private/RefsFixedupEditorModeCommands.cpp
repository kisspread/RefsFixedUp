// Copyright Epic Games, Inc. All Rights Reserved.

#include "RefsFixedUpEditorModeCommands.h"
#include "RefsFixedUpEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RefsFixedUpEditorModeCommands"

FRefsFixedUpEditorModeCommands::FRefsFixedUpEditorModeCommands()
	: TCommands<FRefsFixedUpEditorModeCommands>("RefsFixedUpEditorMode",
		NSLOCTEXT("RefsFixedUpEditorMode", "RefsFixedUpEditorModeCommands", "RefsFixedUp Editor Mode"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FRefsFixedUpEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FRefsFixedUpEditorModeCommands::GetCommands()
{
	return FRefsFixedUpEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
