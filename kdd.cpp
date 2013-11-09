#include "kdd.h"

void run (const char inputfile[], const char outputfile[]) {
	printf("Start read graph\n");
	PNGraph G = LoadEdgeList<PNGraph>(inputfile);

	printf("Start generate MP\n");	
	vector<node> v;
	genMP(v, G);

	queryNeigh (v, G->GetRndNId() );

	printf("Start save MP with vector size %d\n", v.size());
	saveMP(v, outputfile);

	vector<node> v2;
	loadMP (v2, outputfile);
	printf("Read MP with vector size %d\n", v2.size());
}

void genMP (vector<node> & MP, PNGraph & Graph, float RF, float DT) {

	// current K
	int k = SIZE;
	int edgeCount = 0;

	TNGraph::TEdgeI iter;

	map<int, int> pos;
	map<int, int> neighbor;
	set<int> X;

	while (Graph->GetEdges()) {
		// random node d with nonzero degree
		iter = Graph->BegEI();
		
		// append d to L
		// X <- last k vertices in L
		appendNode(MP, iter.GetSrcNId(), Graph, X, k, pos, neighbor);

		while (!neighbor.empty()) {
			// node u most edges to/from X
			// remove edges between v and X
			int u = getMostNeighNode(neighbor);
						
			edgeCount += Graph->GetNI(u).GetDeg();
			
			// append u to L
			appendNode(MP, u, Graph, X, k, pos, neighbor);

			edgeCount -= Graph->GetNI(u).GetDeg();

			// decrease k and update X if necessary
			if (MP.size() % 1000 == 0) {
				if (edgeCount < 2000 * DT * k) {
					k *= RF;
					updateX(k, MP, Graph, X, neighbor);
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

void saveMP2 (const vector<node> & v, const char outputfile[]) {
	ofstream output;
	output.open(outputfile);
	for (int i = 0; i < v.size(); i++) {
		output << v[i].id << ' ' << v[i].in << ' '<< v[i].out << ' '<< v[i].next << endl;
	}
	output.close();
}

void loadMP (vector<node> & v, const char inputfile[]) {
	ifstream FILE;
	FILE.open(inputfile, ios::in|ios::binary);
	FILE.seekg (0, ios::end);
	int length = FILE.tellg();
	FILE.read(reinterpret_cast<char*>(&v), length);
	FILE.close();
}

void queryNeigh (vector<node> & v, const int & key) {
	vector<int> inNode;
	vector<int> outNode;

	int curr = key;
	do {
		for (int i = 0; i < SIZE; i++) {
			
			if (v[curr].in[i]) {
				inNode.push_back(v[curr + i + 1].id);
			}
			if (v[curr].out[i]) {
				outNode.push_back(v[curr + i + 1].id);
			}
		}
		curr = v[curr].next;
	} while (curr != key);

	printf("Query node : %d\nIn : ", v[key].id);
	for (int i = 0 ; i < inNode.size(); i++) {
		printf("%d ", inNode[i]);
	}
	printf("\nOut : ");
	for (int i = 0 ; i < outNode.size(); i++) {
		printf("%d ", outNode[i]);
	}
	printf("\n");
}

void appendNode (vector<node> & MP, const int & id, PNGraph & Graph, set<int> & X, const int & k, map<int, int> & pos, map<int, int> & neighbor) {
	set<int> inNode;
	set<int> outNode;

	TNGraph::TNodeI idIter = Graph->GetNI(id);

	printf("Append node %d to MP, total %d edges left\n", id, Graph->GetEdges() );

	node n;

	// update pos
	updatePos (id, n, MP, pos);

	// update neighbor
	addNeigh(idIter, X, neighbor, inNode, outNode);

	// update X
	int sizeX = X.size();	

	updateX(k-1, MP, Graph, X, neighbor);
	X.insert(id);

	// update MP
	updateMP(MP, sizeX, n, Graph, inNode, outNode);
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

	while (X.size() > k) {
		tempId = MP[MP.size()-X.size()].id;
		X.erase(tempId);
		
		tempIter = Graph->GetNI(tempId);

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

	// delete the node from neighbor map
	int key = v[0].first;
	neighbor.erase(key);

	return key;
}

bool cmp (const pair<int, int> & x, const pair<int, int> & y) {
	return x.second > y.second;
}
