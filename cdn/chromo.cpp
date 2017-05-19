#include "chromo.h"
#include "graph.h"

Random myrand;

void Chromosome::set_gene(int len, vector<int> gene_obj){
        this->length = len;
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
    int chosen = server[myrand.random_int(0,server.size()-1)]; // size - 1 !!!!!!!!!!!!!!!!!!!
    this->gene.reset(chosen);
    this->gene.set(mcmf.get_neighbor(chosen)); 
    return this->get_server();
}  
