#ifndef _GA_SA_H_
#define _GA_SA_H_
#include <bitset>
const int MAX_SERVER=1000;

class Chromosome{
private:
    unsigned length;
    double fitness;
    bitset<MAX_SERVER> gene;

pulic:
    Chromosome(): length(0), fitness(0){ }
    Chromosome(int length): fitness(0){
        
    }

};
#endif