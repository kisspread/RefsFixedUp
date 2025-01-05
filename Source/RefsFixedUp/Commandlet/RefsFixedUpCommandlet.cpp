// Copy Right ZeroSoul
#include "RefsFixedUpCommandlet.h"
#include "YMythEdTools.h"
DEFINE_LOG_CATEGORY_STATIC(LogRefFixedUp, Log, All);

int32 URefsFixedUpCommandlet::Main(const FString& Params)
{
	UE_LOG(LogRefFixedUp, Warning, TEXT("RefFixedUp commandlet is running..."));

	TArray<FString> Tokens;
	TArray<FString> Switches;
	TMap<FString, FString> ParamsMap;
	ParseCommandLine(*Params, Tokens, Switches, ParamsMap);

	IFileManager& FileManager = IFileManager::Get();
	
	FString ScanDirPath = ParamsMap.FindRef(TEXT("ScanDirPath"));
	
	FString InvalidRefPath = ParamsMap.FindRef(TEXT("InvalidRefPath"));
	
	FString NewRefPath = ParamsMap.FindRef(TEXT("NewRefPath"));

	// Example:
	// ScanDirPath= TEXT("X:/YourProject/Content/Blueprints/UI/Menu/");
	// InvalidRefPath = TEXT("/Game/UI/Menu/LyraScrollBox");
	// NewRefPath = TEXT("/Game/YourProject/Blueprints/UI/LyraScrollBox");

	 
	UE_LOG(LogRefFixedUp, Warning, TEXT("ScanDirPath: %s"), *ScanDirPath);
	
	TArray<FString> FilesAndDirectories;

	FileManager.FindFilesRecursive(FilesAndDirectories, *ScanDirPath, TEXT("*.uasset"), true, false, false);

	
	for (const FString& File : FilesAndDirectories)
	{
		// UE_LOG(LogRefFixedUp, Warning, TEXT("File: %s"), *File);
		FString PackagePath;
		if (FPackageName::TryConvertFilenameToLongPackageName(File, PackagePath))
		{
			FString AssetName = FPaths::GetBaseFilename(PackagePath);
    
			// 拼接完整的对象路径
			FString ObjectPath = FString::Printf(TEXT("%s.%s"), *PackagePath, *AssetName);
		
			
			FAssetReferenceInfo AssetReferenceInfo;
			UYMythEdTools::AnalyzeAssetReferences(ObjectPath, AssetReferenceInfo);
			for (auto InvalidPath : AssetReferenceInfo.InvalidReferencePaths)
			{
				if (InvalidRefPath == InvalidPath)
				{
					UE_LOG(LogRefFixedUp, Warning, TEXT("-----Doing ReplaceAssetReference for: ObjectPath: %s"), *ObjectPath);
					UYMythEdTools::ReplaceAssetReference(ObjectPath, InvalidPath, NewRefPath);
				}
			}
		}
	}

	UE_LOG(LogRefFixedUp, Warning, TEXT("RefFixedUp commandlet is done."));
	system("pause");
	return 0;
}
