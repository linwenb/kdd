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
	bitset<SIZE> left;
	bitset<SIZE> right;
	int next;
};

void run (const char inputfile[], const char outputfile[]);

void genMP (vector<node> & MP, PNGraph & Graph, float RF = 0.9, float DT = 0.25);

void saveMP (const vector<node> & v, const char outputfile[]);

void loadMP (vector<node> & v, const char inputfile[]);



// helping functions

void appendNode (vector<node> & MP, const TNGraph::TNodeI & iter, PNGraph & Graph, 
	set<int> & X, map<int, int> & pos, map<int, int> & neighbor);

void addNeigh (const TNGraph::TNodeI & idIter, set<int> & X, map<int, int> & neighbor, set<int> & inNode, set<int> & outNode);

int getMostNeighNode (map<int, int> & neighbor);

bool cmp (const pair<int, int> & x, const pair<int, int> & y);
