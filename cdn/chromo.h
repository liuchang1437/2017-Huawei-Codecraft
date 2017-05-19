#ifndef _CHROMOSOME_H_
#define _CHROMOSOME_H_

#include <iostream>
#include <bitset>
#include <random>
#include <ctime>
#include "graph.h"

using namespace std;

const int MAX_SERVER=1000;

class Random{
	private:
		default_random_engine generator;
	public:
		Random(){
			generator.seed(time(nullptr));
		}			
		inline int random_int(int min, int max){
			return uniform_int_distribution<int>{min, max}(generator);
		}
		inline double random_real(double min, double max){
			return uniform_real_distribution<double>{min, max}(generator);
		}
};	

extern Random myrand;	

class Chromosome{
private:
	int fitness;
	double probability; // probability used in roulette
	unsigned length; // length of gene
	bitset<MAX_SERVER> gene;
	

public:		
	Chromosome(): length(0), fitness(0), probability(0){}
	
	void set_gene(unsigned len, vector<int> gene_obj); // set gene with gene_obj
	void init_random(unsigned len); // initialize gene randomly.

	inline void set_fitness(int f){
		this->fitness = f;
	}
	inline int get_fitness(){
		return this->fitness;
	}
	inline void set_probability(double p){
		this->probability = p;
	}
	inline double get_probability(){
		return this->probability;
	}
	inline int gene_at(unsigned pos){
		return this->gene[pos];
	}
	vector<int> get_server();
	vector<int> get_neighbor(mcmf_network mcmf);
	inline void operator*(Chromosome &other){ // cross over
        int end = myrand.random_int(1,this->length);
        int begin = myrand.random_int(0,end-1);
#ifdef _DEBUG
        cout << "[" << begin << "," << end <<"]" << endl;
#endif
        for(auto i=begin; i<end; ++i){
            if(this->gene[i]!=other.gene[i]){
                this->gene.flip(i);
                other.gene.flip(i);
            }
        }		
    }
	inline void mutate(){
        unsigned pos = myrand.random_int(0,this->length-1);
#ifdef _DEBUG
    std::cout << "Mutate: " << pos <<endl;
#endif
        this->gene.flip(pos);
    }
	inline void operator=(const Chromosome &other){ // copy operation
        this->length = other.length;
        this->fitness = other.fitness;
        this->gene = other.gene;
    }
				
#ifdef _DEBUG
	void print_gene(){
		for (auto i=0; i < this->length; ++i){
			cout << this->gene[i];
		}
		cout << endl;
	}
#endif	
};

#endif
