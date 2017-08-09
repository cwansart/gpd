/**
 * @author Larissa Schenk
 * @since 2017-06-14
 *
 * Licensed under the terms of the MIT license. See LICENSE file in project root.
 */

#ifndef __OGDF_TEST__
#define __OGDF_TEST__

#include <ogdf/basic/graph_generators.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/module/PlanarityModule.h>
#include <ogdf/planarity/MaximumPlanarSubgraph.h>
#include <ogdf/basic/GraphCopy.h>
#include <ogdf/planarlayout/SchnyderLayout.h>
#include <ogdf/planarlayout/PlanarStraightLayout.h>
#include <ogdf/planarlayout/TriconnectedShellingOrder.h>

class OgdfTest{
    ogdf::Graph G;
    ogdf::GraphCopy GC;
    ogdf::GraphAttributes GA;
    bool graphIsPlanar;

    //Helper Methods
    ogdf::node *getNodeByIndex(int index);
    bool transitionAlreadyExists(int iSource, int iTarget);

public:
    OgdfTest(std::string &JSONstrng);

    void JSONMachineToGraph(std::string &JSONstrng);
    void graphToPlanarRep();
    std::string planarRepToJSON();
};

#endif