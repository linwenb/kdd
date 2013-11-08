#include "kdd.h"

void run (const char inputfile[], const char outputfile[]) {

	PNGraph G = LoadEdgeList<PNGraph>(inputfile);
	
	vector<node> v;
	genMP(v, G);

	saveMP (v, outputfile);

	vector<node> v2;
	loadMP (v2, outputfile);

}

void genMP (vector<node> & MP, PNGraph & Graph, float RF, float DT) {

	// current K
	int k = SIZE;

	map<int, int> pos;
	map<int, int> neighbor;
	set<int> X;

	while (!Graph->Empty()) {
		// random node u with nonzero degree
		TNGraph::TNodeI iter = Graph->GetRndNI();
		
		// append u to L
		appendNode(MP, iter, Graph, X, pos, neighbor);

		printf("MP size %d\n", MP.size());
		printf("pos size %d\n", pos.size());
		printf("neighbor size %d\n", neighbor.size());
break;
		// X <- last k vertices in L

		while (!neighbor.empty()) {
			// node v most edges to/from X
			int u = getMostNeighNode(neighbor);
			
			// remove edges between v and X
			int edgeCount = 0;
			
			// append v to L

			// decrease k if necessary
			if (MP.size() % 1000 == 0) {
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

void appendNode (vector<node> & MP, const TNGraph::TNodeI & idIter, PNGraph & Graph, set<int> & X, map<int, int> & pos, map<int, int> & neighbor) {

	int tempId;
	set<int>::iterator setIter;
	map<int, int>::iterator mapIter;
	set<int> inNode;
	set<int> outNode;

	int endMP = MP.size();
	int id = idIter.GetId();

	node n;

	n.id = id;
	mapIter = pos.find(id);
	
	// update pos
	if (mapIter == pos.end()) {
		pos.insert( make_pair(id, endMP) );
		n.next = endMP;
	}
	else {
		int last = mapIter->second;
		mapIter->second = endMP;

		int temp = MP[last].next;
		MP[last].next = endMP;
		n.next = temp;
	}

	// update neighbor


	// update MP, X and 
	X.insert(id);

	if (endMP >= SIZE) {
		
		X.erase(MP[endMP-SIZE].id);
	}
	else {

	}
	
	MP.push_back(n);
}

void addNeigh (const TNGraph::TNodeI & idIter, set<int> & X, map<int, int> & neighbor, set<int> & inNode, set<int> & outNode) {
	set<int>::iterator setIter;
	map<int, int>::iterator mapIter;
	int tempId;

	for (int e = 0; e < idIter.GetInDeg(); e++) {
		tempId = idIter.GetInNId(e);
		setIter = X.find(tempId);
		if (setIter == X.end()) {
			mapIter = neighbor.find(tempId);
			if (mapIter == neighbor.end()) {
				neighbor.insert(make_pair(tempId,1));
			}
			else {
				mapIter->second ++;
			}
		}
		else {
			inNode.insert(tempId);
		}
	}

	for (int e = 0; e < idIter.GetOutDeg(); e++) {
		tempId = idIter.GetOutNId(e);
		setIter = X.find(tempId);
		if (setIter == X.end()) {
			mapIter = neighbor.find(tempId);
			if (mapIter == neighbor.end()) {
				neighbor.insert(make_pair(tempId,1));
			}
			else {
				mapIter->second ++;
			}
		}
		else {
			inNode.insert(tempId);
		}
	}
}

int getMostNeighNode (map<int, int> & neighbor) {
	vector< pair<int,int> > v;
	for (map<int,int>::iterator curr = neighbor.begin(); curr != neighbor.end() ;curr++) {
		v.push_back(make_pair(curr->first, curr->second) );
	}
	sort(v.begin(), v.end(), cmp);

	// delete the node from neighbor map
	int key = v[0].first;
	neighbor.erase(key);

	return key;
}

bool cmp (const pair<int, int> & x, const pair<int, int> & y) {
	return x.second > y.second;
}
