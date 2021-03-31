#pragma once

#if defined (PLAYFAB_PLATFORM_WINDOWS) || defined (PLAYFAB_PLATFORM_XBOX)
#include <string>
#include <Multiplayer/MultiplayerDataModels.h>

namespace PlayFab
{
    namespace QoS
    {
        /// <summary>
        /// The result of pinging a datacenter.
        /// </summary>
        struct RegionResult
        {
        public:
            RegionResult(const String& region, int latencyMs, int errorCode);
            RegionResult() = delete;

            // The datacenter region
            String region;

            // Average latency to reach the data center
            int latencyMs;

            // Last error code recieved while pinging
            int errorCode;
        };
    }
}
#endif // defined (PLAYFAB_PLATFORM_WINDOWS) || defined (PLAYFAB_PLATFORM_XBOX)