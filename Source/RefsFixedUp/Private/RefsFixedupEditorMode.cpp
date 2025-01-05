// Copyright Epic Games, Inc. All Rights Reserved.

#include "RefsFixedUpEditorMode.h"
#include "RefsFixedUpEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "RefsFixedUpEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/RefsFixedUpSimpleTool.h"
#include "Tools/RefsFixedUpInteractiveTool.h"

// step 2: register a ToolBuilder in FRefsFixedUpEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "RefsFixedUpEditorMode"

const FEditorModeID URefsFixedUpEditorMode::EM_RefsFixedUpEditorModeId = TEXT("EM_RefsFixedUpEditorMode");

FString URefsFixedUpEditorMode::SimpleToolName = TEXT("RefsFixedUp_ActorInfoTool");
FString URefsFixedUpEditorMode::InteractiveToolName = TEXT("RefsFixedUp_MeasureDistanceTool");


URefsFixedUpEditorMode::URefsFixedUpEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(URefsFixedUpEditorMode::EM_RefsFixedUpEditorModeId,
		LOCTEXT("ModeName", "RefsFixedUp"),
		FSlateIcon(),
		true);
}


URefsFixedUpEditorMode::~URefsFixedUpEditorMode()
{
}


void URefsFixedUpEditorMode::ActorSelectionChangeNotify()
{
}

void URefsFixedUpEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FRefsFixedUpEditorModeCommands& SampleToolCommands = FRefsFixedUpEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<URefsFixedUpSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<URefsFixedUpInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void URefsFixedUpEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FRefsFixedUpEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> URefsFixedUpEditorMode::GetModeCommands() const
{
	return FRefsFixedUpEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
