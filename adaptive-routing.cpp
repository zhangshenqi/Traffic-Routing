// adaptive-routing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "iostream"
#include "queue"
#include "list"
#include "stack"
using namespace std;
#define MAXN1 1500
#define MAXN2 2000000


int N = 1500;
double velocity = 0.3;
double radius = 1.0;
int L = 10;      //cell的边长
int capacity = 1;
int R;      //单位时间内产生包的数量
double h = 0.5;
int time_final;      //测试时间
int time1;      //取样时间1
int time2;      //取样时间2
int Np;      //网络中包的数量
int Np1;      //在取样时间1包的数量
int Np2;      //在取样时间2包的数量
double H;
stack<int> packet_free;      //用来存放空闲的包编号
list<int> packet_busy;      //用来存放正在使用的包编号

struct
{
	double x;
	double y;
	int load;      //队列长度
	queue<int> Q;
} node[MAXN1];

struct
{
	int destination;
	int node;
	bool moved;
} packet[MAXN2];



//建立网络
void create_network()
{
	int i;
	srand((unsigned)time(NULL));
	for (i = 0; i < N; i++)
	{
		node[i].x = (rand() % 9999 + 1)*1.0 / 10000 * L;
		node[i].y = (rand() % 9999 + 1)*1.0 / 10000 * L;
	}
}


//计算两个结点间的距离
double calculate_distance(int i, int j)
{
	double distance;
	distance = sqrt((node[i].x - node[j].x)*(node[i].x - node[j].x) + (node[i].y - node[j].y)*(node[i].y - node[j].y));
	return distance;
}


//判断两个结点间距离是否在通信半径内
bool within_scope(int i, int j)
{
	double distance;
	distance = sqrt((node[i].x - node[j].x)*(node[i].x - node[j].x) + (node[i].y - node[j].y)*(node[i].y - node[j].y));
	if (distance < radius) return 1;
	else return 0;
}


//判断某个结点是否在cell内
bool within_cell(double x, double y)
{
	if (x <= 0) return 0;
	if (x >= L) return 0;
	if (y <= 0) return 0;
	if (y >= L) return 0;
	return 1;
}


//计算有效距离
double calculate_effective_distance(int i, int j)
{
	double effective_distance;
	effective_distance = h*calculate_distance(i, j) / radius + (1 - h)*node[i].load / capacity;
	return effective_distance;
}


//对每一个队列进行更新
void queue_fresh(int node_num)
{
	int i, j;
	int packet_num;
	bool found;      //如果没有一个结点在范围内，则为0
	double effective_distance_min;
	int next_node_num;
	for (i = 0; i < capacity; i++)
	{
		if (node[node_num].Q.empty()) break;
		if (packet[node[node_num].Q.front()].moved) break;
		packet_num = node[node_num].Q.front();

		//如果终点在通信范围内
		if (within_scope(node_num, packet[packet_num].destination) == 1)
		{
			Np--;
			node[node_num].Q.pop();
			packet_busy.remove(packet_num);
			packet_free.push(packet_num);
		}

		//如果终点不在通信范围内
		else
		{
			found = 0;
			effective_distance_min = MAXN1;
			for (j = 0; j < N; j++)
			{
				if (j != node_num&&j != packet[packet_num].destination)
				{
					if (within_scope(i, j) == 1)
					{
						found = 1;
						if (calculate_effective_distance(j, packet[packet_num].destination) < effective_distance_min)
						{
							effective_distance_min = calculate_effective_distance(j, packet[packet_num].destination);
							next_node_num = j;
						}
					}
				}
			}

			//如果找到了在通信范围内的结点
			if (found == 1)
			{
				node[node_num].Q.pop();
				node[next_node_num].Q.push(packet_num);
				packet[packet_num].node = next_node_num;
				packet[packet_num].moved = 1;
			}
		}
	}
}


//对网络进行更新
void network_fresh()
{
	int i;
	bool flag;
	double theta;
	for (i = 0; i < N; i++)
	{
		flag = 0;
		while (flag == 0)
		{
			theta = rand() % 6284 * 1.0 / 1000;
			if (theta == 6.283)
				theta += rand() % 186 * 1.0 / 1000000;
			else
				theta += rand() % 1000 * 1.0 / 1000000;

			if (within_cell(node[i].x + velocity*cos(theta), node[i].y + velocity*sin(theta)) == 1)
			{
				node[i].x += velocity*cos(theta);
				node[i].y += velocity*sin(theta);
				flag = 1;
			}
		}
	}
}


//进行测试
void test()
{
	int i, j;
	int Np_before = 0;
	int Np_delta = 0;
	FILE *fp1 = fopen("H.txt", "w");
	FILE *fp2 = fopen("time.txt", "w");
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
		Np_delta = Np - Np_before;
		Np_before = Np;
		printf("时间为：%d", i);
		fprintf(fp2, "%d\n", i);
		printf("    %d\n", Np_delta);
		fprintf(fp3, "%d\n", Np);
		//初始化
		for (list<int>::iterator iter = packet_busy.begin(); iter != packet_busy.end(); iter++)
			packet[*iter].moved = 0;
		for (j = 0; j < N; j++) node[i].load = node[i].Q.size();

		//对每一个队列进行更新
		for (j = 0; j < N; j++)
			if (!node[j].Q.empty())
			{
				queue_fresh(j);
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
//			printf("%d ", packet_free.top());
			packet[packet_free.top()].destination = destination;
			packet[packet_free.top()].node = source;
			node[source].Q.push(packet_free.top());
			packet_busy.push_back(packet_free.top());
			packet_free.pop();
		}
		Np += R;

		//更新网络
		network_fresh();

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
	create_network();
	test();
	return 0;
}



