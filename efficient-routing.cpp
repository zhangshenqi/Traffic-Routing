#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "time.h"
#include "math.h"
#define MAXN1 1225
#define MAXN2 100000


int N;                              // Number of nodes
int degree[MAXN1] = { 0 };          // Degree


struct node
{
	int num;
	struct node *next;
};


struct node *network_head[MAXN1];


struct node *route[MAXN1][MAXN1];
int route_before[MAXN1];


int test_begin[MAXN2];                  //记录一个点的测试起点
int test_end[MAXN2];                    //记录一个点的测试终点 
int test_step[MAXN2] = { 0 };           //记录一个点的测试走到第几步
int test_now[MAXN2];                    //记录一个点的测试现在的位置
bool test_moved[MAXN2] = { 0 };         //记录在这个时间段内一个测试点是否已经被挪动
struct node *queue_head[MAXN1];         //每一个结点的队列


int R_now;                             //记录当前有多少个点正在测试
int R_before;                          //记录前一个时间有多少个点正在测试
int R_delta;                           //记录这个时间点相对于上个时间点的差值
int R1;                                //记录时间1有多少个点正在测试
int R2;                                //记录时间2有多少个点正在测试
double H;                              //H值 


FILE *fp1 = fopen("R_delta.txt", "w");                     //写入文件中 
FILE *fp2 = fopen("process.txt", "w");
FILE *fp3 = fopen("R.txt", "w");
FILE *fp4 = fopen("H.txt", "w");

//将网络中两个结点连接
void network_connect(int node1, int node2)
{
	struct node *p;
	p = (struct node*)malloc(sizeof(struct node));
	p->num = node2;
	p->next = network_head[node1];
	network_head[node1] = p;

	p = (struct node*)malloc(sizeof(struct node));
	p->num = node1;
	p->next = network_head[node2];
	network_head[node2] = p;
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
			degree[i*size + j] ++;
			degree[i*size + j + 1]++;
			network_connect(i*size + j, i*size + j + size);
			degree[i*size + j] ++;
			degree[i*size + j + size]++;
		}
	for (i = 0; i<size - 1; i++)
	{
		network_connect(size*(size - 1) + i, size*(size - 1) + i + 1);
		degree[size*(size - 1) + i]++;
		degree[size*(size - 1) + i + 1]++;
		network_connect(i*size + size - 1, i*size + 2 * size - 1);
		degree[i*size + size - 1]++;
		degree[i*size + 2 * size - 1]++;
	}
	for (i = 0; i < size; i++)
	{
		network_connect(i, i + size*(size - 1));
		degree[i]++;
		degree[i + size*(size - 1)]++;
		network_connect(i*size, i*size + size - 1);
		degree[i*size]++;
		degree[i*size + size - 1]++;
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
	nt = 1220;
	nc = 2;
	N = n0 + nt;

	//初始化BA网络
	for(i=0;i<n0;i++)
		for (j = 0; j < i; j++)
		{
			network_connect(i, j);
			degree[i]++;
			degree[j]++;
		}

	srand((unsigned)time(NULL));
	for (i = n0; i < N; i++)
	{
		bool connected[MAXN1] = { 0 };       //防止重复连边
		int denominator = 0;       //已存在结点的度数之和
		for (j = 0; j < i; j++) denominator += degree[j];

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
				threshold += (degree[k] * 1.0) / (denominator*1.0);
				if (probability < threshold)
				{
					flag = 0;
					if (connected[k] == 0)
					{
						network_connect(i, k);
						degree[i]++;
						degree[k]++;
						j++;
						connected[k] = 1;
					}
				}
				k++;
			}
		}
	}
}


//输出网络
void print_network()
{
	int i;
	struct node *p;
	printf("\n输出网络：\n");
	for (i = 0; i < N; i++)
	{
		p = network_head[i];
		printf("%d: ", i);
		while (p != NULL)
		{
			printf("%d ", p->num);
			p = p->next;
		}
		printf("\n");
	}
}


//输出与度有关的数值
void print_degree()
{
	int i;
	printf("\n输出各个结点的度：\n");
	for (i = 0; i < N; i++) printf("%d\n", degree[i]);
}


//将结点存储至sequence链表中
struct node *sequence_in(struct node *sequence,int node)
{
	struct node *p;
	p = (struct node*)malloc(sizeof(struct node));
	p->num = node;
	p->next = sequence;
	sequence = p;
	return sequence;
}


//将sequence链表删除
void sequence_destroy(struct node *sequence)
{
	struct node *p, *q;
	p = sequence;
	while (p != NULL)
	{
		q = p->next;
		free(p);
		p = q;
	}
}


//输出链表
void print_sequence(struct node *sequence)
{
	struct node *p;
	p = sequence;
	while (p != NULL)
	{
		printf("%d ", p->num);
		p = p->next;
	}
	printf("\n");
}


//遍历每找到一个点后，回溯，并将这条路径存储
void route_retrieve(int begin, int end)
{
	int i;
	int level[MAXN1] = { 0 };
	int count = 0;

	//生成level
	i = end;
	while (i != begin)
	{
		level[i] = count;
		count++;
		i = route_before[i];
	}
	i = end;
	while (i != begin)
	{
		level[i] = count - level[i];
		i = route_before[i];
	}

	//用sequence将路径存储
	int sequence[MAXN1];
	sequence[0] = begin;
	i = end;
	while (i != begin)
	{
		sequence[level[i]] = i;
		i = route_before[i];
	}

	//用route链表将路径存储
	struct node *p, *q;
	route[begin][end] = NULL;
	for (i = 1; i < level[end]; i++)
	{
		q = (struct node*)malloc(sizeof(struct node));
		q->num = sequence[i];
		q->next = NULL;
		if (i == 1)
		{
			route[begin][end] = q;
			p = route[begin][end];
		}
		else
		{
			p->next = q;
			p = p->next;
		}
	}
}


//广度优先遍历生成最短路径
void route_BFS()
{
	int begin;         //每一条路径的起点
	for (begin = 0; begin < N; begin++)
	{
		struct node *p, *q;    
		struct node *sequence1, *sequence2;  //用来存储下一层的结点
		struct node *sequence_before[MAXN1];  //用来存储可到达某一个结点的所有结点
		int before_sum[MAXN1] = { 0 };     //用来存储可到达某一个结点的所有结点总数
		bool flag = 1;
		bool visited[MAXN1] = { 0 };     //做标记
		int route_sum[MAXN1] = { 0 };    //从起点到某点的最短路径数
		int level[MAXN1] = { 0 };      //所在层数
		int level_count = 1;       //记录层数


		//初始化
		visited[begin] = 1;
		p = network_head[begin];
		sequence1 = NULL;
		sequence2 = NULL;
		for (int i = 0; i < N; i++) sequence_before[i] = NULL;
		while (p != NULL)
		{
			sequence1 = sequence_in(sequence1, p->num);
			visited[p->num] = 1;
			route_sum[p->num] = 1;
			level[p->num] = level_count;
			sequence_before[p->num] = sequence_in(sequence_before[p->num], begin);
			before_sum[p->num]++;
			route_before[p->num] = begin;
			route_retrieve(begin, p->num);
			p = p->next;
		}

		//遍历
		while (flag == 1)
		{
			int i;
			int step;   //随机化路径，sequence_before指针该走的步数
			flag = 0;
			level_count++;     //层数增加
			q = sequence1;
			while (q != NULL)
			{
				p = network_head[q->num];
				while (p != NULL)
				{
					if (visited[p->num] == 0)     //如果是还没有访问过的结点
					{
						flag = 1;
						sequence2 = sequence_in(sequence2, p->num);
						visited[p->num] = 1;
						route_sum[p->num] += route_sum[q->num];
						level[p->num] = level_count;
						sequence_before[p->num] = sequence_in(sequence_before[p->num], q->num);
						before_sum[p->num]++;
					}
					else
						if (level[p->num] == level_count)    //如果是同一层已经访问过的结点
						{
							route_sum[p->num] += route_sum[q->num];
							sequence_before[p->num] = sequence_in(sequence_before[p->num], q->num);
							before_sum[p->num]++;
						}
					p = p->next;
				}
				q = q->next;
			}

			//随机生成路径，并进行回溯
			p = sequence2;
			while (p != NULL)
			{
				q = sequence_before[p->num];
				if (before_sum[p->num] > 1)
				{
					step = rand() % before_sum[p->num];
					for (i = 0; i < step; i++) q = q->next;
				}
				route_before[p->num] = q->num;
				route_retrieve(begin, p->num);
				p = p->next;
			}

			//对sequence1和sequence2进行处理
			sequence_destroy(sequence1);
			sequence1 = sequence2;
			sequence2 = NULL;
		}
	}
}


//利用dijkstra算法找到有效路径
void route_dijkstra()
{
	int i;
	int begin;      //起点
	int now;               //当前所在的结点
	int count;             //已经遍历过的结点数
	bool visited[MAXN1];
	struct node*p;

	for (begin = 0; begin < N; begin++)
	{
		int distance[MAXN1];

		//初始化
		for (i = 0; i < N; i++) visited[i] = 0;
		visited[begin] = 1;
		for (i = 0; i < N; i++) distance[i] = MAXN2;
		distance[begin] = degree[begin];
		count = 1;
		now = begin;

		//遍历
		while (count <= N)
		{
			struct node *sequence_next;   //用来存储满足要求的下一个点
			int next_sum = 0;    //记录满足要求的下一个点的数目 
			int min_k = MAXN2;    //最小的度数
			int step;   //随机化下一个点，指针该走的步数

			//初始化
			sequence_next = NULL;
			p = network_head[now];

			//更新距离
			while (p != NULL)
			{
				if (visited[p->num] == 0 && degree[p->num] + distance[now] < distance[p->num])
				{
					distance[p->num] = degree[p->num] + distance[now];
					route_before[p->num] = now;
				}
				p = p->next;
			}
			
			//寻找一下个点
			for (i = 0; i < N; i++)
			{
				if (visited[i] == 0&&distance[i] < MAXN2)
				{
					if (distance[i] < min_k)
					{
						sequence_destroy(sequence_next);
						sequence_next = NULL;
						next_sum = 0;
						min_k = distance[i];
						sequence_next = sequence_in(sequence_next, i);
						next_sum++;
					}
					else
					{
						if (distance[i] == min_k)
						{
							sequence_next = sequence_in(sequence_next, i);
							next_sum++;
						}
					}
				}
			}
			if (sequence_next != NULL)
			{
				p = sequence_next;
				if (next_sum > 1)
				{
					step = rand() % next_sum;
					for (i = 0; i < step; i++) p = p->next;
				}
				now = p->num;
				visited[now] = 1;
				route_retrieve(begin, now);
			}
			count++;
		}
	}
}


//输出所有路径
void print_route()
{
	int i, j;
	printf("\n输出所有路径：\n");
	for (i = 0; i<N; i++)
		for (j = 0; j<N; j++)
			if (i != j)
			{
				struct node *p;
				p = route[i][j];
				printf("%d->", i);
				while (p != NULL)
				{
					printf("%d->", p->num);
					p = p->next;
				}
				printf("%d\n", j);
			}
}


//统计经过每一个点的路径总数，包括起点，不包括终点
void print_distribution()
{
	int i, j;
	int distribution[MAXN1] = { 0 };
	for (i = 0; i<N; i++)
		for (j = 0; j<N; j++)
			if (i != j)
			{
				distribution[i]++;
				struct node *p;
				p = route[i][j];
				while (p != NULL)
				{
					distribution[p->num]++;
					p = p->next;
				}
			}
	for (i = 0; i < N; i++)
	{
		printf("%10d", distribution[i]);
		if ((i + 1) % 12 == 0) printf("\n");
	}
}


//入队
void queue_in(int ii, int num)    //ii表示网络中结点序号，num表示要入队的测试点序号
{
	struct node *p, *q;
	p = queue_head[ii];
	if (p == NULL)
	{
		p = (struct node*)malloc(sizeof(struct node));
		p->num = num;
		p->next = NULL;
		queue_head[ii] = p;
	}
	else
	{
		while (p->next != NULL)
			p = p->next;
		q = (struct node*)malloc(sizeof(struct node));
		q->num = num;
		q->next = NULL;
		p->next = q;
	}
}


//出队,并且返回出队测试点的序号
int queue_out(int ii)             //ii表示网络中的结点序号
{
	int num;
	struct node *p, *q;
	num = queue_head[ii]->num;
	if (queue_head[ii]->next == NULL) queue_head[ii] = NULL;
	else
	{
		p = queue_head[ii]->next;
		q = queue_head[ii];
		free(q);
		queue_head[ii] = p;
	}
	test_moved[num] = 1;
	return num;
}


//找到一个点的测试的下一个点
int test_nextnode(int num)
{
	struct node *p;
	p = route[test_begin[num]][test_end[num]];
	int i;
	for (i = 0; i < test_step[num]; i++) p = p->next;
	if (p != NULL) return p->num;
	else return MAXN1;
}


//对每一个队列进行更新
void test_fresh(int ii)                     //ii表示网络中的结点序号
{
	int num, nextnode;
	if (test_moved[queue_head[ii]->num] == 0)      //更新的条件是链表的首个测试点还没有移动过
	{
		num = queue_out(ii);
		nextnode = test_nextnode(num);
		if (nextnode < MAXN1)
		{
			queue_in(nextnode, num);
			test_now[num] = nextnode;
			test_step[num]++;
		}
		else R_now--;
	}
}


//将队列邻接表输出
void print_queue()
{
	int i;
	struct node *p;
	fprintf(fp2, "\n输出队列邻接表：\n");
	for (i = 0; i<N; i++)
	{
		p = queue_head[i];
		if (queue_head[i] == NULL)
		{
			fprintf(fp2, "%d:\n", i);
		}
		else
		{
			fprintf(fp2, "%d: ", i);
			do
			{
				fprintf(fp2, "%d ", p->num);
				p = p->next;
			} while (p != NULL);
			fprintf(fp2, "\n");
		}
	}
}


//进行测试
void test()
{
	int i, j;                 //i表示时间点，j表示每一个时间点新产生的包的序号
	int time;                 //time表示测试时间 
	int R;                    //单位时间内产生的包的数目 
	int test_count = 0;       //route_count用来为路径标号
//	printf("\n请输入测试时间：\n");
//	scanf("%d", &time);
	time = 5000;
	printf("\n请输入单位时间内新产生的包的数量：\n");
	scanf("%d", &R);
	R_now = 0;
	R_before = 0;


	//时间不断更新
	for (i = 0; i<time; i++)
	{
		R_before = R_now;
		//		                                    		fprintf(fp2,"时间为%d\n", i);

		//每一个时间点开始对test_moved进行更新
		for (j = 0; j < test_count; j++) test_moved[j] = 0;


		//对每一个队列进行更新
		for (j = 0; j < N; j++)
			if (queue_head[j] != NULL) test_fresh(j);

		//产生新的点
		//		                                    		fprintf(fp2,"产生新的测试点：\n");
		for (j = 0; j<R; j++)
		{
			int begin = 0, end = 0;              //起点和终点
			while (begin == end)
			{
				begin = rand() % N;
				end = rand() % N;
			}
			//						                            fprintf(fp2,"%d %d\n", begin, end);
			test_begin[test_count] = begin;
			test_end[test_count] = end;
			test_now[test_count] = begin;
			queue_in(begin, test_count);
			test_count++;
		}
		R_now = R_now + R;
		R_delta = R_now - R_before;
//		fprintf(fp1, "R_delta(%d)=%d R_now(%d)=%d\n", i, R_delta, i, R_now);
		//		                                        print_queue();
		//		                                        fprintf(fp2,"\n");
	
		//以下程序仅针对测试，取第400和500个R值
		if (i == 4499) R1 = R_now;
		if (i == 4999) R2 = R_now;
	}

	H = 1.0 / (R*1.0)*((R2 - R1)*1.0) / 500;
	fprintf(fp3, "%d\n", R);
	fprintf(fp4, "%lf\n", H);
//	fclose(fp1);
	//	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
}


int main()
{
	create_lattice();
//	create_BA();
//	print_network();
//	print_degree();
	route_BFS();
//	route_dijkstra();
	print_route();
	print_distribution();
//	test();
	return 0;
}

