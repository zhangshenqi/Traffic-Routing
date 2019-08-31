#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "time.h"
#include "math.h"
#define MAXN1 1225
#define MAXN2 100000

int N;                              // 结点数量
int degree[MAXN1] = { 0 };          // 度


struct node
{
	int num;
	struct node *next;
};


struct node *network_head[MAXN1];
struct node *route[MAXN1][MAXN1];
int route_before[MAXN1];


int test_begin[MAXN2];                  //¼ÇÂ¼Ò»¸öµãµÄ²âÊÔÆðµã
int test_end[MAXN2];                    //¼ÇÂ¼Ò»¸öµãµÄ²âÊÔÖÕµã 
int test_step[MAXN2] = { 0 };           //¼ÇÂ¼Ò»¸öµãµÄ²âÊÔ×ßµ½µÚ¼¸²½
int test_now[MAXN2];                    //¼ÇÂ¼Ò»¸öµãµÄ²âÊÔÏÖÔÚµÄÎ»ÖÃ
bool test_moved[MAXN2] = { 0 };         //¼ÇÂ¼ÔÚÕâ¸öÊ±¼ä¶ÎÄÚÒ»¸ö²âÊÔµãÊÇ·ñÒÑ¾­±»Å²¶¯
struct node *queue_head[MAXN1];         //Ã¿Ò»¸ö½áµãµÄ¶ÓÁÐ


int R_now;                             //¼ÇÂ¼µ±Ç°ÓÐ¶àÉÙ¸öµãÕýÔÚ²âÊÔ
int R_before;                          //¼ÇÂ¼Ç°Ò»¸öÊ±¼äÓÐ¶àÉÙ¸öµãÕýÔÚ²âÊÔ
int R_delta;                           //¼ÇÂ¼Õâ¸öÊ±¼äµãÏà¶ÔÓÚÉÏ¸öÊ±¼äµãµÄ²îÖµ
int R1;                                //¼ÇÂ¼Ê±¼ä1ÓÐ¶àÉÙ¸öµãÕýÔÚ²âÊÔ
int R2;                                //¼ÇÂ¼Ê±¼ä2ÓÐ¶àÉÙ¸öµãÕýÔÚ²âÊÔ
double H;                              //HÖµ 


FILE *fp1 = fopen("R_delta.txt", "w");                     //Ð´ÈëÎÄ¼þÖÐ 
FILE *fp2 = fopen("process.txt", "w");
FILE *fp3 = fopen("R.txt", "w");
FILE *fp4 = fopen("H.txt", "w");

//½«ÍøÂçÖÐÁ½¸ö½áµãÁ¬½Ó
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


//¹¹ÔìlatticeÍøÂç 
void create_lattice()
{
	int i, j;
	int size;
	printf("ÇëÊäÈë±ß³¤£º\n");
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


//¹¹ÔìBAÍøÂç
void create_BA()
{
	int i, j, k;
	int n0, nt, nc;       //Ô­Ê¼½áµãÊý£¬Ôö¼ÓµÄ½áµãÊý£¬ÐÂÔö¼ÓµÄ½áµãÓëÔ­½áµãµÄÁ¬±ßÊý
//	printf("ÇëÊäÈëÔ­Ê¼½áµãÊý£¬Ôö¼ÓµÄ½áµãÊý£¬ÐÂÔö¼ÓµÄ½áµãÓëÔ­½áµãµÄÁ¬±ßÊý£º\n");
//	scanf("%d%d%d", &n0, &nt, &nc);
	n0 = 5;
	nt = 1220;
	nc = 2;
	N = n0 + nt;

	//³õÊ¼»¯BAÍøÂç
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
		bool connected[MAXN1] = { 0 };       //·ÀÖ¹ÖØ¸´Á¬±ß
		int denominator = 0;       //ÒÑ´æÔÚ½áµãµÄ¶ÈÊýÖ®ºÍ
		for (j = 0; j < i; j++) denominator += degree[j];

		j = 0;
		while (j < nc)
		{
			double threshold, probability;    //ãÐÖµ£¬Ëæ»úÉú³ÉµÄ¸ÅÂÊ
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


//Êä³öÍøÂç
void print_network()
{
	int i;
	struct node *p;
	printf("\nÊä³öÍøÂç£º\n");
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


//Êä³öÓë¶ÈÓÐ¹ØµÄÊýÖµ
void print_degree()
{
	int i;
	printf("\nÊä³ö¸÷¸ö½áµãµÄ¶È£º\n");
	for (i = 0; i < N; i++) printf("%d\n", degree[i]);
}


//½«½áµã´æ´¢ÖÁsequenceÁ´±íÖÐ
struct node *sequence_in(struct node *sequence,int node)
{
	struct node *p;
	p = (struct node*)malloc(sizeof(struct node));
	p->num = node;
	p->next = sequence;
	sequence = p;
	return sequence;
}


//½«sequenceÁ´±íÉ¾³ý
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


//Êä³öÁ´±í
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


//±éÀúÃ¿ÕÒµ½Ò»¸öµãºó£¬»ØËÝ£¬²¢½«ÕâÌõÂ·¾¶´æ´¢
void route_retrieve(int begin, int end)
{
	int i;
	int level[MAXN1] = { 0 };
	int count = 0;

	//Éú³Élevel
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

	//ÓÃsequence½«Â·¾¶´æ´¢
	int sequence[MAXN1];
	sequence[0] = begin;
	i = end;
	while (i != begin)
	{
		sequence[level[i]] = i;
		i = route_before[i];
	}

	//ÓÃrouteÁ´±í½«Â·¾¶´æ´¢
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


//¹ã¶ÈÓÅÏÈ±éÀúÉú³É×î¶ÌÂ·¾¶
void route_BFS()
{
	int begin;         //Ã¿Ò»ÌõÂ·¾¶µÄÆðµã
	for (begin = 0; begin < N; begin++)
	{
		struct node *p, *q;    
		struct node *sequence1, *sequence2;  //ÓÃÀ´´æ´¢ÏÂÒ»²ãµÄ½áµã
		struct node *sequence_before[MAXN1];  //ÓÃÀ´´æ´¢¿Éµ½´ïÄ³Ò»¸ö½áµãµÄËùÓÐ½áµã
		int before_sum[MAXN1] = { 0 };     //ÓÃÀ´´æ´¢¿Éµ½´ïÄ³Ò»¸ö½áµãµÄËùÓÐ½áµã×ÜÊý
		bool flag = 1;
		bool visited[MAXN1] = { 0 };     //×ö±ê¼Ç
		int route_sum[MAXN1] = { 0 };    //´ÓÆðµãµ½Ä³µãµÄ×î¶ÌÂ·¾¶Êý
		int level[MAXN1] = { 0 };      //ËùÔÚ²ãÊý
		int level_count = 1;       //¼ÇÂ¼²ãÊý

		//³õÊ¼»¯
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

		//±éÀú
		while (flag == 1)
		{
			int i;
			int step;   //Ëæ»ú»¯Â·¾¶£¬sequence_beforeÖ¸Õë¸Ã×ßµÄ²½Êý
			flag = 0;
			level_count++;     //²ãÊýÔö¼Ó
			q = sequence1;
			while (q != NULL)
			{
				p = network_head[q->num];
				while (p != NULL)
				{
					if (visited[p->num] == 0)     //Èç¹ûÊÇ»¹Ã»ÓÐ·ÃÎÊ¹ýµÄ½áµã
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
						if (level[p->num] == level_count)    //Èç¹ûÊÇÍ¬Ò»²ãÒÑ¾­·ÃÎÊ¹ýµÄ½áµã
						{
							route_sum[p->num] += route_sum[q->num];
							sequence_before[p->num] = sequence_in(sequence_before[p->num], q->num);
							before_sum[p->num]++;
						}
					p = p->next;
				}
				q = q->next;
			}

			//Ëæ»úÉú³ÉÂ·¾¶£¬²¢½øÐÐ»ØËÝ
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

			//¶Ôsequence1ºÍsequence2½øÐÐ´¦Àí
			sequence_destroy(sequence1);
			sequence1 = sequence2;
			sequence2 = NULL;
		}
	}
}


//ÀûÓÃdijkstraËã·¨ÕÒµ½ÓÐÐ§Â·¾¶
void route_dijkstra()
{
	int i;
	int begin;      //Æðµã
	int now;               //µ±Ç°ËùÔÚµÄ½áµã
	int count;             //ÒÑ¾­±éÀú¹ýµÄ½áµãÊý
	bool visited[MAXN1];
	struct node*p;

	for (begin = 0; begin < N; begin++)
	{
		int distance[MAXN1];

		//³õÊ¼»¯
		for (i = 0; i < N; i++) visited[i] = 0;
		visited[begin] = 1;
		for (i = 0; i < N; i++) distance[i] = MAXN2;
		distance[begin] = degree[begin];
		count = 1;
		now = begin;

		//±éÀú
		while (count <= N)
		{
			struct node *sequence_next;   //ÓÃÀ´´æ´¢Âú×ãÒªÇóµÄÏÂÒ»¸öµã
			int next_sum = 0;    //¼ÇÂ¼Âú×ãÒªÇóµÄÏÂÒ»¸öµãµÄÊýÄ¿ 
			int min_k = MAXN2;    //×îÐ¡µÄ¶ÈÊý
			int step;   //Ëæ»ú»¯ÏÂÒ»¸öµã£¬Ö¸Õë¸Ã×ßµÄ²½Êý

			//³õÊ¼»¯
			sequence_next = NULL;
			p = network_head[now];

			//¸üÐÂ¾àÀë
			while (p != NULL)
			{
				if (visited[p->num] == 0 && degree[p->num] + distance[now] < distance[p->num])
				{
					distance[p->num] = degree[p->num] + distance[now];
					route_before[p->num] = now;
				}
				p = p->next;
			}
			
			//Ñ°ÕÒÒ»ÏÂ¸öµã
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


//Êä³öËùÓÐÂ·¾¶
void print_route()
{
	int i, j;
	printf("\nÊä³öËùÓÐÂ·¾¶£º\n");
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


//Í³¼Æ¾­¹ýÃ¿Ò»¸öµãµÄÂ·¾¶×ÜÊý£¬°üÀ¨Æðµã£¬²»°üÀ¨ÖÕµã
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


//Èë¶Ó
void queue_in(int ii, int num)    //ii±íÊ¾ÍøÂçÖÐ½áµãÐòºÅ£¬num±íÊ¾ÒªÈë¶ÓµÄ²âÊÔµãÐòºÅ
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


//³ö¶Ó,²¢ÇÒ·µ»Ø³ö¶Ó²âÊÔµãµÄÐòºÅ
int queue_out(int ii)             //ii±íÊ¾ÍøÂçÖÐµÄ½áµãÐòºÅ
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


//ÕÒµ½Ò»¸öµãµÄ²âÊÔµÄÏÂÒ»¸öµã
int test_nextnode(int num)
{
	struct node *p;
	p = route[test_begin[num]][test_end[num]];
	int i;
	for (i = 0; i < test_step[num]; i++) p = p->next;
	if (p != NULL) return p->num;
	else return MAXN1;
}


//¶ÔÃ¿Ò»¸ö¶ÓÁÐ½øÐÐ¸üÐÂ
void test_fresh(int ii)                     //ii±íÊ¾ÍøÂçÖÐµÄ½áµãÐòºÅ
{
	int num, nextnode;
	if (test_moved[queue_head[ii]->num] == 0)      //¸üÐÂµÄÌõ¼þÊÇÁ´±íµÄÊ×¸ö²âÊÔµã»¹Ã»ÓÐÒÆ¶¯¹ý
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


//½«¶ÓÁÐÁÚ½Ó±íÊä³ö
void print_queue()
{
	int i;
	struct node *p;
	fprintf(fp2, "\nÊä³ö¶ÓÁÐÁÚ½Ó±í£º\n");
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


//½øÐÐ²âÊÔ
void test()
{
	int i, j;                 //i±íÊ¾Ê±¼äµã£¬j±íÊ¾Ã¿Ò»¸öÊ±¼äµãÐÂ²úÉúµÄ°üµÄÐòºÅ
	int time;                 //time±íÊ¾²âÊÔÊ±¼ä 
	int R;                    //µ¥Î»Ê±¼äÄÚ²úÉúµÄ°üµÄÊýÄ¿ 
	int test_count = 0;       //route_countÓÃÀ´ÎªÂ·¾¶±êºÅ
//	printf("\nÇëÊäÈë²âÊÔÊ±¼ä£º\n");
//	scanf("%d", &time);
	time = 5000;
	printf("\nÇëÊäÈëµ¥Î»Ê±¼äÄÚÐÂ²úÉúµÄ°üµÄÊýÁ¿£º\n");
	scanf("%d", &R);
	R_now = 0;
	R_before = 0;


	//Ê±¼ä²»¶Ï¸üÐÂ
	for (i = 0; i<time; i++)
	{
		R_before = R_now;
		//		                                    		fprintf(fp2,"Ê±¼äÎª%d\n", i);

		//Ã¿Ò»¸öÊ±¼äµã¿ªÊ¼¶Ôtest_moved½øÐÐ¸üÐÂ
		for (j = 0; j < test_count; j++) test_moved[j] = 0;


		//¶ÔÃ¿Ò»¸ö¶ÓÁÐ½øÐÐ¸üÐÂ
		for (j = 0; j < N; j++)
			if (queue_head[j] != NULL) test_fresh(j);

		//²úÉúÐÂµÄµã
		//		                                    		fprintf(fp2,"²úÉúÐÂµÄ²âÊÔµã£º\n");
		for (j = 0; j<R; j++)
		{
			int begin = 0, end = 0;              //ÆðµãºÍÖÕµã
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
	
		//ÒÔÏÂ³ÌÐò½öÕë¶Ô²âÊÔ£¬È¡µÚ400ºÍ500¸öRÖµ
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

