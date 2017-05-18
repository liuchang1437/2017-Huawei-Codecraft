#include "graph.h"

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
		servers_init.push_back(des);
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

	while(spfa(super_s, super_t, flow, cost)); //spfa
	//printf("flow = %d\n", flow);
	//printf("flow_requirement = %d\n", flow_requirement);
	if(flow < flow_requirement){
		return -1;
	}
	cost += Graph[super_s].size() * cost_server; //Calculate total cost
	return cost;
}

int mcmf_network::find_path(std::vector<int> & path, int node, int min_flow, int total_flow){ //用dfs找到一条路径
	if(visit[node] == 1){
		return 0;
	}
	else if(node >= num_network && node < super_s){ //dfs搜索到消费节点
		final_path.push_back(path); //将路径加入到结果中
		final_path.back().push_back(node - num_network); //在末尾添加消费节点ID和flow
		final_path.back().push_back(min_flow);
		return min_flow;
	}

	size_t i;
	visit[node] = 1; //搜索网络点
	if(node != super_s){
		path.push_back(node); //将每次搜索的网络点加入到path
	}
	int tmp = total_flow;
	for(i = 0; i < Graph[node].size(); i++){ //DFS
		Edge &e = edges[Graph[node][i]];
		if(e.flow > 0){ //只检查flow大于0的边
			int v = e.des; //往后搜索
			if(visit[v] != 1){
				if(total_flow > 0){ //该点的flow还有剩余
					int return_flow = find_path(path, v, min(min_flow, min(total_flow, e.flow)), min(total_flow, e.flow));
					e.flow = e.flow - return_flow; //更新flow
					total_flow = total_flow - return_flow;
				}
				else{
					break;
				}
			}
		}
	}

	visit[node] = 0; //搜索完后取消标记，因为可能存在重复的路径
	path.pop_back();
	return tmp; //该点的后续节点已搜索完毕，返回total_flow跟之前的相减使得flow为0
}

void mcmf_network::print_path(){
	memset(visit,0,sizeof(visit));
	std::vector<int> path;
	find_path(path, super_s, INF, INF);

	unsigned int i,j;
	for(i = 0; i < final_path.size(); i++){
		for(j = 0; j < final_path[i].size(); j++){
			printf("%d ", final_path[i][j]);
		}
		printf("\n");
	}
}

void mcmf_network::best_init(){ //当每个服务器与消费节点直连，获得最佳初始值
	int cost = num_consumer * cost_server;
	for(int i = 0; i < num_consumer; i++){
		int tmp = servers_init[0];
		std::vector<int>::iterator loc = servers_init.begin() + 0;
		servers_init.erase(loc);
		if(min_cost(servers_init) != -1 && min_cost(servers_init) < cost){
			continue;
		}
		else{
			servers_init.push_back(tmp);
		}
	}
	printf("servers_init is \n");
	for(unsigned int i = 0; i < servers_init.size(); i++){
		printf("%d ", servers_init[i]);
	}
	printf("\n");
	printf("best init cost is %d\n", cost);
}

int mcmf_network::get_neighbor(int node){
	//printf("the number of neighbor is %d\n", Graph[node].size());
	//for(unsigned int i = 0; i < Graph[node].size(); i++){
	//	printf("%d th neighbor is %d -> %d\n", i, edges[Graph[node][i]].src, edges[Graph[node][i]].des);
	//}
	int v = random_int(Graph[node].size());
	if(edges[Graph[node][v]].des == node){
		return edges[Graph[node][v]].src;
	}
	else{
		return edges[Graph[node][v]].des;
	}
}
