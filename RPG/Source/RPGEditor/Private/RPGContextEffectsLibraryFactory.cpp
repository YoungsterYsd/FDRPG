// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGContextEffectsLibraryFactory.h"

#include "Feedback/ContextEffects/RPGContextEffectsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGContextEffectsLibraryFactory)

class FFeedbackContext;
class UClass;
class UObject;

URPGContextEffectsLibraryFactory::URPGContextEffectsLibraryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = URPGContextEffectsLibrary::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* URPGContextEffectsLibraryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	URPGContextEffectsLibrary* RPGContextEffectsLibrary = NewObject<URPGContextEffectsLibrary>(InParent, Name, Flags);

	return RPGContextEffectsLibrary;
}
