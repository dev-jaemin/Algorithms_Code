#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num);

// 주어진 상태 state의 마지막 4비트를 파일에 출력 (화면 출력은 stdout)
void bit_print( FILE *fp, int state);

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
void get_pwgc( int state, int *p, int *w, int *g, int *c);

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph( char *filename, int graph[][16], int num);

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
int is_dead_end( int state);

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
int is_possible_transition( int state1,	int state2);

// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16]);

// 상태 변경: 농부 이동
// return value : 새로운 상태
int changeP( int state);

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태
int changePW( int state);

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태
int changePG( int state);

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태
int changePC( int state);

// 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
int is_visited( int *visited, int level, int state);

// 방문한 상태들을 차례로 화면에 출력
void print_states( int *visited, int count);

// recursive function
void dfs_main( int state, int goal_state, int level, int *visited);

// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int init_state, int goal_state);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int graph[16][16] = {0,};
	
	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// 인접 행렬 출력
	//print_graph( graph, 16);
	
	// .net 파일 만들기 pajek
	save_graph( "pwgc.net", graph, 16);

	//////////////////////////////////////////
	depth_first_search( 0, 15); // initial state, goal state
	
	return 0;
}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num)
{
	for(int i=0;i<num;i++){
		for(int j=0;j<num;j++){
			printf("%d ", graph[i][j]);
		}
		printf("\n");
	}
}

// 주어진 상태 state의 마지막 4비트를 파일에 출력 (화면 출력은 stdout)
void bit_print( FILE *fp, int state)
{
	char* binarystate = (char*)malloc(sizeof(char)*5);
	switch(state){
		case 0: strcpy(binarystate, "0000"); break;
		case 1: strcpy(binarystate, "0001"); break;
		case 2: strcpy(binarystate, "0010"); break;
		case 3: strcpy(binarystate, "0011"); break;
		case 4: strcpy(binarystate, "0100"); break;
		case 5: strcpy(binarystate, "0101"); break;
		case 6: strcpy(binarystate, "0110"); break;
		case 7: strcpy(binarystate, "0111"); break;
		case 8: strcpy(binarystate, "1000"); break;
		case 9: strcpy(binarystate, "1001"); break;
		case 10: strcpy(binarystate, "1010"); break;
		case 11: strcpy(binarystate, "1011"); break;
		case 12: strcpy(binarystate, "1100"); break;
		case 13: strcpy(binarystate, "1101"); break;
		case 14: strcpy(binarystate, "1110"); break;
		default: strcpy(binarystate, "1111"); break;
	}
	fprintf(fp, "%s", binarystate);
	free(binarystate);
}

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
void get_pwgc( int state, int *p, int *w, int *g, int *c)
{
	*p = state & PEASANT ? 1 : 0;
	*w = state & WOLF ? 1 : 0;
	*g = state & GOAT ? 1 : 0;
	*c = state & CABBAGE ? 1 : 0;
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph( char *filename, int graph[][16], int num)
{
	FILE* fp;
	fp = fopen(filename, "wt");
	fprintf(fp, "*Vertices  16\n");

	for(int i=0;i<num;i++){
		fprintf(fp, " %2d \"", i+1);
		bit_print(fp, i);
		fprintf(fp, "\"\n");
	}
	fprintf(fp, "*Edges\n");
	for(int i=0;i<num;i++){
		for(int j=i+1;j<num;j++){
			if(graph[i][j]){
				fprintf(fp, "  %d  %d\n", i+1, j+1);
			}
		}
	}
	fclose(fp);
}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
int is_dead_end( int state)
{
	int p, w, g, c;
	get_pwgc(state, &p ,&w, &g, &c);
	if((w == g && p != g) || (g == c && p != g)) return 1;
	else return 0;
}

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
int is_possible_transition( int state1,	int state2)
{
	if(state2 == changeP(state1)) return 1;
	if(state2 == changePW(state1)) return 1;
	if(state2 == changePG(state1)) return 1;
	if(state2 == changePC(state1)) return 1;

	return 0;
}

// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16])
{
	for(int s1 = 0; s1 < 16; s1++){
		for(int s2 = s1 + 1; s2 < 16; s2++){
			if(is_possible_transition(s1, s2) && !is_dead_end(s2) && !is_dead_end(s1)){
				graph[s1][s2] = 1;
				//graph[s2][s1] = 1;
			}
		}
	}
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
int changeP( int state)
{
	return state ^ PEASANT;
}

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태
int changePW( int state)
{
	return state ^ (PEASANT + WOLF);
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태
int changePG( int state)
{
	return state ^ (PEASANT + GOAT);
}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태
int changePC( int state)
{
	return state ^ (PEASANT + CABBAGE);
}

// 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
int is_visited( int *visited, int level, int state)
{
	if(visited[state] !=0 && visited[state] <= level-1) return 1;
	visited[state] = level+1;
	return 0;
}

// 방문한 상태들을 차례로 화면에 출력
void print_states( int *visited, int count)
{
	int arr[16][16];
	int path = 0;
	int path_n = 1;
	int tmp_path_n;

	arr[0][1] = 0; //initialize

	for(int level = 2;level<=count;level++){
		tmp_path_n = path_n;
		for(path=0;path<tmp_path_n;path++){
			int branch = 0;
			for(int state = 0;state<16;state++){
				if(visited[state] == level && is_possible_transition(arr[path][level-1], state)){
					if(branch){
						for(int i=1;i<level;i++){
							arr[path_n][i] = arr[path][i];
						}
						arr[path_n][level] = state;
						path_n++;
					} else{
						arr[path][level] = state;
						branch++;
					}
				}
				
			}

		}
	}

	for(int i=path_n-1;i>=0;i--){
		printf("Goal-state found!\n");
		for(int j=1;j<=count;j++){
			bit_print(stdout, arr[i][j]);
			printf("\n");
		}
		printf("\n");
	}
	
}

// recursive function
void dfs_main( int state, int goal_state, int level, int *visited)
{
	int curstate = 0;
	
	if(state == goal_state) return;
	for(;curstate <= goal_state;curstate++){
		if(is_possible_transition(state, curstate) && !is_dead_end(state) && !is_dead_end(curstate)){
			if(!is_visited(visited, level, curstate)) dfs_main(curstate, goal_state, level+1, visited);
		}
	}
}

// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int init_state, int goal_state)
{
	int visited[16] = {0,};
	int curstate = 0;

	visited[0] = 1;

	for(;curstate <= goal_state;curstate++){
		if(is_possible_transition(init_state, curstate) && !is_dead_end(init_state) && !is_dead_end(curstate)){
			if(!is_visited(visited, 1, curstate)) dfs_main(curstate, goal_state, 2, visited);
		}
	}

	print_states(visited, visited[goal_state]);
}