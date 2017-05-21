#include "chromo.h"
#include "graph.h"

Random myrand;

void Chromosome::set_gene(int len, vector<int> gene_obj){
        this->length = len;
	this->gene.reset();
        for(auto obj: gene_obj){
            this->gene.set(obj);
        }
    }
    
void Chromosome::init_random(int len){ // initialize gene randomly.
    this->length = len;
    for (auto i=0; i!=len; ++i){
        gene[i] = myrand.random_int(0,1);
    }	
}

vector<int> Chromosome::get_server(){ // get chosen servers
    vector<int> server;
    for(auto i=0; i!=this->length; ++i){
        if(this->gene[i]){
            server.push_back(i);
        }
    }
#ifdef _DEBUG
    for(auto s: server){
        std::cout << s;
    }
    std::cout << std::endl;
#endif
    return server;
}

vector<int> Chromosome::get_neighbor(mcmf_network mcmf){ // get neighbor
    vector<int> server = this->get_server(); 
    int chosen_index = myrand.random_int(0,server.size()-1);
    int chosen = server[chosen_index]; // size - 1 !!!!!!!!!!!!!!!!!!
    int neighbor;
    do{
        neighbor = mcmf.get_neighbor(chosen);
    } while(neighbor >= mcmf.num_network);
    this->gene.reset(chosen);
    this->gene.set(neighbor);
    return this->get_server();
}  
