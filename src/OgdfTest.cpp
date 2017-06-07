#include "OgdfTest.h"

#include <iostream>
#include <json.hpp>

using namespace ogdf;
using namespace nlohmann;

OgdfTest::OgdfTest(std::string &JSONstrng) {
    JSONMachineToGraph(JSONstrng);
}

void OgdfTest::JSONMachineToGraph(std::string &JSONstrng){
    json j = json::parse(JSONstrng);

    // Save the states as nodes
    json states = j["states"];
    for (auto &state: states) {
        G.newNode(state["id"]);
    }

    // Save the transitions as edges
    json edges = j["transitions"];
    for (auto &edge: edges) {
        // TODO: Ignore TwoWay- and Recursive transitions!

        int iSource = edge["from"];
        int iTarget = edge["to"];
        G.newEdge(getNodeByIndex(iSource), getNodeByIndex(iTarget));
    }

    // Save all transitions, that are not uses (recursive, first part of twoWay)
    // ...
}

void OgdfTest::graphToPlanarRep(){
    GA = GraphAttributes(G);
    GA.addAttributes(GraphAttributes::edgeType);
    GA.addAttributes(GraphAttributes::nodeType);
    //randomSimpleGraph(G, 6, 14);

    BoyerMyrvold BM;
    if (BM.isPlanar(G) && G.numberOfNodes()) {
        cout << "Graph is planar." << endl;

        PlanarStraightLayout PSL;
        PSL.call(GA);

        GraphIO::write(GA, "planStraight.svg", GraphIO::drawSVG);
    } else {
        cout << "Graph is not planar." << endl;

        PlanarizationLayout PL;
        PL.call(GA);

        GraphIO::write(GA, "ortho.svg", GraphIO::drawSVG);

        SpringEmbedderGridVariant se;
        se.call(GA);

        GraphIO::write(GA, "straight.svg", GraphIO::drawSVG);
    }


    // We can use GraphAttributes to build the Graph for the Toolbox
    /*cout << "Graphattributes:" << endl;
    for (node vG : G.nodes) {
        cout << "ID:" << vG->index() << ", x:" << GA.x(vG) << ", y:" << GA.y(vG) << endl;
    }
    for (edge eG : G.edges) {
        cout << "ID:" << eG->index() << ", source:" << eG->source() << ", target:" << eG->target() << endl;
    }*/
}

std::string OgdfTest::planarRepToJSON(){

}

node &OgdfTest::getNodeByIndex(int index){
    for (node vG : G.nodes) {
        if(vG->index() == index)
            return vG;
    }
}