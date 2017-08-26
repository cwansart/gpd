/**
 * @author Christian Wansart
 * @since 2017-06-14
 *
 * Licensed under the terms of the MIT license. See LICENSE file in project root.
 */

#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "TcpConnection.h"

class TcpServer
{
    const unsigned short m_port = 2909;

    const std::string m_dhFile;
    const std::string m_pemFile;
    const std::string m_keyFile;
    const std::string m_keyPassword;

    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ssl::context m_context;
    std::function<std::string(std::string)> m_processingCallback;

    void startAccept();
    void handleAccept(std::shared_ptr<TcpConnection> clientConnection,
                      const boost::system::error_code &error);
    std::string get_password() const;

public:
    TcpServer(boost::asio::io_service &io_service,
              std::string dhFile,
              std::string pemFile,
              std::string keyFile,
              std::string keyPassword,
              std::function<std::string(std::string)> processingCallback);
};

#endif
