#ifndef __TCP_CONNECTION__
#define __TCP_CONNECTION__

#include <string>
#include <sstream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class TcpConnection
    : public std::enable_shared_from_this<TcpConnection>
{
    enum class Type
    {
        GET,
        POST,
        UNKNOWN
    };

    boost::asio::ip::tcp::socket m_socket;
    std::stringstream m_message;
    std::array<char, 8192> m_buffer;
    bool m_headerFound;
    bool m_readComplete;
    bool m_isGetRequest;
    int m_packageCounter;
    Type m_packageType;

    void handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred);
    void handleRead(const boost::system::error_code &error, std::size_t bytesTransferred);
    void processMessage();

    void handleGetRequest();
    void handleGetWrite(const boost::system::error_code &error, std::size_t bytesTransferred);

public:
    TcpConnection(boost::asio::io_service &io_service);
    void processRequest();

    boost::asio::ip::tcp::socket &getSocket()
    {
        return m_socket;
    }

    void start();
};

#endif
