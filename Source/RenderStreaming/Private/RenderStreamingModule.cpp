#include "RenderStreamingModule.h"

#include "rtc_base/ssl_adapter.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRenderStreamingModule, Log, All);
DEFINE_LOG_CATEGORY(LogRenderStreamingModule);

void FRenderStreamingModule::StartupModule()
{
    FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([this]()

                                                        { rtc::InitializeSSL();
                                                        this->Streamer = MakeUnique<FRenderStreamingStreamer>(); });
}

void FRenderStreamingModule::ShutdownModule()
{
    Streamer.Reset(nullptr);
}

IMPLEMENT_MODULE(FRenderStreamingModule, RenderStreaming)