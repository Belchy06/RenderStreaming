#include "RenderStreamingModule.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRenderStreamingModule, Log, All);
DEFINE_LOG_CATEGORY(LogRenderStreamingModule);

void FRenderStreamingModule::StartupModule()
{
    FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([this]()
                                                        { Streamer = MakeUnique<FRenderStreamingStreamer>(); });
}

void FRenderStreamingModule::ShutdownModule()
{
    Streamer.Reset(nullptr);
}

IMPLEMENT_MODULE(FRenderStreamingModule, RenderStreaming)