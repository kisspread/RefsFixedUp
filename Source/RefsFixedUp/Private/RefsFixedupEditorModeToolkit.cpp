// Copyright Epic Games, Inc. All Rights Reserved.

#include "RefsFixedUpEditorModeToolkit.h"
#include "RefsFixedUpEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "RefsFixedUpEditorModeToolkit"

FRefsFixedUpEditorModeToolkit::FRefsFixedUpEditorModeToolkit()
{
}

void FRefsFixedUpEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FRefsFixedUpEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FRefsFixedUpEditorModeToolkit::GetToolkitFName() const
{
	return FName("RefsFixedUpEditorMode");
}

FText FRefsFixedUpEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "RefsFixedUpEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
