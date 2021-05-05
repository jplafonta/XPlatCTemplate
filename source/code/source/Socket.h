#pragma once

#if defined(PLAYFAB_PLATFORM_WINDOWS) || defined(PLAYFAB_PLATFORM_XBOX)
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#if !defined (PLAYFAB_PLATFORM_PLAYSTATION)
#include <netdb.h>
#endif // !defined (PLAYFAB_PLATFORM_PLAYSTATION)
constexpr int SOCKET_ERROR = -1;
constexpr int WSAEWOULDBLOCK = 10035;
#endif // defined(PLAYFAB_PLATFORM_WINDOWS) || defined(PLAYFAB_PLATFORM_XBOX)

namespace PlayFab
{

class Socket
{
public:
    static Result<SharedPtr<Socket>> Make();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    ~Socket();

    // Set the address the socket would ping
    HRESULT SetAddress(const char* socketAddr);

    // Set the port the socket would ping
    HRESULT SetPort(int port);

    // Set the timeout for an individual socket call
    HRESULT SetTimeout(int timeout);

    // Send the message and return the error code
    HRESULT SendMessage(const char* message);

    // Receive the reply and return the error code
    HRESULT ReceiveReply(char* buf, const int& buflen);

    // Get last returned error. Varies depending on platform
    int GetLastError() const;

private:
    Socket();

    // Translates a returned error to HRESULT
    HRESULT TranslateError(int error) const;

private:
    struct sockaddr_in m_siOther;
    struct timeval m_timeOutVal;

#if defined(PLAYFAB_PLATFORM_WINDOWS) || defined(PLAYFAB_PLATFORM_XBOX)
    SOCKET m_socket{ 0 };
    int m_slen;
    WSADATA m_wsa{};
#elif defined(PLAYFAB_PLATFORM_ANDROID) || defined(PLAYFAB_PLATFORM_LINUX)
    unsigned int m_socket{ 0 };
    socklen_t m_slen;
#else
    unsigned int m_socket{ 0 };
    unsigned int m_slen;
#endif // defined(PLAYFAB_PLATFORM_WINDOWS) || defined(PLAYFAB_PLATFORM_XBOX)
};

} // namespace PlayFab
