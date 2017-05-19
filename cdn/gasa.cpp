#include "gasa.h"
#include "chromo.h"
extern bool time_out = true;
sigFunc *Signal(int signo, sigFunc *func) {
	struct sigaction	act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
}
/* end signal */

void timeOutHandler(int signo) {
	time_out = false;
	return;
}

int roulette(vector<Chromosome> pop){
	double prob = myrand.random_real(0,1);
	double accum = 0;
	for(auto i=0; i!=pop.size(); ++i){
		accum += pop[i].get_probability();
		if(accum >= prob){
			return i;
		}
	}
	return 0;
}

vector<Chromosome> GASA::init_population(unsigned num_server, vector<int> direct_conn){
	vector<Chromosome> population(this->max_population);
	for(auto i=0; i!=this->max_population; ++i){
		if(!i){
			population[i].set_gene(num_server, direct_conn);
		}
		else{
			population[i].init_random(num_server);
		}
	}
	return population;
}
