#include <stdafx.h>

#ifndef DISABLE_PLAYFABENTITY_API

#include <playfab/PlayFabEvent.h>
#include <playfab/PlayFabPlatformTypes.h>

namespace PlayFab
{
    PlayFabEvent::PlayFabEvent()
    {
        this->eventContents.EventNamespace = "com.playfab.events.default";
    }

    void PlayFabEvent::SetName(const String& eventName)
    {
        this->eventContents.Name = eventName;
    }

    const String& PlayFabEvent::GetName() const
    {
        return this->eventContents.Name;
    }

    void PlayFabEvent::SetNamespace(const String& eventNamespace)
    {
        this->eventContents.EventNamespace = eventNamespace;
    }

    void PlayFabEvent::SetEntity(const EventsModels::EntityKey& entity)
    {
        this->eventContents.Entity = entity;
    }

    void PlayFabEvent::SetProperty(const String& name, const String& value)
    {
        this->eventContents.Payload[name.data()] = value.data();
    }

    void PlayFabEvent::SetProperty(const String& name, const bool value)
    {
        this->eventContents.Payload[name.data()] = value;
    }

    void PlayFabEvent::SetProperty(const String& name, const int8_t value)
    {
        this->eventContents.Payload[name.data()] = value;
    }

    void PlayFabEvent::SetProperty(const String& name, const int16_t value)
    {
        this->eventContents.Payload[name.data()] = value;
    }

    void PlayFabEvent::SetProperty(const String& name, const int32_t value)
    {
        this->eventContents.Payload[name.data()] = value;
    }

    void PlayFabEvent::SetProperty(const String& name, const int64_t value)
    {
#ifdef PLAYFAB_PLATFORM_PLAYSTATION
        this->eventContents.Payload[name.data()] = value;
#elif defined(PLAYFAB_PLATFORM_ANDROID)
        this->eventContents.Payload[name.data()] = (Int64)value;
#elif defined(JSON_HAS_INT64)
        this->eventContents.Payload[name.data()] = (Json::Int64)value;
#else // PLAYFAB_PLATFORM_ANDROID
        this->eventContents.Payload[name.data()] = (long long int)value;
#endif // PLAYFAB_PLATFORM_ANDROID
    }

    void PlayFabEvent::SetProperty(const String& name, const uint8_t value)
    {
        this->eventContents.Payload[name.data()] = value;
    }

    void PlayFabEvent::SetProperty(const String& name, const uint16_t value)
    {
        this->eventContents.Payload[name.data()] = value;
    }

    void PlayFabEvent::SetProperty(const String& name, const uint32_t value)
    {
        this->eventContents.Payload[name.data()] = value;
    }

    void PlayFabEvent::SetProperty(const String& name, const uint64_t value)
    {
#ifdef PLAYFAB_PLATFORM_PLAYSTATION
        this->eventContents.Payload[name.data()] = value;
#elif defined(PLAYFAB_PLATFORM_ANDROID)
        this->eventContents.Payload[name.data()] = (Uint64)value;
#elif defined(JSON_HAS_INT64)
        this->eventContents.Payload[name.data()] = (Json::UInt64)value;
#else // PLAYFAB_PLATFORM_ANDROID
        this->eventContents.Payload[name.data()] = (long long unsigned int)value;
#endif // PLAYFAB_PLATFORM_ANDROID
    }

    void PlayFabEvent::SetProperty(const String& name, const double value)
    {
        this->eventContents.Payload[name.data()] = value;
    }
}

#endif