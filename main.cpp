#include "kdd.h"

int main () {
	char readfile[] = "wiki-Vote.txt.gz";
	char savefile[] = "wiki-Vote.dat";

	run(readfile, savefile);

	return 0;
}
