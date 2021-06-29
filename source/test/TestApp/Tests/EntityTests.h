#pragma once

#include "TestCase.h"
#include <playfab/PlayFabGlobal.h>
#include <playfab/PlayFabEntity.h>

namespace PlayFabUnit
{

class EntityTests : public PlayFabApiTestCase
{
private:
    void TestClientLogin(TestContext& testContext);
    void TestManualTokenRefresh(TestContext& testContext);

protected:
    void AddTests() override;

public:
    PlayFabStateHandle stateHandle{ nullptr };

    void ClassSetUp() override;
    void ClassTearDown() override;
    void SetUp(TestContext& testContext) override;

    void Tick(TestContext&) override {}
};

}
