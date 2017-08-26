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
    if (argc != 4) {
        std::cerr << "GpdServer requires 3 parameters. The a dh.pem file, the certificate (pem) file and the private key file." << std::endl;
        exit(1);
    }

    std::string dhFile, pemFile, keyFile, keyPassword;
    dhFile = argv[1];
    pemFile = argv[2];
    keyFile = argv[3];

    if (argc > 4) {
        keyPassword = argv[4];
    }

    try
    {
        io_service io_service;
        TcpServer server(io_service, dhFile, pemFile, keyFile, keyPassword, [](std::string machine) -> std::string {

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
