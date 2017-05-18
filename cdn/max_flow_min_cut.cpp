#include "max_flow_min_cut.h"

#define INF 0x3f3f3f3f

void mcmf_network::add_edge(int src, int des, int cap, int flow, int cost){
	edges.push_back(Edge(src, des, cap, 0, cost));
	edges.push_back(Edge(des, src, 0, 0, -cost));

	int m = edges.size();
	Graph[src].push_back(m - 2);
	Graph[des].push_back(m - 1);
}

bool mcmf_network::spfa(int s, int t, int &flow, int &cost){
	for(int i = 0; i < N; i++)
			d[i]=INF;
	memset(visit,0,sizeof(visit));
	d[s]=0;
	visit[s]=1;
	p[s]=0;
	a[s]=INF;
	queue<int> Q;
	Q.push(s);
	while(!Q.empty())
	{
			int u=Q.front();
			Q.pop();
			visit[u]--;
			for(size_t i = 0; i < Graph[u].size(); i++)
			{
					Edge& e=edges[Graph[u][i]];
					if(e.cap>e.flow && d[e.des]>d[u]+e.cost)//增广
					{
							d[e.des]=d[u]+e.cost;
							p[e.des]=Graph[u][i];
							a[e.des]=min(a[u],e.cap-e.flow);
							if(!visit[e.des])
							{
									visit[e.des]++;
									Q.push(e.des);
							}
					}
			}
	}
	if(d[t]==INF) return false;//t不可达
	flow+=a[t];
	cost+=d[t]*a[t];
	int u=t;
	while(u!=s)//残余网络
	{
			edges[p[u]].flow+=a[t];
			edges[p[u]^1].flow-=a[t];
			u=edges[p[u]].src;
	}
	return true;
}

void mcmf_network::load_graph(char * topo[MAX_EDGE_NUM], int line_num){
	flow_requirement = 0;
	//Line 0: 网络节点数量 网络链路数量 消费节点数量
	sscanf(topo[0], "%d %d %d", &num_network, &num_edge, &num_consumer);

	//超级源点和超级汇点的编号
	super_s = num_consumer + num_network;
	super_t = num_consumer + num_network + 1;

	//Line 2: 视频内容服务器部署成本
	sscanf(topo[2], "%d", &cost_server);

	//Line 4 ~ n: 链路起始节点ID 链路终止节点ID 总带宽大小 单位网络租用费
	int src, des, cap, cost;
	for(int i = 0; i < num_edge; i++){
		sscanf(topo[4 + i], "%d %d %d %d", &src, &des, &cap, &cost);
		//printf("src = %d, des = %d, cap = %d, cost = %d\n", src, des, cap, cost);
		add_edge(src, des, cap, 0, cost);
		add_edge(des, src, cap, 0, cost);
	}

	//Line n + 1: 消费节点ID 相连网络节点ID 视频带宽消耗需求
	for(int i = 0; i < num_consumer; i++){
		sscanf(topo[5 + num_edge + i], "%d %d %d", &src, &des, &cap);
		add_edge(des, src + num_network, cap, 0, 0); //网络节点 -> 消费节点
		add_edge(src + num_network, super_t, cap, 0, 0); //消费节点 -> 超级汇点
		flow_requirement += cap; //消耗总需求
	}
}


void mcmf_network::reset_loc(){
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

int mcmf_network::min_cost(std::vector<int> & servers){
	int cost = 0;
	int flow = 0;

	size_t i;
	reset_loc(); //重置服务器位置
	for(i = 0; i < servers.size(); i++){
		add_edge(super_s, servers[i], INF, 0, 0); //超级源点->服务器
	}

	while(spfa(super_s, super_t, flow, cost));
	printf("flow = %d\n", flow);
	printf("flow_requirement = %d\n", flow_requirement);
	if(flow < flow_requirement){
		return -1;
	}
	cost += Graph[super_s].size() * cost_server; //Calculate total cost
	return cost;
}
