#pragma once

#ifndef DISABLE_PLAYFABENTITY_API

// define body for logging or debug output
#define LOG_PIPELINE(S) /*std::cout << S*/

#include "PlayFabEvent.h"
#include "PlayFabEventBuffer.h"

#include <mutex>
#include <unordered_map>

namespace PlayFab
{
    class PlayFabEventsInstanceAPI;

    enum class PlayFabEventPipelineType
    {
        PlayFabPlayStream,
        PlayFabTelemetry,
    };

    /// <summary>
    /// Settings for any event pipeline
    /// NOTE: settings are expected to be set prior to calling PlayFabEventPipeline::Start()
    /// changing them after PlayFabEventPipeline::Start() may cause threading issues
    /// users should not expect changes made to settings to take effect after ::Start is called unless the pipeline is destroyed and re-created
    /// </summary>
    class PlayFabEventPipelineSettings
    {
    public:
        PlayFabEventPipelineSettings();
        PlayFabEventPipelineSettings(PlayFabEventPipelineType emitType);
        PlayFabEventPipelineSettings(PlayFabEventPipelineType emitType, bool useBackgroundThread);
        virtual ~PlayFabEventPipelineSettings() {};

        size_t bufferSize; // The minimal size of buffer, in bytes. The actually allocated size will be a power of 2 that is equal or greater than this value.
        size_t maximalNumberOfItemsInBatch; // The maximal number of items (events) a batch can hold before it is sent out.
        size_t maximalBatchWaitTime; // The maximal wait time before a batch must be sent out even if it's still incomplete, in seconds.
        size_t maximalNumberOfRetries; // The maximal number of retries for transient transport errors, before a batch is discarded.
        size_t maximalNumberOfBatchesInFlight; // The maximal number of batches currently "in flight" (sent to a transport plugin).
        int64_t readBufferWaitTime; // The wait time between attempts to read events from buffer when it is empty, in milliseconds.
        SharedPtr<PlayFabAuthenticationContext> authenticationContext; // The optional PlayFab authentication context that can be used with static PlayFab events API
        PlayFabEventPipelineType emitType; // whether we call WriteEvent or WriteTelemetryEvent through PlayFab
        bool useBackgroundThread;
    };

    /// <summary>
    /// Interface for any event pipeline
    /// </summary>
    class IPlayFabEventPipeline
    {
    public:
        virtual ~IPlayFabEventPipeline() {}
        virtual void Start() {} // Start pipeline's worker thread
        virtual void Stop() = 0;
        virtual void Update() = 0;
        virtual void IntakeEvent(SharedPtr<const IPlayFabEmitEventRequest> request) = 0; // Intake an event. This method must be thread-safe.
    };

    /// <summary>
    /// Implementation of PlayFab-specific event pipeline
    /// </summary>
    class PlayFabEventPipeline : public IPlayFabEventPipeline
    {
    public:
        explicit PlayFabEventPipeline(const SharedPtr<PlayFabEventPipelineSettings>& settings);
        virtual ~PlayFabEventPipeline() override;

        PlayFabEventPipeline(const PlayFabEventPipeline& source) = delete; // disable copy
        PlayFabEventPipeline(PlayFabEventPipeline&&) = delete; // disable move
        PlayFabEventPipeline& operator=(const PlayFabEventPipeline& source) = delete; // disable assignment
        PlayFabEventPipeline& operator=(PlayFabEventPipeline&& other) = delete; // disable move assignment

        // NOTE: settings are expected to be set prior to calling PlayFabEventPipeline::Start()
        // changing them after PlayFabEventPipeline::Start() may cause threading issues unless you have set useBackgroundThread flag to true
        // If this flag is not set, users should not expect changes made to settings to take effect after ::Start is called
        //   unless the pipeline is A.) destroyed and re-created or B.) restart it by running Stop() and then Start() again 
        SharedPtr<PlayFabEventPipelineSettings> GetSettings() const;
        virtual void Start() override;
        virtual void Stop() override;
        virtual void Update() override;
        virtual void IntakeEvent(SharedPtr<const IPlayFabEmitEventRequest> request) override;

        //void SetExceptionCallback(ExceptionCallback callback);

    protected:
        virtual void SendBatch(Vector<SharedPtr<const IPlayFabEmitEventRequest>>& batch);

    private:
        void WorkerThread();
        bool DoWork();
        void WriteEventsApiCallback(Result<EventsModels::WriteEventsResponse> result, SharedPtr<Vector<SharedPtr<const IPlayFabEmitEventRequest>>> batch, size_t batchId);
        //void WriteEventsApiErrorCallback(const PlayFabError& error, SharedPtr<Vector<SharedPtr<const IPlayFabEmitEventRequest>>> batch, size_t batchId);
        void CallbackRequest(SharedPtr<const IPlayFabEmitEventRequest> request, SharedPtr<const IPlayFabEmitEventResponse> response);

    private:
        SharedPtr<PlayFabEventsInstanceAPI> eventsApi;

        std::atomic<size_t> nextBatchId{ 0 };
        std::atomic<size_t> batchesInFlight{ 0 };
        Vector<SharedPtr<const IPlayFabEmitEventRequest>> batch;
        std::chrono::steady_clock::time_point momentBatchStarted;
        SharedPtr<PlayFabEventPipelineSettings> settings;
        PlayFabEventBuffer buffer;
        std::thread workerThread;
        std::atomic<bool> isWorkerThreadRunning;
        std::mutex userExceptionCallbackMutex;
        //ExceptionCallback userExceptionCallback;
    };
}

#endif
