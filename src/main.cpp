#include <iostream>
#include <boost/asio.hpp>
#include "TcpServer.h"
#include "OgdfTest.h"

using boost::asio::io_service;

int main()
{
    std::string machineRep = "{\"states\": [\n"
            "      {\n"
            "         \"id\": 0,\n"
            "         \"title\": \"q0\",\n"
            "         \"initial\": true,\n"
            "         \"final\": false,\n"
            "         \"xPos\": 453,\n"
            "         \"yPos\": 177\n"
            "      },\n"
            "      {\n"
            "         \"id\": 1,\n"
            "         \"title\": \"q1\",\n"
            "         \"initial\": false,\n"
            "         \"final\": true,\n"
            "         \"xPos\": 649,\n"
            "         \"yPos\": 176\n"
            "      }\n"
            "   ],\n"
            "   \"transitions\": [\n"
            "      {\n"
            "         \"idTransition\": \"284\",\n"
            "         \"machine_idMachine\": \"29\",\n"
            "         \"token\": \"a\",\n"
            "         \"from_idState\": \"206\",\n"
            "         \"to_idState\": \"207\",\n"
            "         \"anchorPointX\": null,\n"
            "         \"anchorPointY\": null,\n"
            "         \"creationDate\": \"2017-03-27 08:07:46\",\n"
            "         \"updateDate\": \"2017-03-27 08:07:46\",\n"
            "         \"from\": 0,\n"
            "         \"to\": 1\n"
            "      }\n"
            "   ]\n"
            "}";
    OgdfTest ogdf(machineRep);
    ogdf.graphToPlanarRep();
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
