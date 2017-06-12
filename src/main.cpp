#include <iostream>
#include <boost/asio.hpp>
#include "TcpServer.h"
#include "OgdfTest.h"

using boost::asio::io_service;

int main()
{
    // This is an example of a JSON-string
    // we need to build an ogdf::Graph.
    // TODO: In toolbox: Transitions:
    // -Add the transition type as an integer
    // -Change the Datatype of "from" and "to"
    // to integer
    std::string machineRep = "{\"states\":[  \n"
            "        {  \n"
            "            \"id\":0,\n"
            "            \"title\":\"q0\",\n"
            "            \"initial\":false,\n"
            "            \"final\":false,\n"
            "            \"xPos\":330,\n"
            "            \"yPos\":315\n"
            "        },\n"
            "        {  \n"
            "            \"id\":1,\n"
            "            \"title\":\"q1\",\n"
            "            \"initial\":false,\n"
            "            \"final\":false,\n"
            "            \"xPos\":331,\n"
            "            \"yPos\":135\n"
            "        },\n"
            "        {  \n"
            "            \"id\":2,\n"
            "            \"title\":\"q2\",\n"
            "            \"initial\":false,\n"
            "            \"final\":false,\n"
            "            \"xPos\":524,\n"
            "            \"yPos\":133\n"
            "        },\n"
            "        {  \n"
            "            \"id\":3,\n"
            "            \"title\":\"q3\",\n"
            "            \"initial\":false,\n"
            "            \"final\":false,\n"
            "            \"xPos\":534,\n"
            "            \"yPos\":322\n"
            "        }\n"
            "    ],\n"
            "    \"transitions\":[  \n"
            "        {  \n"
            "            \"idTransition\":\"348\",\n"
            "            \"type\":1,\n"
            "            \"machine_idMachine\":\"31\",\n"
            "            \"token\":\"\",\n"
            "            \"from_idState\":\"243\",\n"
            "            \"to_idState\":\"243\",\n"
            "            \"anchorPointX\":\"194\",\n"
            "            \"anchorPointY\":\"284\",\n"
            "            \"creationDate\":\"2017-06-08 09:26:20\",\n"
            "            \"updateDate\":\"2017-06-08 09:26:20\",\n"
            "            \"from\":0,\n"
            "            \"to\":0\n"
            "        },\n"
            "        {  \n"
            "            \"idTransition\":\"349\",\n"
            "            \"type\":0,\n"
            "            \"machine_idMachine\":\"31\",\n"
            "            \"token\":\"\",\n"
            "            \"from_idState\":\"244\",\n"
            "            \"to_idState\":\"245\",\n"
            "            \"anchorPointX\":null,\n"
            "            \"anchorPointY\":null,\n"
            "            \"creationDate\":\"2017-06-08 09:26:20\",\n"
            "            \"updateDate\":\"2017-06-08 09:26:20\",\n"
            "            \"from\":1,\n"
            "            \"to\":2\n"
            "        },\n"
            "        {  \n"
            "            \"idTransition\":\"350\",\n"
            "            \"type\":0,\n"
            "            \"machine_idMachine\":\"31\",\n"
            "            \"token\":\"\",\n"
            "            \"from_idState\":\"244\",\n"
            "            \"to_idState\":\"243\",\n"
            "            \"anchorPointX\":null,\n"
            "            \"anchorPointY\":null,\n"
            "            \"creationDate\":\"2017-06-08 09:26:20\",\n"
            "            \"updateDate\":\"2017-06-08 09:26:20\",\n"
            "            \"from\":1,\n"
            "            \"to\":0\n"
            "        },\n"
            "        {  \n"
            "            \"idTransition\":\"351\",\n"
            "            \"type\":0,\n"
            "            \"machine_idMachine\":\"31\",\n"
            "            \"token\":\"\",\n"
            "            \"from_idState\":\"246\",\n"
            "            \"to_idState\":\"243\",\n"
            "            \"anchorPointX\":null,\n"
            "            \"anchorPointY\":null,\n"
            "            \"creationDate\":\"2017-06-08 09:26:20\",\n"
            "            \"updateDate\":\"2017-06-08 09:26:20\",\n"
            "            \"from\":3,\n"
            "            \"to\":0\n"
            "        },\n"
            "        {  \n"
            "            \"idTransition\":\"352\",\n"
            "            \"type\":2,\n"
            "            \"machine_idMachine\":\"31\",\n"
            "            \"token\":\"\",\n"
            "            \"from_idState\":\"245\",\n"
            "            \"to_idState\":\"246\",\n"
            "            \"anchorPointX\":null,\n"
            "            \"anchorPointY\":null,\n"
            "            \"creationDate\":\"2017-06-08 09:26:20\",\n"
            "            \"updateDate\":\"2017-06-08 09:26:20\",\n"
            "            \"from\":2,\n"
            "            \"to\":3\n"
            "        },\n"
            "        {  \n"
            "            \"idTransition\":\"353\",\n"
            "            \"type\":2,\n"
            "            \"machine_idMachine\":\"31\",\n"
            "            \"token\":\"\",\n"
            "            \"from_idState\":\"246\",\n"
            "            \"to_idState\":\"245\",\n"
            "            \"anchorPointX\":null,\n"
            "            \"anchorPointY\":null,\n"
            "            \"creationDate\":\"2017-06-08 09:26:20\",\n"
            "            \"updateDate\":\"2017-06-08 09:26:20\",\n"
            "            \"from\":3,\n"
            "            \"to\":2\n"
            "        },\n"
            "        {  \n"
            "            \"idTransition\":\"354\",\n"
            "            \"type\":0,\n"
            "            \"machine_idMachine\":\"31\",\n"
            "            \"token\":\"\",\n"
            "            \"from_idState\":\"244\",\n"
            "            \"to_idState\":\"246\",\n"
            "            \"anchorPointX\":null,\n"
            "            \"anchorPointY\":null,\n"
            "            \"creationDate\":\"2017-06-08 09:26:20\",\n"
            "            \"updateDate\":\"2017-06-08 09:26:20\",\n"
            "            \"from\":1,\n"
            "            \"to\":3\n"
            "        },\n"
            "        {  \n"
            "            \"idTransition\":\"355\",\n"
            "            \"type\":0,\n"
            "            \"machine_idMachine\":\"31\",\n"
            "            \"token\":\"\",\n"
            "            \"from_idState\":\"243\",\n"
            "            \"to_idState\":\"245\",\n"
            "            \"anchorPointX\":null,\n"
            "            \"anchorPointY\":null,\n"
            "            \"creationDate\":\"2017-06-08 09:26:20\",\n"
            "            \"updateDate\":\"2017-06-08 09:26:20\",\n"
            "            \"from\":0,\n"
            "            \"to\":2\n"
            "        },\n"
            "        {  \n"
            "            \"idTransition\":\"356\",\n"
            "            \"type\":1,\n"
            "            \"machine_idMachine\":\"31\",\n"
            "            \"token\":\"\",\n"
            "            \"from_idState\":\"244\",\n"
            "            \"to_idState\":\"244\",\n"
            "            \"anchorPointX\":\"331\",\n"
            "            \"anchorPointY\":\"121\",\n"
            "            \"creationDate\":\"2017-06-08 09:26:20\",\n"
            "            \"updateDate\":\"2017-06-08 09:26:20\",\n"
            "            \"from\":1,\n"
            "            \"to\":1\n"
            "        }\n"
            "    ]}";


    /*OgdfTest ogdf(machineRep);
    ogdf.graphToPlanarRep();
    std::cout << "Positions for the planar representation:\n" << ogdf.planarRepToJSON() << std::endl;*/

    try
    {
        io_service io_service;
        TcpServer server(io_service, [](std::string machine) -> std::string {
            std::cout << "Called inside processMessage" << std::endl;
            std::cout << "MACHINE BLUB" << std::endl << machine << std::endl;

             // Call the ogdf functions...
            OgdfTest o(machine);
            o.graphToPlanarRep();
            o.planarRepToJSON(); // Send this string back to the toolbox

            return "";
        });
        io_service.run();
    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
