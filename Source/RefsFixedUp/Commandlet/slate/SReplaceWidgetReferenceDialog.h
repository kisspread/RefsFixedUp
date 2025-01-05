#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SSearchBox.h"

/** 表示一个需要替换的引用项 */
struct FReferenceReplaceItem
{
	/** 原始资产路径 */
	FString OriginalPath;

	/** 新的资产路径 */
	FString NewPath;

	/** 构造函数 */
	FReferenceReplaceItem(const FString& InOriginalPath)
		: OriginalPath(InOriginalPath)
	{
	}
};

typedef TSharedPtr<FReferenceReplaceItem> FReferenceReplaceItemPtr;

/**
 * 用于显示和替换无效Widget引用的对话框
 */
class SReplaceWidgetReferenceDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SReplaceWidgetReferenceDialog)
			: _InvalidReferencePaths()
		{
		}

		/** 需要替换的资产路径列表 */
		SLATE_ARGUMENT(TArray<FString>, InvalidReferencePaths)
	SLATE_END_ARGS()

	/** 构造函数 */
	SReplaceWidgetReferenceDialog()
		: PathRedirectsPtr(nullptr)
	{
	}

	/** 构造函数 */
	void Construct(const FArguments& InArgs);

	/** 显示对话框 */
	static bool ShowDialog(const TArray<FString>& InvalidReferencePaths, TMap<FString, FString>& OutPathRedirects);

private:
	/** 创建列表视图 */
	TSharedRef<SWidget> MakePathListView();

	/** 创建资产选择器 */
	TSharedRef<SWidget> MakeAssetPicker(const FReferenceReplaceItemPtr& Item);

	/** 获取列表项文本 */
	FText GetItemText(FReferenceReplaceItemPtr Item) const;

	/** 处理资产选择 */
	void OnAssetSelected(const FAssetData& AssetData, FReferenceReplaceItemPtr Item);

	/** 处理确认按钮点击 */
	FReply OnConfirmClicked();

	/** 处理取消按钮点击 */
	FReply OnCancelClicked();

	/** 检查是否可以确认 */
	bool CanConfirm() const;

private:
	/** 列表视图 */
	TSharedPtr<SListView<FReferenceReplaceItemPtr>> ListView;

	/** 列表项 */
	TArray<FReferenceReplaceItemPtr> ListItems;

	/** 输出的路径重定向映射 */
	TMap<FString, FString>* PathRedirectsPtr;

	/** 对话框窗口 */
	TSharedPtr<SWindow> DialogWindow;
};
