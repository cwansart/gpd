#ifndef __TCP_CONNECTION__
#define __TCP_CONNECTION__

#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

class TcpConnection
	: public boost::enable_shared_from_this<TcpConnection>
{
	boost::asio::ip::tcp::socket m_socket;
	std::string m_message;

	TcpConnection(boost::asio::io_service &io_service);
	void handleWrite();

public:
	typedef boost::shared_ptr<TcpConnection> Pointer;

	static Pointer create(boost::asio::io_service &io_service)
	{
		return Pointer(new TcpConnection(io_service));
	}

	boost::asio::ip::tcp::socket &getSocket()
	{
		return m_socket;
	}

	void start();
};

#endif
