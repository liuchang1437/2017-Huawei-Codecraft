#include <cstdio>
#include <assert.h>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <deque>
#include <vector>
#include <queue>
#include "deploy.h"

#define N 2000
using namespace std;


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

		int num_edge;
		int num_network;
		int num_consumer;
		int cost_server; //服务器部署成本
		int flow_requirement; //总流量需求

		void add_edge(int src, int des, int cap, int flow, int cost);
		void load_graph(char * topo[MAX_EDGE_NUM], int line_num);
		bool spfa(int s, int t, int &flow, int &cost);

		void reset_loc();
		int min_cost(std::vector<int> & servers);
};
