#include "TestAppPch.h"
#include "AsyncOpTests.h"
#include <AsyncOp.h>
#include <thread>

using namespace PlayFab;

namespace PlayFabUnit
{

AsyncOp<void> AsyncSleep(uint32_t sleepMs)
{
    auto opContext = std::make_shared<AsyncOpContext<void>>();

    std::thread asyncOp([opContext, sleepMs]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
        opContext->Complete(Result<void>{ S_OK });
    });

    asyncOp.detach();

    return AsyncOp<void>{ opContext };
}

AsyncOp<int> AsyncIncrement(int input)
{
    auto opContext = std::make_shared<AsyncOpContext<int>>();

    std::thread asyncOp([opContext, input]()
    {
        opContext->Complete(Result<int>(input + 1));
    });

    asyncOp.detach();
    return AsyncOp<int>{ opContext };
}

void AsyncOpTests::BasicContinuationTest(TestContext& testContext)
{
    const std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    AsyncSleep(100).Finally([start, &testContext](Result<void> result) mutable
    {
        if (Failed(result))
        {
            testContext.Fail();
        }
        else if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() < 100)
        {
            testContext.Fail();
        }
        else
        {
            testContext.Pass();
        }
    });
}

void AsyncOpTests::ChainedOperationTest(TestContext& testContext)
{
    AsyncIncrement(0).Then([](Result<int> result) -> AsyncOp<int>
    {
        if (Failed(result)) 
        {
            return result.hr;
        }
        return AsyncIncrement(result.Payload());
    })
    .Finally([&testContext](Result<int> result) mutable
    {
        if (Failed(result) || result.Payload() != 2)
        {
            testContext.Fail();
        }
        else
        {
            testContext.Pass();
        }
    });
}

void AsyncOpTests::ContinuationExceptionTest(TestContext& testContext)
{
    AsyncSleep(0).Then([](Result<void>) -> Result<void>
    {
        throw std::bad_alloc();
    })
    .Finally([&testContext](Result<void> result) mutable
    {
        if (result.hr == E_OUTOFMEMORY) // std::bad_alloc should get translated to E_OUTOFMEMORY
        {
            testContext.Pass();
        }
        else
        {
            testContext.Fail();
        }
    });
}

void AsyncOpTests::AddTests()
{
    AddTest("BasicContinuationTest", &AsyncOpTests::BasicContinuationTest);
    AddTest("ChainedOperationTest", &AsyncOpTests::ChainedOperationTest);
    AddTest("ContinuationExceptionTest", &AsyncOpTests::ContinuationExceptionTest);
}

}