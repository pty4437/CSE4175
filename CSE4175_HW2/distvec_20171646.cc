#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <vector>

using namespace std;

typedef struct _NODE {
	//int next_node;
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
			if (i != j) {
				vector_table[i][j].cost = max_num;
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
				if(size == 1)
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

void func_distvec() {
	for (int i = 0; i < numNode; i++) {
		for (int j = 0; j < numNode; j++) {
			//업데이트
			if (vector_table[i][j].cost != max_num) {
				for (int k = 0; k < numNode; k++) {
					if (vector_table[j][k].cost > vector_table[j][i].cost + vector_table[i][k].cost) {
						vector_table[j][k].cost = vector_table[j][i].cost + vector_table[i][k].cost;
						make_road(j, k, i);
						
					}
				}
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
				vector_table[i][j].next_node.push_back(i);
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
				vector_table[i][j].next_node.push_back(j);
			}
		}
	}


	func_distvec();
}

int main(int argc, char* argv[]) {

	//인자의 수가 맞지 않음
	if (argc != 4) {
		printf("usage: distvec topologyfile messagesfile changesfile\n");
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
	output_file = fopen("output_dv.txt", "w");

	init_vector_table();
	func_distvec();

	print_vector_table();

	solve_message();

	fprintf(output_file, "\n");
	messages_file = fopen("messagesfile.txt", "r");

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

		print_vector_table();
		solve_message();
		fprintf(output_file, "\n");
		messages_file = fopen("messagesfile.txt", "r");
	}

	printf("Complete. Output file written to output_dv.txt\n");


	fclose(topology_file);
	fclose(messages_file);
	fclose(changes_file);
	fclose(output_file);
}
