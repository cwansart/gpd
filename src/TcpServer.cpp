/**
 * @author Christian Wansart
 * @since 2017-06-14
 *
 * Licensed under the terms of the MIT license. See LICENSE file in project root.
 */

#include "TcpServer.h"

using boost::bind;
using boost::asio::io_service;
using boost::asio::ip::tcp;

TcpServer::TcpServer(io_service &io_service,
                     std::string dhFile,
                     std::string pemFile,
                     std::string keyFile,
                     std::string keyPassword,
                     std::function<std::string(std::string)> processingCallback)
    : m_acceptor(io_service, tcp::endpoint(tcp::v4(), m_port)),
      m_processingCallback(processingCallback),
      m_context(boost::asio::ssl::context::sslv23),
      m_dhFile(dhFile),
      m_pemFile(pemFile),
      m_keyFile(keyFile),
      m_keyPassword(keyPassword)
{
    m_context.set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use
    );

    if (m_keyPassword.length() > 0) {
        m_context.set_password_callback(boost::bind(&TcpServer::get_password, this));
    }
    
    m_context.use_certificate_chain_file(m_pemFile);
    m_context.use_private_key_file(m_keyFile, boost::asio::ssl::context::pem);

    // The dh2048.pem file can be created with the following command:
    // openssl dhparam -out dh2048.pem 2048
    m_context.use_tmp_dh_file(m_dhFile);

    startAccept();
}

/**
 * Creates new TcpConnection objects for every connection and calls handleAccept for it.
 *
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpServer::startAccept()
{
    auto newConnection = std::make_shared<TcpConnection>(m_acceptor.get_io_service(), m_context, m_processingCallback);

    m_acceptor.async_accept(
        newConnection->socket(),
        bind(&TcpServer::handleAccept,
            this,
            newConnection,
            boost::asio::placeholders::error)
    );
}

/**
 * If no errors occurred TcpConnection::processRequest() gets called. After that a new connection will be accepted.
 *
 * @param clientConnection the instantiated TcpConnection object passed from startAccept
 * @param error Boost.Asio's error object
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpServer::handleAccept(std::shared_ptr<TcpConnection> clientConnection, const boost::system::error_code &error)
{
    if (!error) {
        clientConnection->processRequest();
    }

    startAccept();
}

/**
 * Returns the private key password.
 *
 * @return private key password
 * @author Christian Wansart
 * @since 2017-08-25
 */
std::string TcpServer::get_password() const
{
    return m_keyPassword;
}
