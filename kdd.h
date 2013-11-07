#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <iterator>

#include "Snap.h"

using namespace std;
using namespace TSnap;

#define SIZE  8

struct node {
	unsigned int id;
	bitset<SIZE> left;
	bitset<SIZE> right;
	unsigned int next;
};

void run (const char inputfile[], const char outputfile[]);

vector<node> * genMP (PNGraph Graph, float RF = 0.9, float DT = 0.25);

void saveMP (vector<node> * v, const char outputfile[]);
