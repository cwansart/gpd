#include <iostream>
#include <boost/asio.hpp>
#include "TcpServer.h"
#include "OgdfTest.h"

using boost::asio::io_service;

int main()
{
    OgdfTest ogdf;
    /*try
    {
        io_service io_service;
        TcpServer server(io_service);
        io_service.run();
    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }*/
}
