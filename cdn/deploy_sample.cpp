#include "deploy.h"
#include "max_flow_min_cut.h"
#include <stdio.h>

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	int i;
	//for(i = 0; i < line_num; i++){
	//	printf("%s", topo[i]);
	//}

	mcmf_network mcmf;
	mcmf.load_graph(topo, line_num);

	std::vector<int> servers;
	servers.push_back(0);
	servers.push_back(12);
	servers.push_back(15);
	servers.push_back(20);
	servers.push_back(22);
	servers.push_back(26);
	servers.push_back(37);
	servers.push_back(48);	

	printf("mcmf output is %d\n", mcmf.min_cost(servers));

	// 需要输出的内容
	char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);

}
