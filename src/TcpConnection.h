/**
 * @author Christian Wansart
 * @since 2017-06-14
 *
 * Licensed under the terms of the MIT license. See LICENSE file in project root.
 */

#ifndef __TCP_CONNECTION__
#define __TCP_CONNECTION__

#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ssl.hpp>

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

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

    ssl_socket m_socket;

    std::stringstream m_message;
    std::array<char, 32768> m_buffer;
    std::string m_writeBuffer;
    bool m_headerFound;
    bool m_readComplete;
    int m_packageCounter;
    size_t m_headerSize;
    size_t m_transferredTotal;
    size_t m_contentLength;
    Type m_packageType;

    void handle_handshake(const boost::system::error_code &error);

    std::function<std::string(std::string)> m_processingCallback;

    void handleRead(const boost::system::error_code &error, std::size_t bytesTransferred);
    void processMessage();

    void extractContentLength(const std::string &buf);
    void extractHeaderAndBody(const std::string &buf, const std::size_t bytesTransferred);
    void splitHeaderAndBody(const std::string &buf);
    void processPackage(const std::string &buf, const std::size_t bytesTransferred);

    void handleGetRequest();
    void handlePostRequest(const std::string &buf, const std::size_t bytesTransferred);
    void handleOptionsRequest();
    void handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred);



public:
    TcpConnection(boost::asio::io_service &io_service, boost::asio::ssl::context &context, std::function<std::string(std::string)> processingCallback);
    void processRequest();

    // TODO: should be moved to cpp file.
    ssl_socket::lowest_layer_type& socket()
    {
        return m_socket.lowest_layer();
    }
};

#endif
