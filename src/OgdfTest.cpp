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

        int iSource = edge["from"];
        int iTarget = edge["to"];

        // TwoWay Transitions
        if(edge["type"] == 2){
            if(!transitionAlreadyExists(iTarget, iSource))
                G.newEdge(*getNodeByIndex(iSource), *getNodeByIndex(iTarget));
        }
        // OneWay Transitions
        else if(edge["type"] == 0){
            G.newEdge(*getNodeByIndex(iSource), *getNodeByIndex(iTarget));
        }
        // Recursive Transisitons are ignored ..
    }
}

void OgdfTest::graphToPlanarRep(){
    GA = GraphAttributes(G);
    GA.addAttributes(GraphAttributes::edgeType);
    GA.addAttributes(GraphAttributes::nodeType);

    BoyerMyrvold BM;
    if (BM.isPlanar(G) && G.numberOfNodes()) {
        PlanarStraightLayout PSL;
        PSL.call(GA);

        GraphIO::write(GA, "planStraight.svg", GraphIO::drawSVG);   // Delete this later
    }
    else {
        PlanarizationLayout PL;
        PL.call(GA);

        GraphIO::write(GA, "ortho.svg", GraphIO::drawSVG);          // Delete this later

        SpringEmbedderGridVariant se;
        se.call(GA);

        GraphIO::write(GA, "straight.svg", GraphIO::drawSVG);        // Delete this later
    }
}

// INFO: Mention that we only need the new positions of the states/nodes
// and the ID of every node. So we can change the position of every
// node in the toolbox.
std::string OgdfTest::planarRepToJSON(){
    int counter = 0;
    json j;
    j["states"] = json::array();

    for (node vG : G.nodes) {
        j["states"][counter] = { { "id", vG->index() }, {"xPos:", GA.x(vG)},  {"yPos:", GA.y(vG)}};
        counter++;
    }

    return j.dump();
}

node *OgdfTest::getNodeByIndex(int index){
    node *n = NULL;
    for (node vG : G.nodes) {
        if(vG->index() == index){
            n = &vG;
            return n;
        }
    }
    return NULL;
}

bool OgdfTest::transitionAlreadyExists(int iSource, int iTarget){
    for (edge eG : G.edges) {
        if(eG->source()->index() == iSource && eG->target()->index() == iTarget)
            return true;
    }
    return false;
}