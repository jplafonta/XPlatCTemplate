#include "stdafx.h"

#ifndef DISABLE_PLAYFABENTITY_API

#include "PlayFabEventPipeline.h"
#include "PlayFabSettings.h"
#include <Events/EventsApi.h>

#include <chrono>

namespace PlayFab
{
    static const int defaultBufferSize = 256;
    static const int defaultMaxItemsInBatch = 5;
    static const int defaultMaxBatchWaitTimeInSeconds = 3;
    static const int defaultMaxBatchesInFlight = 16;
    static const int defaultReadBufferWaitTimeInMs = 10;

    PlayFabEventPipelineSettings::PlayFabEventPipelineSettings() :
        bufferSize(defaultBufferSize),
        maximalNumberOfItemsInBatch(defaultMaxItemsInBatch),
        maximalBatchWaitTime(defaultMaxBatchWaitTimeInSeconds),
        maximalNumberOfBatchesInFlight(defaultMaxBatchesInFlight),
        readBufferWaitTime(defaultReadBufferWaitTimeInMs),
        authenticationContext(nullptr),
        emitType(PlayFabEventPipelineType::PlayFabPlayStream),
        useBackgroundThread(true)
    {
    }

    PlayFabEventPipelineSettings::PlayFabEventPipelineSettings(PlayFabEventPipelineType type, bool useBackgroundThread) :
        bufferSize(defaultBufferSize),
        maximalNumberOfItemsInBatch(defaultMaxItemsInBatch),
        maximalBatchWaitTime(defaultMaxBatchWaitTimeInSeconds),
        maximalNumberOfBatchesInFlight(defaultMaxBatchesInFlight),
        readBufferWaitTime(defaultReadBufferWaitTimeInMs),
        authenticationContext(nullptr),
        emitType(type),
        useBackgroundThread(useBackgroundThread)
    {
    }

    PlayFabEventPipeline::PlayFabEventPipeline(const SharedPtr<PlayFabEventPipelineSettings>& settings) :
        nextBatchId(0),
        buffer(settings->bufferSize),
        isWorkerThreadRunning(false)
    {
        // TODO
        //eventsApi = MakeShared<EventsAPI>(PlayFabSettings::staticPlayer);

        this->settings = settings;
        if (this->settings->useBackgroundThread)
        {
            this->Start();
        }
    }

    PlayFabEventPipeline::~PlayFabEventPipeline()
    {
        Stop();
    }

    void PlayFabEventPipeline::Start()
    {
        if (!this->settings->useBackgroundThread)
        {
            LOG_PIPELINE("PlayFabEventPipeline is set to NOT use background threads. Start() is not needed to be called then, but Update() is required to be manually called every tick in this case.");
            return;
        }

        if(isWorkerThreadRunning)
        {
            LOG_PIPELINE("PlayFabEventPipeline has already been started, and should not be started again until Stop() is called.");
            return;
        }

        // start worker thread
        this->isWorkerThreadRunning = true;
        if (!this->workerThread.joinable())
        {
            this->workerThread = std::thread(&PlayFabEventPipeline::WorkerThread, this);
        }
    }

    void PlayFabEventPipeline::Stop()
    {
        // flush remaining events
        if (this->batch.size() > 0)
        {
            this->SendBatch(this->batch);
        }

        if (!this->settings->useBackgroundThread)
        {
            LOG_PIPELINE("PlayFabEventPipeline is set to NOT use background threads. Stop() is not needed to be called then, but Update() is required to be manually called every tick in this case.");
            return;
        }

        if(!isWorkerThreadRunning)
        {
            LOG_PIPELINE("PlayFabEventPipeline has already been stopped, and should not be stopped again until Start() is called.");
            return;
        }

        // stop worker thread
        this->isWorkerThreadRunning = false;
        if (this->workerThread.joinable())
        {
            this->workerThread.join();
        }
    }

    void PlayFabEventPipeline::Update()
    {
        if (this->settings->useBackgroundThread)
        {
            throw std::runtime_error("You should not call Update() when PlayFabEventPipelineSettings::useBackgroundThread == true");
        }

        bool hasMoreWorkToProcess = false;
        do
        {
            hasMoreWorkToProcess = DoWork();
        } while (hasMoreWorkToProcess);
    }

    // NOTE: settings are expected to be set prior to calling PlayFabEventPipeline::Start()
    // changing them after PlayFabEventPipeline::Start() may cause threading issues
    // users should not expect changes made to settings to take effect after ::Start is called unless the pipeline is destroyed and re-created
    SharedPtr<PlayFabEventPipelineSettings> PlayFabEventPipeline::GetSettings() const
    {
        return this->settings;
    }

    void PlayFabEventPipeline::IntakeEvent(SharedPtr<const IPlayFabEmitEventRequest> request)
    {
        try
        {
            using Result = PlayFabEventBuffer::EventProducingResult;
            EmitEventResult emitResult;

            // put event into buffer
            switch (this->buffer.TryPut(request))
            {
            case Result::Success:
                return;

            case Result::Overflow:
            {
                emitResult = EmitEventResult::Overflow;
                LOG_PIPELINE("Buffer overflow");
            }
            break;

            case Result::Disabled:
            {
                emitResult = EmitEventResult::Disabled;
            }
            break;

            default:
            {
                emitResult = EmitEventResult::NotSupported;
                LOG_PIPELINE("TryPut returned an unknown type of result");
            }
            break;
            }

            // pipeline failed to intake the event, create a response
            SharedPtr<const PlayFabEmitEventRequest> playFabEmitRequest = std::dynamic_pointer_cast<const PlayFabEmitEventRequest>(request);
            auto playFabEmitEventResponse = MakeShared<PlayFabEmitEventResponse>();
            playFabEmitEventResponse->emitEventResult = emitResult;

            SharedPtr<ServiceResponse> emitEventError = MakeShared<ServiceResponse>();

            emitEventError->ErrorName = "PlayFabEventPipeline IntakeEvent Error";
            emitEventError->ErrorMessage = "PlayFabEventPipeline did not accept the event. Please see ErrorDetails for more information.";

            emitEventError->HttpCode = 0;
            emitEventError->HttpStatus = "None";

            if (emitResult == EmitEventResult::Overflow)
            {
                emitEventError->ErrorDetails = "PlayFabEventPipeline was unable to take the event due to memory limits. Please wait for batching to complete before retrying or increase the PlayFabEventBuffer size (see its constructor)";
            }
            else if (emitResult == EmitEventResult::Disabled)
            {
                emitEventError->ErrorDetails = "PlayFabEventPipeline was unable to take the event due to being disabled. Please enable the pipeline before attempting to add events.";
            }
            else
            {
                emitEventError->ErrorDetails = "PlayFabEventPipeline was unable to take the event due to an unknown reason.";
            }

            playFabEmitEventResponse->playFabError = emitEventError;

            // call an emit event callback
            CallbackRequest(playFabEmitRequest, std::move(playFabEmitEventResponse));
        }
        catch (...)
        {
            LOG_PIPELINE("An exception was caught in PlayFabEventPipeline::IntakeEvent method");
        }
    }

    //void PlayFabEventPipeline::SetExceptionCallback(ExceptionCallback ex)
    //{
    //    { // LOCK userCallbackMutex
    //        std::unique_lock<std::mutex> lock(userExceptionCallbackMutex);
    //        userExceptionCallback = ex;
    //    } // UNLOCK userCallbackMutex
    //}

    void PlayFabEventPipeline::WorkerThread()
    {
        using clock = std::chrono::steady_clock;

        while (this->isWorkerThreadRunning)
        {
            bool hasMoreWorkToProcess = DoWork();

            if (!hasMoreWorkToProcess)
            {
                // give some time for batches in flight to deflate
                std::this_thread::sleep_for(std::chrono::milliseconds(this->settings->readBufferWaitTime));
            }
        }
        if( this->batch.size() > 0)
        {
            // Flush remaining events on shutdown
            this->SendBatch(this->batch);            
        }
    }

    bool PlayFabEventPipeline::DoWork()
    {
        using clock = std::chrono::steady_clock;
        using Result = PlayFabEventBuffer::EventConsumingResult;
        SharedPtr<const IPlayFabEmitEventRequest> request;

        try
        {
            this->batch.reserve(this->settings->maximalNumberOfItemsInBatch);
        
            while (this->isWorkerThreadRunning)
            {
                // Process events in the loop
                if (batchesInFlight >= this->settings->maximalNumberOfBatchesInFlight)
                {
                    // do not take new events from buffer if batches currently in flight are at the maximum allowed number
                    // and are not sent out (or received an error) yet
                    return false;
                }

                // Don't try taking a request until we get an entity token. We'd rather not lose events
                // that were generated before an entity token was created (i.e. before a user was created).
                if (PlayFabSettings::staticPlayer->entityToken.empty() &&
                    (settings->authenticationContext == nullptr || settings->authenticationContext->entityToken.empty()))
                {
                    return false;
                }
                
                switch (this->buffer.TryTake(request))
                {
                case Result::Success:
                {
                    // add an event to batch
                    this->batch.push_back(std::move(request));

                    if (this->batch.size() == 1)
                    {
                        // if it is the first event in an incomplete batch then set the batch creation moment
                        this->momentBatchStarted = clock::now();
                    }
                    
                    if (this->batch.size() >= this->settings->maximalNumberOfItemsInBatch)
                    {
                        // if batch is full
                        this->SendBatch(this->batch);
                    }
                    return true;
                }

                case Result::Disabled:
                case Result::Empty:
                default:
                    break;
                }

                // if batch was started
                if (this->batch.size() > 0)
                {
                    // check if the batch wait time expired
                    std::chrono::seconds batchAge = std::chrono::duration_cast<std::chrono::seconds>(clock::now() - this->momentBatchStarted);
                    if (batchAge.count() >= (int32_t)this->settings->maximalBatchWaitTime)
                    {
                        // batch wait time expired, send incomplete batch
                        this->SendBatch(this->batch);
                        return true;
                    }
                }
                return false;
            }
        }
        catch (const std::exception& ex)
        {
            UNREFERENCED_PARAMETER(ex);
            LOG_PIPELINE("An exception was caught in PlayFabEventPipeline::WorkerThread method");
            this->isWorkerThreadRunning = false;

            { // LOCK userCallbackMutex
                std::unique_lock<std::mutex> lock(userExceptionCallbackMutex);
                //if (userExceptionCallback)
                //{
                //    userExceptionCallback(ex);
                //}
            } // UNLOCK userCallbackMutex
        }
        catch (...)
        {
            LOG_PIPELINE("A non std::exception was caught in PlayFabEventPipeline::WorkerThread method");
        }
        return false;
    }

    void PlayFabEventPipeline::SendBatch(Vector<SharedPtr<const IPlayFabEmitEventRequest>>& localbatch)
    {
        // create a WriteEvents API request to send the batch
        EventsModels::WriteEventsRequest batchReq;
        if (this->settings->authenticationContext != nullptr)
        {
            //batchReq.authenticationContext = this->settings->authenticationContext;
        }

        // TODO
        //for (const auto& eventEmitRequest : localbatch)
        //{
        //    const auto& playFabEmitRequest = std::dynamic_pointer_cast<const PlayFabEmitEventRequest>(eventEmitRequest);
        //    batchReq.Events.push_back(playFabEmitRequest->event->eventContents);
        //}

        size_t batchId = this->nextBatchId.fetch_add(1);
        ++batchesInFlight;
        auto batchPtr = MakeShared<Vector<SharedPtr<const IPlayFabEmitEventRequest>>>(std::move(localbatch));
        localbatch.clear(); // batch vector will be reused
        localbatch.reserve(this->settings->maximalNumberOfItemsInBatch);

        if (this->settings->emitType == PlayFabEventPipelineType::PlayFabPlayStream)
        {
            // call Events API to send the batch
            eventsApi->WriteEvents(
                batchReq,
                TaskQueue() // TODO allow setting queue here somehow
            ).Finally(std::bind(&PlayFabEventPipeline::WriteEventsApiCallback, this, std::placeholders::_1, batchPtr, batchId));
        }
        else
        {
            // call Events API to send the batch, bypassing Playstream
            eventsApi->WriteTelemetryEvents(
                batchReq,
                TaskQueue() // TODO allow setting queue here somehow
            ).Finally(std::bind(&PlayFabEventPipeline::WriteEventsApiCallback, this, std::placeholders::_1, batchPtr, batchId));
        }
    }

    void PlayFabEventPipeline::WriteEventsApiCallback(Result<EventsModels::WriteEventsResponse> result, SharedPtr<Vector<SharedPtr<const IPlayFabEmitEventRequest>>> batchWritten, size_t batchId)
    {
        --batchesInFlight;

        if (Succeeded(result))
        {
            // call individual emit event callbacks
            for (const auto& eventEmitRequest : *batchWritten)
            {
                SharedPtr<const PlayFabEmitEventRequest> playFabEmitRequest = std::dynamic_pointer_cast<const PlayFabEmitEventRequest>(eventEmitRequest);
                auto playFabEmitEventResponse = SharedPtr<PlayFabEmitEventResponse>(new PlayFabEmitEventResponse());
                playFabEmitEventResponse->emitEventResult = EmitEventResult::Success;
                auto playFabError = MakeShared<ServiceResponse>();
                playFabError->HttpCode = 200;
                playFabError->ErrorCode = ServiceErrorCode::Success;
                playFabEmitEventResponse->playFabError = playFabError;
                playFabEmitEventResponse->writeEventsResponse = SharedPtr<EventsModels::WriteEventsResponse>(new EventsModels::WriteEventsResponse(result.Payload()));
                playFabEmitEventResponse->batch = batchWritten;
                playFabEmitEventResponse->batchNumber = batchId;

                // call an emit event callback
                CallbackRequest(playFabEmitRequest, std::move(playFabEmitEventResponse));
            }
        }
        else
        {
            // call individual emit event callbacks
            for (const auto& eventEmitRequest : *batchWritten)
            {
                SharedPtr<const PlayFabEmitEventRequest> playFabEmitRequest = std::dynamic_pointer_cast<const PlayFabEmitEventRequest>(eventEmitRequest);
                auto playFabEmitEventResponse = SharedPtr<PlayFabEmitEventResponse>(new PlayFabEmitEventResponse());
                playFabEmitEventResponse->emitEventResult = EmitEventResult::Success;
                playFabEmitEventResponse->playFabError = SharedPtr<ServiceResponse>(new ServiceResponse(/*TODO*/));
                playFabEmitEventResponse->batch = batchWritten;
                playFabEmitEventResponse->batchNumber = batchId;

                // call an emit event callback
                CallbackRequest(playFabEmitRequest, std::move(playFabEmitEventResponse));
            }
        }
    }

    void PlayFabEventPipeline::CallbackRequest(SharedPtr<const IPlayFabEmitEventRequest> request, SharedPtr<const IPlayFabEmitEventResponse> response)
    {
        const auto& playFabEmitRequest = std::dynamic_pointer_cast<const PlayFabEmitEventRequest>(request);

        if (playFabEmitRequest->callback != nullptr)
        {
            playFabEmitRequest->callback(playFabEmitRequest->event, response);
        }

        if (playFabEmitRequest->stdCallback != nullptr)
        {
            playFabEmitRequest->stdCallback(playFabEmitRequest->event, response);
        }
    }
}

#endif
