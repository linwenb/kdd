#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <map>
#include <set>

#include "Snap.h"

using namespace std;
using namespace TSnap;

#define SIZE 8

struct node {
	int id;
	bitset<SIZE> out;
	bitset<SIZE> in;
	int next;
};

void run (const char inputfile[], const char outputfile[]);

void genMP (vector<node> & MP, PNGraph & Graph, float RF = 0.9, float DT = 0.25);

void saveMP (const vector<node> & v, const char outputfile[]);

void loadMP (vector<node> & v, const char inputfile[]);



// helping functions
void updatePos (const int & id, node & n, vector<node> & MP,  map<int, int> & pos);

void updateX (const int k, vector<node> & MP, PNGraph & Graph, set<int> & X, map<int, int> & neighbor);

void updateMP (vector<node> & MP, const int & sizeX, const node & n, PNGraph & Graph, set<int> & inNode, set<int> & outNode);

void appendNode (vector<node> & MP, const int & id, PNGraph & Graph, 
	set<int> & X, map<int, int> & pos, map<int, int> & neighbor);

void addNeigh (const TNGraph::TNodeI & idIter, set<int> & X, map<int, int> & neighbor, set<int> & inNode, set<int> & outNode);

int getMostNeighNode (map<int, int> & neighbor);

bool cmp (const pair<int, int> & x, const pair<int, int> & y);
