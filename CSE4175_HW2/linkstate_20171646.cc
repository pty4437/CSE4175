#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <vector>
#include <queue>
#include <utility>
#include <iostream>
#include <algorithm>

using namespace std;

typedef struct _NODE {
	int cost;
	vector<int> next_node;
} node;

FILE* topology_file, * messages_file, * changes_file, * output_file;
node vector_table[100][100];
int init_table[100][100];

int numNode;
const int max_num = 987654321;


void init_vector_table() {
	char line[30];

	fgets(line, sizeof(line), topology_file);
	numNode = atoi(line);

	for (int i = 0; i < numNode; i++) {
		for (int j = 0; j < numNode; j++) {
			init_table[i][j] = 0;
			if (i != j) {
				vector_table[i][j].cost = max_num;
				//vector_table[i][j].next_node.push_back = -1;
			}
			else {
				vector_table[i][j].cost = 0;
				vector_table[i][j].next_node.push_back(i);
			}
		}
	}

	while (fgets(line, sizeof(line), topology_file) != NULL) {
		char tmp[30] = "";
		char* ptr;
		int start, end, cost;

		strcpy(tmp, line);

		ptr = strtok(tmp, " ");
		start = atoi(ptr);

		ptr = strtok(NULL, " ");
		end = atoi(ptr);

		ptr = strtok(NULL, " ");
		cost = atoi(ptr);

		init_table[start][end] = cost;
		init_table[end][start] = cost;

		vector_table[start][end].cost = cost;
		vector_table[start][end].next_node.push_back(end);
		vector_table[end][start].cost = cost;
		vector_table[end][start].next_node.push_back(start);;
	}

}

void print_vector_table() {
	for (int i = 0; i < numNode; i++) {
		for (int j = 0; j < numNode; j++) {
			if (vector_table[i][j].cost != max_num) {
				int size = vector_table[i][j].next_node.size();
				
				if (size == 1)
					fprintf(output_file, "%d %d %d\n", j, vector_table[i][j].next_node.back(), vector_table[i][j].cost);
				else {
					fprintf(output_file, "%d %d %d\n", j, vector_table[i][j].next_node.front(), vector_table[i][j].cost);
				}

			}
			else
				fprintf(output_file, "%d - -\n", j);
		}
		fprintf(output_file, "\n");
	}
}

void make_road(int start, int end, int tmp) {
	vector_table[start][end].next_node.clear();
	vector_table[start][end].next_node = vector_table[start][tmp].next_node;

	for (int i = 0; i < (int)vector_table[tmp][end].next_node.size(); i++) {
		vector_table[start][end].next_node.push_back(vector_table[tmp][end].next_node[i]);
	}
}

void func_linkstate() {

	for (int start = 0; start < numNode; start++) {
		int dist[100], visited[100], prev[100];

		for (int i = 0; i < 100; i++) {
			dist[i] = max_num;
			visited[i] = 0;
			prev[i] = 0;
		}

		priority_queue<pair<int, int>> pq;
		dist[start] = 0;
		pq.push({ -1, start });

		while (!pq.empty()) {
			int cur = pq.top().second; pq.pop();
			if (visited[cur]) continue;

			visited[cur] = 1;

			for (int i = 0; i < numNode; i++) {
				if (init_table[cur][i]) {
					int next = i, cost = init_table[cur][i];
					int next_dist = dist[cur] + cost;
					if (dist[next] > next_dist) {
						dist[next] = next_dist;
						prev[next] = cur;
						pq.push({ -next_dist, next });
					}
				}
			}
		}

		for (int i = 0; i < numNode; i++) {
			vector_table[start][i].cost = dist[i];
		}

		for (int i = 0; i < numNode; i++) {
			vector_table[start][i].next_node.clear();
			if (start == i) 
				vector_table[start][i].next_node.push_back(i);
			else {
				int tmp = i;
				while (tmp != start) {
					vector_table[start][i].next_node.push_back(tmp);
					tmp = prev[tmp];
				}
				reverse(vector_table[start][i].next_node.begin(), vector_table[start][i].next_node.end());
			}

		}
	}
}

void solve_message() {
	char line[100] = "";

	while (fgets(line, sizeof(line), messages_file) != NULL) {
		char tmp[100] = "";
		char* ptr;
		int start, end;
		char message[100] = "";

		strcpy(tmp, line);

		ptr = strtok(tmp, " ");
		start = atoi(ptr);

		ptr = strtok(NULL, " ");
		end = atoi(ptr);

		ptr = strtok(NULL, "\0");
		strcpy(message, ptr);

		fprintf(output_file, "from %d to %d cost %d hops ", start, end, vector_table[start][end].cost);

		fprintf(output_file, "%d ", start);

		for (int i = 0; i < (int)vector_table[start][end].next_node.size() - 1; i++)
			fprintf(output_file, "%d ", vector_table[start][end].next_node[i]);

		message[strlen(message)-1] = '\n';
		fprintf(output_file, "message %s", message);
	}
}

void solve_changes(int start, int end, int cost) {
	if (cost == -999) cost = max_num;

	init_table[start][end] = cost;
	init_table[end][start] = cost;

	for (int i = 0; i < numNode; i++) {
		for (int j = 0; j < numNode; j++) {
			vector_table[i][j].next_node.clear();
			if (i == j) {
				vector_table[i][j].cost = 0;
			}
			else {
				vector_table[i][j].cost = max_num;
			}
		}
	}

	for (int i = 0; i < numNode; i++) {
		
		for (int j = 0; j < numNode; j++) {
			if (init_table[i][j]) {
				vector_table[i][j].cost = init_table[i][j];
				//vector_table[i][j].next_node.push_back(j);
			}
		}
	}


	func_linkstate();
}

int main(int argc, char* argv[]) {

	//인자의 수가 맞지 않음
	if (argc != 4) {
		printf("usage: linkstate topologyfile messagesfile changesfile\n");
		exit(0);
	}

	topology_file = fopen(argv[1], "r+");
	//topology_file = fopen("topology.txt", "r");

	//입력 파일 존재하지 않는 경우
	if (topology_file == NULL) {
		printf("Error: open input file.\n");
		exit(0);
	}

	messages_file = fopen(argv[2], "r+");
	//messages_file = fopen("messagesfile.txt", "r");

	//입력 파일 존재하지 않는 경우
	if (messages_file == NULL) {
		printf("Error: open input file.\n");
		exit(0);
	}

	changes_file = fopen(argv[3], "r+");
	//changes_file = fopen("changes.txt", "r");

	//입력 파일 존재하지 않는 경우
	if (changes_file == NULL) {
		printf("Error: open input file.\n");
		exit(0);
	}

	output_file = fopen("output_ls.txt", "w");

	init_vector_table();
	func_linkstate();
	print_vector_table();
	
	solve_message();
	fprintf(output_file, "\n");
	messages_file = fopen(argv[2], "r+");

	char line[30];

	while (fgets(line, sizeof(line), changes_file) != NULL) {
		char tmp[30] = "";
		char* ptr;
		int start, end, cost;

		strcpy(tmp, line);

		ptr = strtok(tmp, " ");
		start = atoi(ptr);

		ptr = strtok(NULL, " ");
		end = atoi(ptr);

		ptr = strtok(NULL, " ");
		cost = atoi(ptr);

		solve_changes(start, end, cost);
		func_linkstate();

		print_vector_table();
		solve_message();
		fprintf(output_file, "\n");
		messages_file = fopen(argv[2], "r+");
	}

	printf("Complete. Output file written to output_ls.txt\n");

	fclose(topology_file);
	fclose(messages_file);
	fclose(changes_file);
	fclose(output_file);
}
