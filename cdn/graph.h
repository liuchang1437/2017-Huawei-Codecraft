#ifndef _GRAPH_
#define _GRAPH_
#include <cstdio>
#include <assert.h>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <deque>
#include <vector>
#include <queue>
#include <time.h>
#include "deploy.h"

#define INF 0x3f3f3f3f
#define N 2000
using namespace std;

inline int random_int2(int range){
	srand((int)time(NULL));
	return rand()%range;
}

class mcmf_network{
	private:
		struct Edge{
			int src, des, cap, flow, cost;
			Edge(int src, int des, int cap, int flow, int cost): src(src), des(des), cap(cap), flow(flow), cost(cost) {}
		};

//		static int N = 2000;

		int super_s, super_t;
		int d[N]; //距离
		int visit[N]; //是否访问过
		int p[N]; //前驱节点
		int a[N]; //剩余的flow


	public:
		std::vector<int> Graph[N]; //图
		std::vector<Edge> edges; //边集
		std::vector<std::vector<int>> final_path; //最佳路径
		std::vector<int> servers_init; //初始服务器位置，直连到所有消费节点
		std::vector<char> output_path;

		int num_edge;
		int num_network;
		int num_consumer;
		int cost_server; //服务器部署成本
		int flow_requirement; //总流量需求

		void add_edge(int src, int des, int cap, int flow, int cost);
		void load_graph(char * topo[MAX_EDGE_NUM], int line_num);
		bool spfa(int s, int t, int &flow, int &cost);

		inline void reset_loc(){
			size_t i;
			for(i = 0; i < Graph[super_s].size(); i++){
				int edge_num = Graph[super_s][i];
				Graph[edges[edge_num].des].pop_back(); //在图中，删除与超级源点相连的服务器的边
			}
			for(i = Graph[super_s].size() * 2; i > 0; i--){
				edges.pop_back(); //在边集中，删除超级源点与服务器之间的边
			}
			Graph[super_s].clear(); //在图中，删除超级源点连出去的边
			for(i = 0; i < edges.size(); i++){
				edges[i].flow = 0; //重置flow
			}
		}
		
		inline int min_cost(std::vector<int> & servers){
			int cost = 0;
			int flow = 0;

			size_t i;
			reset_loc(); //重置服务器位置
			for(i = 0; i < servers.size(); i++){
				add_edge(super_s, servers[i], INF, 0, 0); //超级源点->服务器
			}

			while(spfa(super_s, super_t, flow, cost)); //spfa
			//printf("flow = %d\n", flow);
			//printf("flow_requirement = %d\n", flow_requirement);
			if(flow < flow_requirement){
				return -1;
			}
			cost += Graph[super_s].size() * cost_server; //Calculate total cost
			return cost;
		}

		int find_path(std::vector<int> & path, int node, int min_flow, int total_flow);
		void print_path();
		char* write_path();
		void best_init();
		int get_neighbor(int node);
};
#endif