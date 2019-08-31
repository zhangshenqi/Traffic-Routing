// global-routing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdlib.h"
#include "malloc.h"
#include "time.h"
#include "math.h"
#include "iostream"
#include "queue"
#include "list"
#include "stack"
using namespace std;
#define MAXN1 1000
#define MAXN2 200000


int N;        //网络的结点数

typedef struct linked_list
{
	int node_num;        //结点编号        
//	int edge_num;        //连边编号
	struct linked_list *next;
} Linked_List;
Linked_List *adjacency_list[MAXN1];

typedef struct node
{
	int degree;
	int load;             //每一条队列的长度
	queue<int> Q;         //队列
} Node;
Node node[MAXN1];

typedef struct packet
{
	int destination;
	int node;
	bool moved;
	stack<int> St;
} Packet;
Packet packet[MAXN2];


int R;               //单位时间内产生包的数量
int capacity;        //每个结点的容量
double H;
int time_final;      //测试时间
int time1;           //取样时间1
int time2;           //取样时间2
int Np;              //网络中包的数量
int Np1;             //在取样时间1包的数量
int Np2;             //在取样时间2包的数量
stack<int> packet_free;      //用来存放空闲的包编号
list<int> packet_busy;       //用来存放正在使用的包编号


//将网络中两个结点连接
void network_connect(int node1, int node2)
{
	Linked_List *p;
	p = (Linked_List*)malloc(sizeof(Linked_List));
	p->node_num = node2;
	p->next = adjacency_list[node1];
	adjacency_list[node1] = p;
	p = (Linked_List*)malloc(sizeof(Linked_List));
	p->node_num = node1;
	p->next = adjacency_list[node2];
	adjacency_list[node2] = p;
}


//构造测试网络
void create_model()
{
	int i, j;
	int size;
//	printf("请输入边长：\n");
//	scanf("%d", &size);
	size = 6;
	N = size*size;
	srand((unsigned)time(NULL));
	for (i = 0; i<size - 1; i++)
		for (j = 0; j<size - 1; j++)
		{
			network_connect(i*size + j, i*size + j + 1);
			node[i*size + j].degree++;
			node[i*size + j + 1].degree++;
			network_connect(i*size + j, i*size + j + size);
			node[i*size + j].degree++;
			node[i*size + j + size].degree++;
		}
	for (i = 0; i<size - 1; i++)
	{
		network_connect(size*(size - 1) + i, size*(size - 1) + i + 1);
		node[size*(size - 1) + i].degree++;
		node[size*(size - 1) + i + 1].degree++;
		network_connect(i*size + size - 1, i*size + 2 * size - 1);
		node[i*size + size - 1].degree++;
		node[i*size + 2 * size - 1].degree++;
	}
	for (i = 0; i < N; i++) node[i].load = rand() % 9 + 1;
	for (i = 0; i < N; i++)
	{
		printf("%5d", node[i].load);
		if ((i + 1) % size == 0) printf("\n");
	}
}


//构造lattice网络 
void create_lattice()
{
	int i, j;
	int size;
	printf("请输入边长：\n");
	scanf("%d", &size);
	//	size = 35;
	N = size*size;
	srand((unsigned)time(NULL));
	for (i = 0; i<size - 1; i++)
		for (j = 0; j<size - 1; j++)
		{
			network_connect(i*size + j, i*size + j + 1);
			node[i*size + j].degree++;
			node[i*size + j + 1].degree++;
			network_connect(i*size + j, i*size + j + size);
			node[i*size + j].degree++;
			node[i*size + j + size].degree++;
		}
	for (i = 0; i<size - 1; i++)
	{
		network_connect(size*(size - 1) + i, size*(size - 1) + i + 1);
		node[size*(size - 1) + i].degree++;
		node[size*(size - 1) + i + 1].degree++;
		network_connect(i*size + size - 1, i*size + 2 * size - 1);
		node[i*size + size - 1].degree++;
		node[i*size + 2 * size - 1].degree++;
	}
	for (i = 0; i < size; i++)
	{
		network_connect(i, i + size*(size - 1));
		node[i].degree++;
		node[i + size*(size - 1)].degree++;
		network_connect(i*size, i*size + size - 1);
		node[i*size].degree++;
		node[i*size + size - 1].degree++;
	}
}


//构造BA网络
void create_BA()
{
	int i, j, k;
	int n0, nt, nc;       //原始结点数，增加的结点数，新增加的结点与原结点的连边数
//	printf("请输入原始结点数，增加的结点数，新增加的结点与原结点的连边数：\n");
//	scanf("%d%d%d", &n0, &nt, &nc);
	n0 = 5;
	nt = 495;
	nc = 2;
	N = n0 + nt;

	//初始化BA网络
	for (i = 0; i<n0; i++)
		for (j = 0; j < i; j++)
		{
			network_connect(i, j);
			node[i].degree++;
			node[j].degree++;
		}

	srand((unsigned)time(NULL));
	for (i = n0; i < N; i++)
	{
		bool connected[MAXN1] = { 0 };       //防止重复连边
		int denominator = 0;       //已存在结点的度数之和
		for (j = 0; j < i; j++) denominator += node[j].degree;

		j = 0;
		while (j < nc)
		{
			double threshold, probability;    //阈值，随机生成的概率
			bool flag;
			threshold = 0;
			flag = 1;
			probability = (rand() % 1000)*1.0 / 1000;
			k = 0;
			while (k < i&&flag == 1)
			{
				threshold += (node[k].degree * 1.0) / (denominator*1.0);
				if (probability < threshold)
				{
					flag = 0;
					if (connected[k] == 0)
					{
						network_connect(i, k);
						node[i].degree++;
						node[k].degree++;
						j++;
						connected[k] = 1;
					}
				}
				k++;
			}
		}
	}
}


//将网络的结点数存储到文件中
void fprint_N()
{
	FILE *fp;
	fp = fopen("N.txt", "w");
	fprintf(fp, "%d", N);
	fclose(fp);
}


//将网络中每个结点的度存储到文件中
void fprint_degree()
{
	int i;
	FILE *fp;
	fp = fopen("degree.txt", "w");
	for (i = 0; i < N; i++) fprintf(fp, "%d\n", node[i].degree);
	fclose(fp);
}


//将网络邻接表存储到文件中
void fprint_adjacency_list()
{
	int i;
	FILE *fp;
	fp = fopen("adjacency_list.txt", "w");
	for (i = 0; i<N; i++)
	{
		Linked_List *p;
		p = adjacency_list[i];
		while (p != NULL)
		{
			fprintf(fp, "%d ", p->node_num);
			p = p->next;
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}


//从文件中读取网络的结点数
void fscan_N()
{
	FILE *fp;
	fp = fopen("N.txt", "r");
	fscanf(fp, "%d", &N);
	fclose(fp);
}


//从文件中读取每个结点的度
void fscan_degree()
{
	int i;
	FILE *fp;
	fp = fopen("degree.txt", "r");
	for (i = 0; i < N; i++) fscanf(fp, "%d", &node[i].degree);
	fclose(fp);
}


//从文件中读取网络邻接表
void fscan_adjacency_list()
{
	int i, j;
	FILE *fp;
	fp = fopen("adjacency_list.txt", "r");
	for (i = 0; i < N; i++)
		for (j = 0; j < node[i].degree; j++)
		{
			int node;
			fscanf(fp, "%d", &node);
			if (node < i) network_connect(i, node);
		}
	fclose(fp);
}


//输出网络
void print_adjacency_list()
{
	int i;
	Linked_List *p;
	printf("\n输出网络：\n");
	for (i = 0; i < N; i++)
	{
		p = adjacency_list[i];
		printf("%d: ", i);
		while (p != NULL)
		{
			printf("%d ", p->node_num);
			p = p->next;
		}
		printf("\n");
	}
}


//输出与度有关的数值
void print_degree()
{
	int i;
	double degree_average;
	//	printf("\n输出各个结点的度：\n");
	//	for (i = 0; i < N; i++) printf("%d\n", degree[i]);
	degree_average = 0;
	printf("\n输出度平均：\n");
	for (i = 0; i < N; i++) degree_average += node[i].degree * 1.0;
	degree_average /= N;
	printf("%lf\n", degree_average);
}


//输出某一个包的参数
void print_packet(int packet_num)
{
	int i;
	printf("终点：%d\n", packet[packet_num].destination);
	printf("当前：%d\n", packet[packet_num].node);
	printf("\n");
	printf("\n");
}


//利用dijkstra算法，为一个包找到路径
void route_dijkstra(int packet_num, int source, int destination)
{
	int i;
	Linked_List *p;
	int now = source;   //当前所在位置
	int count;
	int route_before[MAXN1];
	int distance[MAXN1];
	int min_distance;
	bool visited[MAXN1] = { 0 };

	//初始化
	count = 1;
	for (i = 0; i < N; i++) distance[i] = MAXN2;
	distance[source] = 0;
	visited[source] = 1;

	//循环
	while (count < N)
	{
		//		printf("%5d", now);
		p = adjacency_list[now];

		//更新距离
		while (p != NULL)
		{
			if (visited[p->node_num] == 0 && node[p->node_num].load + distance[now] < distance[p->node_num])
			{
				distance[p->node_num] = node[p->node_num].load + distance[now];
				route_before[p->node_num] = now;
			}
			p = p->next;
		}

		//寻找下一个点
		min_distance = MAXN2;
		for (i = 0; i < N; i++)
		{
			if (visited[i] == 0 && distance[i] < min_distance)
			{
				min_distance = distance[i];
				now = i;
			}
		}
		visited[now] = 1;
		count++;
	}

	i = destination;
	while (i != source)
	{
		packet[packet_num].St.push(i);
		i = route_before[i];
	}
}


//对每一个队列进行更新
void test_fresh(int node_num)
{
	int i;
	Linked_List *p;
	bool flag;         //判断是否到达终点的标志
	int packet_num;
	for (i = 0; i < capacity; i++)
	{
		if (node[node_num].Q.empty()) break;
		if (packet[node[node_num].Q.front()].moved) break;
		packet_num = node[node_num].Q.front();
		node[node_num].Q.pop();
		packet[packet_num].node = packet[packet_num].St.top();
		packet[packet_num].St.pop();

		//如果下一个是终点
		if (packet[packet_num].node == packet[packet_num].destination)
		{
			Np--;
			packet_busy.remove(packet_num);
			packet_free.push(packet_num);
//			if (packet_num == 0) printf("消失**********************************************\n");
		}

		//如果下一个点不是终点
		else
		{
			node[packet[packet_num].node].Q.push(packet_num);
			packet[packet_num].moved = 1;
//			if (packet_num == 0) print_packet(0);
		}
	}
}


//进行测试
void test()
{
	int i, j, k;
	FILE *fp1 = fopen("H.txt", "w");
	FILE *fp2 = fopen("t.txt", "w");
	FILE *fp3 = fopen("Np.txt", "w");
//	printf("\n请输入测试时间：\n");
//	scanf("%d", &time_final);
	time_final = 2000;
	printf("\n请输入单位时间内新产生的包的数量：\n");
	scanf("%d", &R);
//	printf("\n请输入每个队列一次出队的包数：\n");
//	scanf("%d", &capacity);
	capacity = 1;
//	printf("\n请输入观察点1和观察点2：\n");
//	scanf("%d%d", &time1, &time2);
	time1 = 1699;
	time2 = 1999;
	Np = 0;
	for (i = MAXN2; i >= 0; i--) packet_free.push(i);

	//时间不断推进
	for (i = 0; i < time_final; i++)
	{
		printf("时间为：%d\n", i);
		fprintf(fp2, "%d\n", i);
		fprintf(fp3, "%d\n", Np);
		//初始化
		for (list<int>::iterator iter = packet_busy.begin(); iter != packet_busy.end(); iter++)
			packet[*iter].moved = 0;

		//对每一个队列进行更新
		for (j = 0; j < N; j++)
			if (!node[j].Q.empty())
			{
//				printf("%d\n", j);
				test_fresh(j);
			}

		//更新队列长度
		for (j = 0; j < N; j++)
		{
			node[j].load = node[j].Q.size() + 1;
//			printf("%d ", node[j].load);
		}

		//产生新的测试包
		for (j = 0; j < R; j++)
		{

			int source = 0, destination = 0;
			while (source == destination)
			{
				source = rand() % N;
				destination = rand() % N;
			}
			packet[packet_free.top()].destination = destination;
			packet[packet_free.top()].node = source;
			while (!packet[packet_free.top()].St.empty()) packet[packet_free.top()].St.pop();
			node[source].Q.push(packet_free.top());
			if (packet_free.top() == 0)
			{
//				printf("时间为：%d\n", i);
//				print_packet(0);
			}
//			printf("%d ", packet_free.top());
			route_dijkstra(packet_free.top(), source, destination);
			packet_busy.push_back(packet_free.top());
			packet_free.pop();
		}
		Np += R;

		//取样
		if (i == time1) Np1 = Np;
		if (i == time2) Np2 = Np;
	}

	//计算参数
	H = capacity*1.0 / (R*1.0)*(Np2*1.0 - Np1*1.0) / (time2*1.0 - time1*1.0);

	//输出参数
	fprintf(fp1, "%lf\n", H);
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
}


int main()
{
//	create_model();
//	create_lattice();
//	create_BA();
//	fprint_N();
//	fprint_degree();
//	fprint_adjacency_list();
	fscan_N();
	fscan_degree();
	fscan_adjacency_list();
//	print_adjacency_list();
//	print_degree();
	test();
	return 0;
}

