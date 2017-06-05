#include "OgdfTest.h"

#include <iostream>

#include <ogdf/basic/graph_generators.h>
#include <ogdf/planarity/SubgraphPlanarizer.h>
#include <ogdf/planarity/PlanarSubgraphFast.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>
#include <ogdf/fileformats/GraphIO.h>

using namespace ogdf;

OgdfTest::OgdfTest() {
    Graph G;
    randomSimpleGraph(G, 100, 150);

    SubgraphPlanarizer SP;
    SP.setSubgraph(new PlanarSubgraphFast<int>);
    SP.setInserter(new VariableEmbeddingInserter);

    int crossNum;
    PlanRep PR(G);
    SP.call(PR, 0, crossNum);

    cout << crossNum << " crossings" << endl;
}