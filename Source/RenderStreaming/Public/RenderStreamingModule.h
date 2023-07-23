#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "RenderStreamingStreamer.h"

class FRenderStreamingModule : public IModuleInterface
{
public:
	static inline FRenderStreamingModule &Get()
	{
		return FModuleManager::LoadModuleChecked<FRenderStreamingModule>("RenderStreaming");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RenderStreaming");
	}

private:
	void StartupModule() override;
	void ShutdownModule() override;

	TUniquePtr<FRenderStreamingStreamer> Streamer;
};