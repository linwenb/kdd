#include "kdd.h"

extern int totalBits;

void run (const char inputfile[], const char outputfile[], const int & K, const float & RF, const float & DT) {
	printf("Start read graph\n");
	PNGraph G = LoadEdgeList<PNGraph>(inputfile);

	totalBits = 0;
	int edges = G->GetEdges();

	string s = string(outputfile) + ".graph";
	TFOut FOut(s.c_str()); G->Save(FOut);

	printf("Start generate MP\n");
	vector<node> v;

	clock_t begin_time = clock();
	genMP(v, G, K, RF, DT);
	clock_t end_time = clock();

	float tDiff = float( end_time - begin_time ) /  CLOCKS_PER_SEC;
	printf("\nProcessing time %f sec (%f min)\n", tDiff, tDiff/60);
	
	printf("MP size %d\n\n", v.size());

	statistic(outputfile, v.size(), totalBits, edges, K, RF, DT);
/*
	int key = G->GetRndNId();
	queryNeigh (v, key);

	saveMP(v, outputfile);

	vector<node> v2;
	loadMP(v2, outputfile);
	printf("Read MP size %d\n", v2.size());

	queryNeigh (v2, key);
*/
}

void genMP (vector<node> & MP, PNGraph & Graph, const int & K, const float & RF, const float & DT) {

	// current K
	int k = K;
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

void statistic (const char outputfile[], const int & vSize, const int bits, const int & edges, const int & K, const float & RF, const float & DT) {
	ofstream output;
	output.open("statistic.txt", ios::app);
	output << outputfile << '\t' << vSize << '\t'
		<< bits << '\t' << edges << '\t' << 1.0*bits/edges
		<< '\t' << K << '\t' << RF << '\t' << DT << endl;
	output.close();
}

void saveMP (const vector<node> & v, const char outputfile[]) {
	ofstream FILE;
	FILE.open(outputfile, ios::out|ios::trunc|ios::binary);
	for (int i = 0; i< v.size(); i++) {
		FILE.write(reinterpret_cast<const char*>(&(v[i].id)), sizeof(int));
		FILE.write(reinterpret_cast<const char*>(&(v[i].next)), sizeof(int));
		char a,b;
		for (int j = 7; j >= 0; j--) {
			a = a<<1 | v[i].in[j];
			b = b<<1 | v[i].out[j];
		}
		FILE.write(reinterpret_cast<const char*>(&a), sizeof(char));
		FILE.write(reinterpret_cast<const char*>(&b), sizeof(char));
	}
	FILE.close();
}

void loadMP (vector<node> & v, const char inputfile[]) {
	node n;
	int i;
	char c;
	ifstream FILE;
	FILE.open(inputfile, ios::in|ios::binary);
	while (true) {
		FILE.read(reinterpret_cast<char*>(&i), sizeof(int));
		n.id = i;
		FILE.read(reinterpret_cast<char*>(&i), sizeof(int));
		n.next = i;
		FILE.read(reinterpret_cast<char*>(&c), sizeof(char));
		n.in = bitset<SIZE>(c);
		FILE.read(reinterpret_cast<char*>(&c), sizeof(char));
		n.out = bitset<SIZE>(c);

		if (FILE.eof()) break;
		v.push_back(n);
	}	
	FILE.close();
}

void queryNeigh (vector<node> & v, const int & key) {
	vector<int> inNode;
	vector<int> outNode;

	int curr = key;
	do {
		for (unsigned int i = 0; i < v[curr].in.size(); i++) {
			
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
	for (unsigned int i = 0 ; i < inNode.size(); i++) {
		printf("%d ", inNode[i]);
	}
	printf("\nOut : ");
	for (unsigned int i = 0 ; i < outNode.size(); i++) {
		printf("%d ", outNode[i]);
	}
	printf("\n\n");
}

void appendNode (vector<node> & MP, const int & id, PNGraph & Graph, set<int> & X, const int & k, map<int, int> & pos, map<int, int> & neighbor) {
	set<int> inNode;
	set<int> outNode;

	TNGraph::TNodeI idIter = Graph->GetNI(id);

	printf("Append node %d to MP, total %d edges left\n", id, Graph->GetEdges() );

	node n;

	// update pos
	updatePos(id, n, MP, pos);

	// update neighbor
	addNeigh(idIter, X, neighbor, inNode, outNode);

	// update X
	int sizeX = X.size();
	updateX(k-1, MP, Graph, X, neighbor);
	X.insert(id);

	// update MP
	updateMP(MP, sizeX, n, Graph, inNode, outNode);

	totalBits += k;
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
			MP[endMP-i].out[i-1] = true;
			Graph->DelEdge (tempId, id);
		}
		
		setIter = outNode.find(tempId);
		if (setIter != outNode.end()) {
			MP[endMP-i].in[i-1] = true;
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
