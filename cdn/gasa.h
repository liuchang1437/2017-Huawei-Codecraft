#ifndef _GA_SA_H_
#define _GA_SA_H_

#include "chromo.h"
#include "graph.h"
#include "deploy.h"

/* utils */
int roulette(vector<Chromosome> pop);

// timing
typedef void (sigFunc)(int);
extern bool time_out;
sigFunc *Signal(int signo, sigFunc *func);
void timeOutHandler(int signo);
/* end utils */

class GASA{
private:
	double temperature;
	int max_population;
public:
	GASA(double init_temp=20, int max_pop=26): temperature(init_temp), max_population(max_pop){}
	vector<Chromosome> init_population(int, vector<int>);
	char* process(char *[], int, double, double, double);
};
#endif
