#pragma once

#include "TestCase.h"

namespace PlayFabUnit
{
    class AsyncOpTests : public TestCase
    {
    private:
        void BasicContinuationTest(TestContext& testContext);
        void ChainedOperationTest(TestContext& testContext);
        void ContinuationExceptionTest(TestContext& testContext);

    protected:
        void AddTests() override;

    public:
        void Tick(TestContext&) override {}
    };
}
