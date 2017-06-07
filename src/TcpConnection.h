#ifndef __TCP_CONNECTION__
#define __TCP_CONNECTION__

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class TcpConnection
	: public std::enable_shared_from_this<TcpConnection>
{
	boost::asio::ip::tcp::socket m_socket;
	std::string m_message;

	void handleWrite();

public:
    TcpConnection(boost::asio::io_service &io_service);

	boost::asio::ip::tcp::socket &getSocket()
	{
		return m_socket;
	}

	void start();
};

#endif
