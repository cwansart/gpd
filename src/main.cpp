/**
 * @author Christian Wansart
 * @author Larissa Schenk
 * @since 2017-06-14
 *
 * Licensed under the terms of the MIT license. See LICENSE file in project root.
 */

#include <iostream>
#include <boost/asio.hpp>
#include "TcpServer.h"
#include "OgdfTest.h"

using boost::asio::io_service;

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "GpdServer requires at least one parameter. For example:" << std::endl
                  << argv[0] << " /etc/ssl/myPemFile.pem" << std::endl
                  << argv[0] << " /etc/ssl/myPemFile.pem pemFilePassword" << std::endl;
        exit(1);
    }

    std::string pemFile, pemPassword;
    pemFile = argv[1];
    pemPassword = argv[2];

    try
    {
        io_service io_service;
        TcpServer server(io_service, pemFile, pemPassword, [](std::string machine) -> std::string {

            // Call the ogdf functions...
            OgdfTest o(machine);
            o.graphToPlanarRep();

            return o.planarRepToJSON();
        });
        io_service.run();
    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
