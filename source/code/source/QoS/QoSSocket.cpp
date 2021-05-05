#include "stdafx.h"
#include "QoS.h"
#include "QoSSocket.h"
#include "TaskQueue.h"

namespace PlayFab
{
namespace QoS
{

// Clear the buffer with this value before receiving the reply.
constexpr char BUFFER_VALUE = '1';

// Port the QoS server listen on
constexpr int PORT = 3075;

Result<SharedPtr<QoSSocket>> QoSSocket::MakeAndConfigure(int timeoutMs)
{
    auto socketResult = Socket::Make();
    if (Failed(socketResult))
    {
        return socketResult.hr;
    }

    auto socket = socketResult.ExtractPayload();

    RETURN_IF_FAILED(socket->SetPort(PORT));
    RETURN_IF_FAILED(socket->SetTimeout(timeoutMs));

    return SharedPtr<QoSSocket>{ new (Allocator<QoSSocket>{}.allocate(1)) QoSSocket{ std::move(socket) } };
}

QoSSocket::QoSSocket(SharedPtr<Socket> socket) : m_socket{ std::move(socket) }
{
}

Result<uint32_t> QoSSocket::PingServer(const char* address)
{
    RETURN_IF_FAILED(m_socket->SetAddress(address));

    // Clear the buffer
    memset(buf, BUFFER_VALUE, BUFLEN);

    // Snap the start time
    auto begin = Clock::now();

    // Send the message & await the reply
    RETURN_IF_FAILED(m_socket->SendMessage(message));
    RETURN_IF_FAILED(m_socket->ReceiveReply(buf, BUFLEN));

    // Snap the end time
    auto end = Clock::now();

    // TODO : Can add error checking for the reply and return based on that
    // The first 2 bytes should be set to 0s.

    // Calculate the total time
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
}

} // namespace QoS
} // namespace PlayFab
