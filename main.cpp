#include "kdd.h"

int totalBits;

int main (int argc, char* argv[]) {

/*	run(argv[1], argv[2]);
*/
	char* inputFiles[] = {"p2p-Gnutella08.txt.gz","p2p-Gnutella24.txt.gz","wiki-Vote.txt.gz","email-Enron.txt.gz","soc-Epinions1.txt.gz", "soc-sign-Slashdot090216.txt.gz", "amazon0302.txt.gz","web-Stanford.txt.gz"};

	char* outputFiles[] = {"p2p-Gnutella08.dat","p2p-Gnutella24.dat","wiki-Vote.dat","email-Enron.dat","soc-Epinions1.dat", "soc-sign-Slashdot090216.dat","amazon0302.dat","web-Stanford.dat"};
	
	int K[] = {4,8,16,24}; //K
	float DT[] = {0.15,0.25,0.3}; // density threshold

	//K=8,RF =1, DT = 0
	for(int i = 0; i < 8; i++)
		run(inputFiles[i], outputFiles[i], 8, 1, 0);

	
	for(int i = 0 ; i < 8; i++)
		for(int j = 0; j < 4; j++)
			for(int k = 0; k < 3; k++)
				run(inputFiles[i], outputFiles[i], K[j], 0.9, DT[k]);

	return 0;
}
