#include "kdd.h"

void run (const char inputfile[], const char outputfile[]) {

	PNGraph G = LoadEdgeList<PNGraph>(inputfile);
	
	vector<node> v;
	genMP(v, G);

	saveMP (v, outputfile);
	vector<node> v2(v.size());
	loadMP (v2, outputfile);
	cout << v2[0].left << endl;
}

void genMP (vector<node> & v, PNGraph Graph, float RF, float DT) {

	// current K
	int k = SIZE;

	while (!Graph->Empty()) {
		// random node u with nonzero degree
		TNGraph::TNodeI iter = Graph->GetRndNI();
		
		// append u to L
		node n;
		n.id = iter.GetId();
		n.next = iter.GetId();
		v.push_back(n);

		cout << v[0].left << endl;
break;
		// X <- last k vertices in L

		while (false) {
			// node v most edges to/from X
			
			// remove edges between v and X
			int edgeCount = 0;
			
			// append v to L

			// decrease k if necessary
			if (v.size() % 1000 == 0) {
				if (edgeCount < 2000 * DT * k) {
					k *= RF;
				}
				edgeCount = 0;
			}
		}
	}

}

void saveMP (const vector<node> & v, const char outputfile[]) {
	ofstream FILE;
	FILE.open(outputfile, ios::out|ios::binary);
	FILE.write(reinterpret_cast<const char*>(&v), sizeof(v));
	FILE.close();
}

void loadMP (vector<node> & v, const char inputfile[]) {
	ifstream FILE;
	FILE.open(inputfile, ios::in|ios::binary);
	FILE.seekg (0, ios::end);
	int length = FILE.tellg();
	FILE.read(reinterpret_cast<char*>(&v), length);
	FILE.close();
}
