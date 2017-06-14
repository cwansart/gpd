#include "OgdfTest.h"

#include <iostream>
#include <json.hpp>

using namespace ogdf;
using namespace nlohmann;

/**
* Constructor, calls JSONMachineToGraph to convert the machine to an ogdf::Graph
*
* @param    JSONstrng, machine as a JSON string
* @author   laschenk
* @since    2017-06-08
*/
OgdfTest::OgdfTest(std::string &JSONstrng): graphIsPlanar(false) {
    JSONMachineToGraph(JSONstrng);
}

/**
* Converts the machine to a processable ogdf::Graph (ignores i.e. recursive transitions)
*
* @param    JSONstrng, machine as a JSON string
* @author   laschenk
* @since    2017-06-08
*/
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

/**
* Proofs if the graph is (not) planar, creates a (mostly) planar layout and new positions for the nodes
*
* @author   laschenk
* @since    2017-06-08
*/
void OgdfTest::graphToPlanarRep(){
    GA = GraphAttributes(G);
    GA.addAttributes(GraphAttributes::edgeType);
    GA.addAttributes(GraphAttributes::nodeType);
    randomSimpleGraph(G, 6, 8);

    BoyerMyrvold BM;
    if (BM.isPlanar(G)) {
        graphIsPlanar = true;

        FPPLayout FPPL;
        FPPL.call(GA);

        GraphIO::write(GA, "planStraight.svg", GraphIO::drawSVG);   // TODO: Delete this later
    }
    else {
        graphIsPlanar = false;

        List<edge> delEdges;
        GC = GraphCopy(G);
        GA = GraphAttributes(GC);
        MaximumPlanarSubgraph MPS;

        MPS.callAndDelete(GC, delEdges);

        FPPLayout FPPL;
        FPPL.call(GA);

        GraphIO::write(GA, "maxPlanSubgraphStraight.svg", GraphIO::drawSVG);    // TODO: Delete this later
    }
}

/**
* Saves the new node-positions in a JSON string, that is send to the toolbox later
*
* @author   laschenk
* @since    2017-06-08
* @return   string, the JSON string
*/
std::string OgdfTest::planarRepToJSON() {
    int counter = 0;
    json j;
    j["states"] = json::array();

    // Use the original Graph
    if (graphIsPlanar){
        for (node vG : G.nodes) {
            j["states"][counter] = {{"id",   vG->index()}, {"xPos", GA.x(vG)}, {"yPos", GA.y(vG)}};
            counter++;
        }
    }
    // Use the copy
    else{
        for (node vG : GC.nodes) {
            j["states"][counter] = {{"id",   vG->index()}, {"xPos", GA.x(vG)}, {"yPos", GA.y(vG)}};
            counter++;
        }
    }
    return j.dump();
}

/**
 *
 * Helper methods
 * @author laschenk
 */

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