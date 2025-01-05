// Copy Right ZeroSoul
#include "YMythEdTools.h"

#include "AssetHeaderPatcher.h"
#include "AssetManagerEditorModule.h"
#include "FileHelpers.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "slate/SReplaceWidgetReferenceDialog.h"

#define LOCTEXT_NAMESPACE "FYMythEditorModule"

bool UYMythEdTools::AnalyzeAssetReferences(const FString& AssetPath, FAssetReferenceInfo& OutReferenceInfo)
{
	OutReferenceInfo = FAssetReferenceInfo(AssetPath);

	// 验证资产路径
	if (!ValidateAssetPath(AssetPath))
	{
		OutReferenceInfo.ValidationErrors.Add(FText::Format(
			FText::AsLocalizable_Advanced(TEXT(LOCTEXT_NAMESPACE), TEXT("InvalidAssetPath"), TEXT("Invalid asset path: {0}")),
			FText::FromString(AssetPath)));
		return false;
	}

	// 加载资产
	UObject* Asset = LoadAsset(AssetPath);
	if (!Asset)
	{
		OutReferenceInfo.ValidationErrors.Add(FText::Format(
			LOCTEXT("FailedToLoadAsset", "Failed to load asset: {0}"),
			FText::FromString(AssetPath)));
		return false;
	}


	// 获取资产注册表
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// 获取依赖项
	TArray<FAssetDependency> Dependencies;
	FAssetIdentifier AssetId = FAssetIdentifier(Asset->GetOutermost()->GetFName());

	FAssetManagerDependencyQuery Query;
	Query.Categories = UE::AssetRegistry::EDependencyCategory::Package;
	Query.Flags = UE::AssetRegistry::EDependencyQuery::Hard;

	AssetRegistry.GetDependencies(AssetId, Dependencies, Query.Categories, Query.Flags);

	// 检查每个依赖项
	for (const FAssetDependency& Dep : Dependencies)
	{
		if (Dep.AssetId.IsPackage())
		{
			FString DependencyPath = Dep.AssetId.PackageName.ToString();
			if (!DoesAssetExist(DependencyPath))
			{
				OutReferenceInfo.InvalidReferencePaths.Add(DependencyPath);
			}
		}
	}
	// reload asset to cut off file handle.
	TArray<UPackage*> PackagesToReload;
	PackagesToReload.Add(Asset->GetPackage());
	UPackageTools::ReloadPackages(PackagesToReload);

	return OutReferenceInfo.InvalidReferencePaths.IsEmpty() &&
		OutReferenceInfo.ValidationErrors.IsEmpty();
}


bool UYMythEdTools::ReplaceAssetReference(const FString& AssetToFixPath, const FString& InvalidRefPath, const FString& NewRefPath)
{
	//验证路径
	if (!ValidateAssetPath(AssetToFixPath) || !ValidateAssetPath(NewRefPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid asset path provided. AssetToFixPath: %s, NewRefPath: %s"), *AssetToFixPath, *NewRefPath);
		return false;
	}

	FString SrcFilename;
	if (!FPackageName::DoesPackageExist(AssetToFixPath, &SrcFilename))
	{
		UE_LOG(LogTemp, Error, TEXT("ReplaceAssetReference: Source asset doesn't exist %s."), *AssetToFixPath);
		return false;
	}

	//构建对象路径

	FString NewPackageName = FPackageName::ObjectPathToPackageName(NewRefPath);

	// 创建路径映射
	TMap<FString, FString> PackageRenames = {
		{InvalidRefPath, NewPackageName}
	};
	// PackageRenames.Add("/Game/UI/Menu/LyraScrollBox", "/Game/YMyth/Blueprints/UI/LyraScrollBox");
	// self include
	FString PackageNameToFix = FPackageName::ObjectPathToPackageName(AssetToFixPath);
	PackageRenames.Add(PackageNameToFix, PackageNameToFix);

	// 设置重定向上下文
	FAssetHeaderPatcher::FContext Context(PackageRenames, false);
	FCoreRedirects::AddRedirectList(Context.Redirects, TEXT("Asset Header Patcher Tests"));

	// 生成临时文件路径（使用GUID确保唯一性）
	FString TempFilePath = FPaths::CreateTempFilename(*FPaths::ProjectSavedDir(), TEXT("AssetPatch_"), TEXT(".tmp"));

	// 创建并执行 AssetHeaderPatcher
	FAssetHeaderPatcher Patcher;
	FAssetHeaderPatcher::EResult Result = Patcher.DoPatch(SrcFilename, TempFilePath, Context);

	auto DoCleanup = [TempFilePath, Context]
	{
		// 清理临时文件
		IFileManager::Get().Delete(*TempFilePath);
		// 清理重定向
		FCoreRedirects::RemoveRedirectList(Context.Redirects, TEXT("Asset Header Patcher Tests"));
	};

	if (Result != FAssetHeaderPatcher::EResult::Success)
	{
		DoCleanup();
		UE_LOG(LogTemp, Error, TEXT("Failed to replace asset reference. Asset: %s, Error: %s"), *SrcFilename, *LexToString(Result));
		return false;
	}


	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FSoftObjectPath ObjectPath(*AssetToFixPath);
	TArray<FAssetData> AssetsToDelete;
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(ObjectPath);
	if (ObjectTools::ForceDeleteObjects({AssetData.GetAsset()}, false) <= 0)
	{
		DoCleanup();
		UE_LOG(LogTemp, Error, TEXT("Failed to delete original asset: %s"), *SrcFilename);
		return false;
	}

	// 移动临时文件到目标位置
	if (!IFileManager::Get().Move(*SrcFilename, *TempFilePath, true, true))
	{
		DoCleanup();
		UE_LOG(LogTemp, Error, TEXT("Failed to replace original file with temp file %s -> %s"),
		       *TempFilePath, *SrcFilename);
		return false;
	}
	DoCleanup();
	UE_LOG(LogTemp, Log, TEXT("Successfully replaced asset reference in: %s"), *SrcFilename);
	return true;
}


bool UYMythEdTools::ShowReplaceReferenceDialog(const TArray<FString>& InvalidReferencePaths, TMap<FString, FString>& OutPathRedirects)
{
	if (InvalidReferencePaths.IsEmpty())
	{
		return false;
	}

	return SReplaceWidgetReferenceDialog::ShowDialog(InvalidReferencePaths, OutPathRedirects);
}

bool UYMythEdTools::BatchFixAssetReferences(const FString& AssetPath,
                                            const TMap<FString, FString>& PathRedirects,
                                            TArray<FString>& OutFailedAssets)
{
	if (AssetPath.IsEmpty() || PathRedirects.IsEmpty())
	{
		return false;
	}

	// FScopedSlowTask SlowTask(AssetPaths.Num(), NSLOCTEXT("WidgetAssetFixer", "BatchFixingAssetReferences", "Fixing Asset References..."));
	// SlowTask.MakeDialog();

	bool bSuccess = true;
	// for (const FString& AssetPath : AssetPaths)
	// {
	// 	SlowTask.EnterProgressFrame(1);

	// 遍历所有需要重定向的路径
	for (const auto& PathPair : PathRedirects)
	{
		const FString& InvalidPath = PathPair.Key;
		const FString& NewPath = PathPair.Value;

		if (!ReplaceAssetReference(AssetPath, InvalidPath, NewPath))
		{
			OutFailedAssets.Add(AssetPath);
			bSuccess = false;
			break;
		}
	}
	// }

	return bSuccess;
}

bool UYMythEdTools::DoesAssetExist(const FString& AssetPath)
{
	if (AssetPath.IsEmpty())
	{
		return false;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FSoftObjectPath ObjectPath(AssetPath);
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(ObjectPath);
	return AssetData.IsValid();
}

bool UYMythEdTools::ValidateAssetPath(const FString& AssetPath)
{
	return !AssetPath.IsEmpty();
}

UObject* UYMythEdTools::LoadAsset(const FString& AssetPath)
{
	if (!ValidateAssetPath(AssetPath))
	{
		return nullptr;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FSoftObjectPath ObjectPath(AssetPath);
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(ObjectPath);
	if (!AssetData.IsValid())
	{
		return nullptr;
	}

	return AssetData.GetAsset();
}

bool UYMythEdTools::SavePackages(const TArray<UPackage*>& PackagesToSave)
{
	if (PackagesToSave.IsEmpty())
	{
		return true;
	}

	FEditorFileUtils::FPromptForCheckoutAndSaveParams SaveParams;
	SaveParams.bCheckDirty = false;
	SaveParams.bPromptToSave = true;
	SaveParams.bIsExplicitSave = true;

	return FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, SaveParams) == FEditorFileUtils::PR_Success;
}


#undef LOCTEXT_NAMESPACE
