#include "gasa.h"
#include "chromo.h"
#include <signal.h>
#include <unistd.h>
#include <iostream>

/* utils  */
bool time_out = true;
sigFunc *Signal(int signo, sigFunc *func) {
	struct sigaction	act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
}
void timeOutHandler(int signo) {
	time_out = false;
	return;
}
int roulette(vector<Chromosome> pop){
	double prob = myrand.random_real(0,1);
	double accum = 0;
	for(decltype(pop.size()) i=0; i!=pop.size(); ++i){
		accum += pop[i].get_probability();
		if(accum >= prob){
			return i;
		}
	}
	return 0;
}
/* end utils */

/*
 * 	Initilize population. The first chromosome is initialized with pruned servers 
 * that directly connected to consumers. The others are initialized randomly.
 */
vector<Chromosome> GASA::init_population(int num_server, vector<int> direct_conn){
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

char* GASA::process(char * topo[MAX_EDGE_NUM], int line_num, double decay_factor, double cross_prob, double mutate_prob){
	Signal(SIGALRM, timeOutHandler);
	alarm(86);
	mcmf_network mcmf;
	mcmf.load_graph(topo, line_num);
	mcmf.best_init();
	vector<int> direct_conn = mcmf.servers_init;// get runed servers that directly connected to consumers
	int num_server = mcmf.num_network; // get number of servers
	cout << "number of servers: " << num_server << endl;
	vector<Chromosome> population = this->init_population(num_server,direct_conn);
	Chromosome best = population[0]; // minimum cost
	best.set_cost(mcmf.min_cost(direct_conn));
	vector<Chromosome> next_generation(this->max_population);
	double init_temp = this->temperature;
	
	int iteration = 0;
	while(time_out && this->temperature > 0.1){
		/* simulated annealing */
		for(decltype(population.size()) i=0; time_out && i!=population.size(); ++i){
			vector<int> server = population[i].get_server();
			int server_cost = mcmf.min_cost(server); // get minimum cost 
			if(server_cost==-1){
				continue;
			}
			vector<int> neigbour = population[i].get_neighbor(mcmf);
			int neigbour_cost = mcmf.min_cost(neigbour);
			if(neigbour_cost!=-1){
				double delta_cost = neigbour_cost - server_cost;
				if(min<double>(1,exp(-delta_cost/this->temperature)) > myrand.random_real(0,1)){ // accept probability
					population[i].set_gene(num_server,neigbour);
					population[i].set_cost(neigbour_cost);
				} else{
					population[i].set_cost(server_cost);
				}
			} else{
				population[i].set_cost(server_cost);
			}
			if(population[i].get_cost() < best.get_cost()){
				cout << "Best changed: "<< best.get_cost() << " -> " << population[i].get_cost() << endl;
				best = population[i];
				//vector<int> best_server_tmp = best.get_server();
				//for(decltype(best_server_tmp.size()) j=0; j!=best_server_tmp.size(); ++j){
				//	cout<<best_server_tmp[j]<<", ";
				//}
				//cout << endl;
			}
		} /* simulated annealing end */
		// vector<int> tmp_best_servers = best.get_server();
		// cout << mcmf.min_cost(tmp_best_servers) << endl;
		/* gene algorithm */
		/* select next generation by roulette. */
		int cost_min = best.get_cost();
		double fitness_total = 0;
		for(auto chrome: population){
			if(!time_out){
				break;
			}
			double delta_cost = chrome.get_cost() - cost_min;
			chrome.set_fitness(exp(-delta_cost/this->temperature));
			fitness_total += chrome.get_fitness();
		}
		for(auto chrome: population){
			if(!time_out){
				break;
			}
			chrome.set_probability(chrome.get_fitness()/fitness_total);
		}
		next_generation[0] = best; // keap the best in the next generation.
		for(decltype(population.size()) i=1; time_out && i!=population.size(); ++i){
			next_generation[i] = population[roulette(population)];
		}
		for(decltype(population.size()) i=0; time_out && i!=population.size(); ++i){
			population[i] = next_generation[i];
		}

		/* cross over */
		for(decltype(population.size()) i=0; time_out && (i+1)<population.size(); i+=2){
			if(myrand.random_real(0,1) < cross_prob){
				population[i] * population[i+1];
			}
		}

		/* mutate */
		for(decltype(population.size()) i=0; time_out && i<population.size(); ++i){
			if(myrand.random_real(0,1) < mutate_prob){
				population[i].mutate();
			}
		}

		this->temperature *= decay_factor;
		iteration += 1;
		//cout << "Loop end" << endl;
	}
	std::cout << "******************** Parameters ********************"<< std::endl;
	std::cout << "Initial temperature: "<< init_temp << std::endl;
	std::cout << "Decay Factor: "<< decay_factor << std::endl;
	std::cout << "Cross over probability: "<< cross_prob << std::endl;
	std::cout << "Mutation probability: "<< mutate_prob << std::endl;
	std::cout << std::endl;
	std::cout << "******************** Results ********************"<< std::endl;
	std::cout << "Final temperature: "<< this->temperature << std::endl;
	std::cout << "Iteration: "<< iteration << std::endl;
	std::vector<int> best_server = best.get_server();
//	for(decltype(best_server.size()) j=0; j!=best_server.size(); ++j){
//		cout<<best_server[j]<<", ";
//	}
//	cout << endl;
	std::cout << "Best cost: " << mcmf.min_cost(best_server) << std::endl;
	//mcmf.min_cost(best_server);
	//vector<int> pop_server = population[0].get_server();
	//std::cout << "population[0] cost: " << mcmf.min_cost(pop_server) << std::endl;
	return mcmf.write_path();
}
