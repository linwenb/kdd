#include "kdd.h"

void run (const char inputfile[], const char outputfile[]) {

	printf("Start read graph\n");
	PNGraph G = LoadEdgeList<PNGraph>(inputfile, 0, 1);

	printf("Start generate MP\n");	
	vector<node> v;
	genMP(v, G);

	printf("Start save MP\n");
	saveMP (v, outputfile);

	printf("Start read MP\n");
	vector<node> v2;
	loadMP (v2, outputfile);

}

void genMP (vector<node> & MP, PNGraph & Graph, float RF, float DT) {

	// current K
	int k = SIZE;

	TNGraph::TEdgeI iter;

	map<int, int> pos;
	map<int, int> neighbor;
	set<int> X;

	while (Graph->GetEdges()) {
		// random node d with nonzero degree
		iter = Graph->BegEI();
		
		// append d to L
		appendNode(MP, iter.GetSrcNId(), Graph, X, pos, neighbor);

		// X <- last k vertices in L

		while (!neighbor.empty()) {
			// node u most edges to/from X
			// remove edges between v and X
			int u = getMostNeighNode(neighbor);
						
			int edgeCount = 0;
			
			// append u to L
			appendNode(MP, u, Graph, X, pos, neighbor);

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
	FILE.write(reinterpret_cast<const char*>(&v), sizeof(node)*v.size());
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

void appendNode (vector<node> & MP, const int & id, PNGraph & Graph, set<int> & X, map<int, int> & pos, map<int, int> & neighbor) {

	set<int> inNode;
	set<int> outNode;

	TNGraph::TNodeI idIter = Graph->GetNI(id);

	printf("Add node %d, total %d nodes %d edges left\n", id, Graph->GetNodes(), Graph->GetEdges() );

	node n;

	// update pos
	updatePos (id, n, MP, pos);

	printf("Update neighbor\n");
	// update neighbor
	addNeigh(idIter, X, neighbor, inNode, outNode);

	printf("Update X, now size %d \n", X.size() );
	// update X
	int sizeX = X.size();	

	updateX(SIZE, MP, Graph, X, neighbor);
	X.insert(id);

	printf("Update MP\n");
	// update MP
	updateMP (MP, sizeX, n, Graph, inNode, outNode);
/*
	if (idIter.GetDeg() == 0) {
		Graph->DelNode(id);
	}
*/
	printf("Finished append Node\n");
}

void updateMP (vector<node> & MP, const int & sizeX, const node & n, PNGraph & Graph, set<int> & inNode, set<int> & outNode) {

	int tempId;
	int id = n.id;
	int endMP = MP.size();
	set<int>::iterator setIter;

	for (int i = 1; i <= sizeX; i++) {
		tempId = MP[endMP-i].id;
		setIter = inNode.find(tempId);
		if (setIter != inNode.end()) {
			MP[endMP-i].out[i-1] = 1;
			Graph->DelEdge (tempId, id);
		}
		
		setIter = outNode.find(tempId);
		if (setIter != outNode.end()) {
			MP[endMP-i].in[i-1] = 1;
			Graph->DelEdge (id, tempId);
		}
	}

	MP.push_back(n);
}

void updateX (const int k, vector<node> & MP, PNGraph & Graph, set<int> & X, map<int, int> & neighbor) {
	map<int, int>::iterator mapIter;
	TNGraph::TNodeI tempIter;
	int tempId;
	int sizeX = X.size();

	if (sizeX >= SIZE) {
		tempId = MP[MP.size()-SIZE].id;
		X.erase(tempId);
		
		tempIter = Graph->GetNI(tempId);
		printf("remove edges of %d from neighbor, in: %d, out: %d \n", tempId,tempIter.GetInDeg(), tempIter.GetOutDeg());

		for (int e = 0; e < tempIter.GetInDeg(); e++) {
			tempId = tempIter.GetInNId(e);
			mapIter = neighbor.find(tempId);
			if (mapIter != neighbor.end()) {
				if (mapIter->second > 1) {
					mapIter->second --;
				}
				else {
					neighbor.erase(tempId);
				}
			}
		}

		for (int e = 0; e < tempIter.GetOutDeg(); e++) {
			tempId = tempIter.GetOutNId(e);
			mapIter = neighbor.find(tempId);

			if (mapIter != neighbor.end()) {
				if (mapIter->second > 1) {
					mapIter->second --;
				}
				else {
					neighbor.erase(tempId);
				}
			}
		}
	}
}

void updatePos (const int & id, node & n, vector<node> & MP,  map<int, int> & pos) {

	n.id = id;

	int endMP = MP.size();
	map<int, int>::iterator mapIter = pos.find(id);	

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
}

void addNeigh (const TNGraph::TNodeI & idIter, set<int> & X, map<int, int> & neighbor, set<int> & inNode, set<int> & outNode) {
	set<int>::iterator setIter;
	map<int, int>::iterator mapIter;
	int tempId;

	// buffer edges between u and X, between X and V-X

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
			outNode.insert(tempId);
		}
	}
}

int getMostNeighNode (map<int, int> & neighbor) {
	vector< pair<int,int> > v;
	for (map<int,int>::iterator curr = neighbor.begin(); curr != neighbor.end() ;curr++) {
		v.push_back(make_pair(curr->first, curr->second) );
	}
	sort(v.begin(), v.end(), cmp);
	printf("largest %d \n", v[0].second);

	// delete the node from neighbor map
	int key = v[0].first;
	neighbor.erase(key);

	return key;
}

bool cmp (const pair<int, int> & x, const pair<int, int> & y) {
	return x.second > y.second;
}
