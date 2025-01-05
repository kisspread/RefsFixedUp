// Copy Right ZeroSoul

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
// #include "Editor/UnrealEd/Classes/Commandlets/ReplaceAssetsCommandlet.h"
#include "RefsFixedUpCommandlet.generated.h"



/**
 * 
 */
UCLASS()
class REFSFIXEDUP_API URefsFixedUpCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	virtual int32 Main(const FString& Params) override;
};
