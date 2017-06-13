#ifndef __TCP_CONNECTION__
#define __TCP_CONNECTION__

#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class TcpConnection
    : public std::enable_shared_from_this<TcpConnection>
{
    enum class Type
    {
        GET,
        POST,
        OPTIONS,
        UNKNOWN
    };

    boost::asio::ip::tcp::socket m_socket;
    std::stringstream m_message;
    std::array<char, 8192> m_buffer;
    bool m_headerFound;
    bool m_readComplete;
    int m_packageCounter;
    size_t m_headerSize;
    size_t m_transferredTotal;
    size_t m_contentLength;
    Type m_packageType;

    std::function<std::string(std::string)> m_processingCallback;

    void handleRead(const boost::system::error_code &error, std::size_t bytesTransferred);
    void processMessage();

    void extractContentLength(const std::string &buf);
    void extractHeaderAndBody(const std::string &buf, const std::size_t bytesTransferred);
    void processPackage(const std::string &buf, const std::size_t bytesTransferred);

    void handleGetRequest();
    void handlePostRequest();
    void handleOptionsRequest();
    void handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred);

public:
    TcpConnection(boost::asio::io_service &io_service, std::function<std::string(std::string)> processingCallback);
    void processRequest();

    boost::asio::ip::tcp::socket &getSocket()
    {
        return m_socket;
    }
};

#endif
