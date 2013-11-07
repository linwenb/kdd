#include "kdd.h"

void run (const char inputfile[], const char outputfile[]) {

	PNGraph G = LoadEdgeList<PNGraph>(inputfile);
	
	vector<node> * v =  genMP(G);

}

vector<node> * genMP (PNGraph Graph, float RF, float DT) {
	
	vector<node> * v = new vector<node>();

	// current K
	int k = SIZE;

	while (!Graph->Empty()) {
		// random node u with nonzero degree
		TNGraph::TNodeI iter = Graph->GetRndNI();
		
		// append u to L
		node n;
		n.id = iter.GetId();
		n.next = iter.GetId();
		v->push_back(n);

		printf("Number of nodes: %d\n", n.id);
		// X <- last k vertices in L

		while (false) {
			// node v most edges to/from X
			
			// remove edges between v and X
			int edgeCount = 0;
			
			// append v to L

			// decrease k if necessary
			if (v->size() % 1000 == 0) {
				if (edgeCount < 2000 * DT * k) {
					k *= RF;
				}
				edgeCount = 0;
			}
		}
	}

	return v;
}

void saveMP (vector<node> * v, const char outputfile[]) {
	
}
