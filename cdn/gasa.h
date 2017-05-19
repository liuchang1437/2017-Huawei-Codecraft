#ifndef _GA_SA_H_
#define _GA_SA_H_

#include "chromo.h"
#include "graph.h"
#include "deploy.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int roulette(vector<Chromosome> pop);

typedef void (sigFunc)(int);
extern bool time_out;

sigFunc *Signal(int signo, sigFunc *func);
void timeOutHandler(int signo);

class GASA{
	private:
		double temperature;
		unsigned max_population;
		int iteration;

	public:
		GASA(double init_t=20, unsigned max_pop=26): temperature(init_t), max_population(max_pop),iteration(0){}
			
		vector<Chromosome> init_population(unsigned num_server, vector<int> direct_conn);
			
		void process(char * topo[MAX_EDGE_NUM], int line_num, double decay_factor=0.999, double cross_prob=0.5, double mutate_prob=0.15){
			Signal(SIGALRM, timeOutHandler);
			alarm(86);
			mcmf_network mcmf;
			mcmf.load_graph(topo, line_num);
			mcmf.best_init();
			vector<int> direct_conn = mcmf.servers_init;// return direct connectivity.
			vector<int> best_init_copy = direct_conn;
			unsigned num_server = mcmf.num_network; //get number of servers
			vector<Chromosome> population = this->init_population(num_server,direct_conn);
			Chromosome best = population[0]; // minimum cost	
			vector<Chromosome> next_generation(this->max_population);
			int best_cost = mcmf.min_cost(best_init_copy);
			while(time_out && this->temperature > 0.1){
				/* simulated annealing */
				for(auto i=0; time_out && i!=population.size(); ++i){
					vector<int> server = population[i].get_server();
					int server_cost = mcmf.min_cost(server); // get minimum cost 
					if(server_cost==-1){
						continue;
					}
					vector<int> neigbour = population[i].get_neighbor(mcmf);
					int neigbour_cost = mcmf.min_cost(neigbour);
					if(neigbour_cost!=-1){
						int delta_cost = neigbour_cost - server_cost;
						if(min<double>(1,exp(-delta_cost/this->temperature)) > myrand.random_real(0,1)){ // accept probability
							population[i].set_gene(num_server,neigbour);
							population[i].set_fitness(neigbour_cost);
						} else{
							population[i].set_fitness(server_cost);
						}
					} else{
						population[i].set_fitness(server_cost);
					}
					if(population[i].get_fitness() < best_cost){
						best = population[i];
						best_cost = population[i].get_fitness();
					}
					//cout << "server cost: " << server_cost << " neigbour cost" << neigbour_cost << endl;
				} /* simulated annealing end */
				/* gene algorithm */
				int fitness_min = best.get_fitness();
				double fitness_total = 0;
				for(auto chrome: population){
					int delta_cost = chrome.get_fitness() - fitness_min;
					chrome.set_fitness(exp(-delta_cost/this->temperature));
					fitness_total += chrome.get_fitness();
				}
				for(auto chrome: population){
					chrome.set_probability(chrome.get_fitness()/fitness_total);
				}
				next_generation[0] = best;
				for(auto i=1; time_out && i!=this->max_population; ++i){
					next_generation[i] = population[roulette(population)];
				
				}
				for(auto i=0; time_out && i!=this->max_population; ++i){
					population[i] = next_generation[i];
				
				}
				/* cross over */
				for(auto i=0; time_out && (i+1)<(this->max_population); i+=2){
					if(myrand.random_real(0,1) < cross_prob){
						population[i] * population[i+1];
					}
				}

				/* mutate */
				for(auto i=0; time_out && i!=this->max_population; ++i){
					if(myrand.random_real(0,1) < mutate_prob){
						population[i].mutate();
					}
				}
				for(auto i=0; time_out && i!=this->max_population; ++i){
					vector<int> tmp_server = population[i].get_server();
					cout << mcmf.min_cost(tmp_server) <<" "<< endl;
				}
				cout << endl;
				
				for(auto i=0; time_out && i!=this->max_population; ++i){
					vector<int> tmp_servers = population[i].get_server();
					int tmp_cost = mcmf.min_cost(tmp_servers);
					if(tmp_cost!=-1 && (tmp_cost < best_cost)){
						best = population[i];
						best_cost = tmp_cost;
					}
				}
				vector<int> best_tmp_server = best.get_server();
				cout << "Best cost " << mcmf.min_cost(best_tmp_server) << endl;
				this->temperature *= decay_factor;
				this->iteration += 1;
			}
			std::cout << "temperature: "<< this->temperature << std::endl;
			std::cout << "iteration: "<< this->iteration << std::endl;
			std::vector<int> best_server = best.get_server();
			std::cout << "Init cost " << mcmf.min_cost(best_init_copy) << std::endl;
			std::cout << "Best cost " << best_cost << std::endl;
		}
};
#endif
