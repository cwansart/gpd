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

int main()
{
    try
    {
        io_service io_service;
        TcpServer server(io_service, [](std::string machine) -> std::string {

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
