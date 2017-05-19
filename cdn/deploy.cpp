#include "deploy.h"
#include "graph.h"
#include <stdio.h>
#include "gasa.h"

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	int max_population = 26;
	double init_temperature = 20;
	double decay_factor = 0.99;
	double cross_probability = 0.5;
	double mutate_probability = 0.15;

	GASA program(init_temperature, max_population);
	

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(program.process(topo, line_num, decay_factor, cross_probability, mutate_probability), filename);
}
