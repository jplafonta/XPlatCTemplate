#include "stdafx.h"
#include "GlobalState.h"
#include "AsyncProvider.h"
#include <httpClient/httpClient.h>

using namespace PlayFab;

PlayFabGlobalState::PlayFabGlobalState() :
    state{ MakeShared<GlobalState>() }
{
}

HRESULT PlayFabGlobalState::Create(PlayFabStateHandle* stateHandle)
{
    RETURN_IF_FAILED(HCInitialize(nullptr));

    Allocator<PlayFabGlobalState> a{};
    *stateHandle = UniquePtr<PlayFabGlobalState>(new (a.allocate(1)) PlayFabGlobalState()).release();
    return S_OK;
}

HRESULT PlayFabGlobalState::CleanupAsync(XAsyncBlock* async)
{
    struct CleanupProvider : public Provider
    {
        CleanupProvider(XAsyncBlock* async, PlayFabStateHandle handle) :
            Provider{ async },
            stateHandle{ handle }
        {
        }

        HRESULT Begin(TaskQueue&& queue) override
        {
            if (stateHandle->state.use_count() > 1)
            {
                // Need to decide if it is client's responsibility to wait for any pending API calls to complete before
                // calling cleanup or if we should wait here
                return E_FAIL;
            }
            else
            {
                // stateHandle->state is the only remaining reference. GlobalState will be destroyed on this thread before
                // control returns to client
                UniquePtr<PlayFabGlobalState> reclaim{ stateHandle };

                // Cleanup libHttpClient
                hcCleanupAsync.queue = queue.DeriveWorkerQueue().GetHandle();
                hcCleanupAsync.callback = HCCleanupComplete;
                hcCleanupAsync.context = this;

                RETURN_IF_FAILED(HCCleanupAsync(&hcCleanupAsync));

                return E_PENDING;
            }
        }

        static void CALLBACK HCCleanupComplete(XAsyncBlock* async)
        {
            CleanupProvider* self = static_cast<CleanupProvider*>(async->context);
            assert(self);

            HRESULT hr = XAsyncGetStatus(async, false);
            assert(hr != E_PENDING);

            if (hr == E_HC_INTERNAL_STILLINUSE)
            {
                // If something else is still referencing libHttpClient, we do not care
                self->Complete(0);
                return;
            }
            else if (FAILED(hr))
            {
                // Pass along HCCleanup error. Note that state is still destroyed
                self->Fail(hr);
                return;
            }

            self->Complete(0);
        }

        XAsyncBlock hcCleanupAsync{};
        PlayFabStateHandle stateHandle;
    };

    return Provider::Run(UniquePtr<Provider>{ MakeUnique<CleanupProvider>(async, this).release() });
}
