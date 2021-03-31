#pragma once

#include <playfab/PlayFabGlobal.h>

namespace PlayFab
{
    // Any global state will go here
    class GlobalState
    {
    public:
        GlobalState() = default;
        virtual ~GlobalState() = default;
    };
}

struct PlayFabGlobalState
{
    PlayFab::SharedPtr<PlayFab::GlobalState> state;

    static HRESULT Create(_Outptr_ PlayFabStateHandle* stateHandle);
    HRESULT CleanupAsync(XAsyncBlock* async);

private:
    PlayFabGlobalState();
};