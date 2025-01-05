#include "SReplaceWidgetReferenceDialog.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#define LOCTEXT_NAMESPACE "ReplaceWidgetReferenceDialog"

void SReplaceWidgetReferenceDialog::Construct(const FArguments& InArgs)
{
    // 创建列表项
    for (const FString& Path : InArgs._InvalidReferencePaths)
    {
        ListItems.Add(MakeShared<FReferenceReplaceItem>(Path));
    }

    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
        .Padding(FMargin(8.0f))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 0, 0, 8)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("ReplaceWidgetReference_Description", "选择要替换的资产:"))
                .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
            ]

            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                MakePathListView()
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 8, 0, 0)
            .HAlign(HAlign_Right)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(0, 0, 8, 0)
                [
                    SNew(SButton)
                    .Text(LOCTEXT("ReplaceWidgetReference_Confirm", "确认"))
                    .IsEnabled(this, &SReplaceWidgetReferenceDialog::CanConfirm)
                    .OnClicked(this, &SReplaceWidgetReferenceDialog::OnConfirmClicked)
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SButton)
                    .Text(LOCTEXT("ReplaceWidgetReference_Cancel", "取消"))
                    .OnClicked(this, &SReplaceWidgetReferenceDialog::OnCancelClicked)
                ]
            ]
        ]
    ];
}

bool SReplaceWidgetReferenceDialog::ShowDialog(const TArray<FString>& InvalidReferencePaths, TMap<FString, FString>& OutPathRedirects)
{
    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(LOCTEXT("ReplaceWidgetReference_Title", "替换Widget引用"))
        .SizingRule(ESizingRule::UserSized)
        .ClientSize(FVector2D(800, 400))
        .SupportsMaximize(false)
        .SupportsMinimize(false);

    TSharedRef<SReplaceWidgetReferenceDialog> Dialog = SNew(SReplaceWidgetReferenceDialog)
        .InvalidReferencePaths(InvalidReferencePaths);
    Dialog->PathRedirectsPtr = &OutPathRedirects;
    Dialog->DialogWindow = Window;

    Window->SetContent(Dialog);

    GEditor->EditorAddModalWindow(Window);

    return Dialog->PathRedirectsPtr->Num() > 0;
}

TSharedRef<SWidget> SReplaceWidgetReferenceDialog::MakePathListView()
{
    ListView = SNew(SListView<FReferenceReplaceItemPtr>)
        // .ItemHeight(24)
        .ListItemsSource(&ListItems)
        .SelectionMode(ESelectionMode::None)
        .OnGenerateRow_Lambda([this](FReferenceReplaceItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable) {
            return SNew(STableRow<FReferenceReplaceItemPtr>, OwnerTable)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(0.4f)
                .VAlign(VAlign_Center)
                .Padding(4, 0)
                [
                    SNew(STextBlock)
                    .Text(this, &SReplaceWidgetReferenceDialog::GetItemText, Item)
                ]
                + SHorizontalBox::Slot()
                .FillWidth(0.6f)
                .Padding(4, 0)
                [
                    MakeAssetPicker(Item)
                ]
            ];
        });

    return ListView.ToSharedRef();
}

TSharedRef<SWidget> SReplaceWidgetReferenceDialog::MakeAssetPicker(const FReferenceReplaceItemPtr& Item)
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

    FAssetPickerConfig Config;
    Config.SelectionMode = ESelectionMode::Single;
    Config.InitialAssetViewType = EAssetViewType::List;
    Config.bFocusSearchBoxWhenOpened = true;
    Config.bAllowNullSelection = false;
    Config.bShowBottomToolbar = false;
    Config.bAutohideSearchBar = false;
    Config.bShowPathInColumnView = true;
    Config.bShowTypeInColumnView = true;
    Config.bSortByPathInColumnView = true;

    // 只显示 Widget 蓝图
    // Config.Filter.ClassPaths.Add(UWidgetBlueprint::StaticClass()->GetClassPathName());

    Config.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SReplaceWidgetReferenceDialog::OnAssetSelected, Item);

    return SNew(SBox)
        .HeightOverride(300)
        [
            ContentBrowserModule.Get().CreateAssetPicker(Config)
        ];
}

FText SReplaceWidgetReferenceDialog::GetItemText(FReferenceReplaceItemPtr Item) const
{
    return FText::FromString(Item->OriginalPath);
}

void SReplaceWidgetReferenceDialog::OnAssetSelected(const FAssetData& AssetData, FReferenceReplaceItemPtr Item)
{
    if (AssetData.IsValid())
    {
        Item->NewPath = AssetData.GetObjectPathString();
    }
}

FReply SReplaceWidgetReferenceDialog::OnConfirmClicked()
{
    if (PathRedirectsPtr)
    {
        for (const auto& Item : ListItems)
        {
            if (!Item->NewPath.IsEmpty())
            {
                PathRedirectsPtr->Add(Item->OriginalPath, Item->NewPath);
            }
        }
    }

    if (DialogWindow.IsValid())
    {
        DialogWindow->RequestDestroyWindow();
    }

    return FReply::Handled();
}

FReply SReplaceWidgetReferenceDialog::OnCancelClicked()
{
    if (DialogWindow.IsValid())
    {
        DialogWindow->RequestDestroyWindow();
    }

    return FReply::Handled();
}

bool SReplaceWidgetReferenceDialog::CanConfirm() const
{
    // 至少有一个项目选择了新的资产
    return ListItems.ContainsByPredicate([](const FReferenceReplaceItemPtr& Item) {
        return !Item->NewPath.IsEmpty();
    });
}

#undef LOCTEXT_NAMESPACE
