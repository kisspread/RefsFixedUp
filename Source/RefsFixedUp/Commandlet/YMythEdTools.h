// Copy Right ZeroSoul

#pragma once
#include "CoreMinimal.h"
#include "YMythEdTools.generated.h"

/** Widget 资产引用分析结果 */
USTRUCT(BlueprintType)
struct FAssetReferenceInfo
{
	GENERATED_BODY()

	/** 资产路径 */
	UPROPERTY(BlueprintReadOnly, Category = "Asset Reference")
	FString AssetPath;

	/** 无效的引用路径 */
	UPROPERTY(BlueprintReadOnly, Category = "Asset Reference")
	TArray<FString> InvalidReferencePaths;

	/** 验证错误信息 */
	UPROPERTY(BlueprintReadOnly, Category = "Asset Reference")
	TArray<FText> ValidationErrors;

	FAssetReferenceInfo()
	{}

	explicit FAssetReferenceInfo(const FString& InAssetPath)
		: AssetPath(InAssetPath)
	{}
};

/**
 * 用于在蓝图中修复 Widget 资产引用的工具类
 */
UCLASS()
class UYMythEdTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 分析资产的引用问题 */
	UFUNCTION(BlueprintCallable, Category = "Asset Tools|Widget")
	static bool AnalyzeAssetReferences(const FString& AssetPath, FAssetReferenceInfo& OutReferenceInfo);

	/** 替换资产引用 */
	UFUNCTION(BlueprintCallable, Category = "Asset Tools|Widget")
	static bool ReplaceAssetReference(const FString& AssetToFixPath, const FString& InvalidRefPath, const FString& NewRefPath);

	/** 显示替换引用对话框 */
	UFUNCTION(BlueprintCallable, Category = "Asset Tools|Widget")
	static bool ShowReplaceReferenceDialog(const TArray<FString>& InvalidReferencePaths, TMap<FString, FString>& OutPathRedirects);

	/** 批量修复资产引用 */
	UFUNCTION(BlueprintCallable, Category = "Asset Tools|Widget")
	static bool BatchFixAssetReferences(const FString& AssetPath, 
	UPARAM(ref) const TMap<FString, FString>& PathRedirects, 
	TArray<FString>& OutFailedAssets); 

	/** 检查资产是否存在 */
	UFUNCTION(BlueprintPure, Category = "Asset Tools|Widget")
	static bool DoesAssetExist(const FString& AssetPath);

private:
	/** 检查资产路径是否有效 */
	static bool ValidateAssetPath(const FString& AssetPath);

	/** 加载资产 */
	static UObject* LoadAsset(const FString& AssetPath);

	/** 保存修改的包 */
	static bool SavePackages(const TArray<UPackage*>& PackagesToSave);
};
