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
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const bool value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const int8_t value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const int16_t value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const int32_t value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const int64_t value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const uint8_t value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const uint16_t value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const uint32_t value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const uint64_t value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }

    void PlayFabEvent::SetProperty(const String& name, const double value)
    {
        JsonUtils::ObjectAddMember(eventContents.Payload, JsonUtils::ToJson(name), value);
    }
}

#endif