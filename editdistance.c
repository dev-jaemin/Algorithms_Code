#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INSERT_OP      0x01
#define DELETE_OP      0x02
#define SUBSTITUTE_OP  0x04
#define MATCH_OP       0x08
#define TRANSPOSE_OP   0x10

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1

int rec_index = 1;

// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태에 대한 연산자 정보(backtrace)가 저장된 행렬 (1차원 배열임에 주의!)
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba"
void backtrace_main( int *op_matrix, int col_size, char *str1, char *str2, int n, int m, int level, char align_str[][8]);

// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// 각 연산자를  괄호 안의 기호로 표시한다. 삽입(I), 삭제(D), 교체(S), 일치(M), 전위(T)
void print_matrix( int *op_matrix, int col_size, int n, int m);

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance( char *str1, char *str2);

////////////////////////////////////////////////////////////////////////////////
// 세 정수 중에서 가장 작은 값을 리턴한다.
int __GetMin3( int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if(c < min)
		min = c;
	return min;
}

////////////////////////////////////////////////////////////////////////////////
// 네 정수 중에서 가장 작은 값을 리턴한다.
int __GetMin4( int a, int b, int c, int d)
{
	int min = __GetMin3( a, b, c);
	return (min > d) ? d : min;
}

////////////////////////////////////////////////////////////////////////////////
// 정렬된 문자쌍들을 출력 
void print_alignment( char align_str[][8], int level)
{
	int i;
	
	for (i = level; i >= 0; i--)
	{
		printf( "%s\n", align_str[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// backtrace_main을 호출하는 wrapper 함수
void backtrace( int *op_matrix, int col_size, char *str1, char *str2, int n, int m)
{
	char align_str[n+m][8]; // n+m strings
	
	backtrace_main( op_matrix, col_size, str1, str2, n, m, 0, align_str);
}

////////////////////////////////////////////////////////////////////////////////
int main()
{
	char str1[30];
	char str2[30];
	
	int distance;
	
	printf( "INSERT_COST = %d\n", INSERT_COST);
	printf( "DELETE_COST = %d\n", DELETE_COST);
	printf( "SUBSTITUTE_COST = %d\n", SUBSTITUTE_COST);
	printf( "TRANSPOSE_COST = %d\n", TRANSPOSE_COST);
	
	while( fscanf( stdin, "%s\t%s", str1, str2) != EOF)
	{
		printf( "\n==============================\n");
		printf( "%s vs. %s\n", str1, str2);
		printf( "==============================\n");
		
		distance = min_editdistance( str1, str2);
		
		printf( "\nMinEdit(%s, %s) = %d\n", str1, str2, distance);
	}
	return 0;
}

// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태에 대한 연산자 정보(backtrace)가 저장된 행렬 (1차원 배열임에 주의!)
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba"
void backtrace_main( int *op_matrix, int col_size, char *str1, char *str2, int n, int m, int level, char align_str[][8])
{
	if(level == -1){
	if(op_matrix[m*col_size + n] != 0) return;
	if(n == 0){op_matrix[m*col_size + n] = m * 32 + INSERT_OP; return;}
	else if(m == 0){op_matrix[m*col_size + n] = n * 32 + DELETE_OP; return;}

	int cost;
	if(str1[n-1] == str2[m-1]) cost = 0;
	else cost = SUBSTITUTE_COST;

	backtrace_main(op_matrix, col_size, str1, str2, n, m-1, -1, align_str);
	backtrace_main(op_matrix, col_size, str1, str2, n-1, m, -1, align_str);

	int left = op_matrix[(m-1)*col_size + n] / 32;
	int bottom = op_matrix[m*col_size + n - 1] / 32;
	int left_bottom = op_matrix[(m-1)*col_size + n - 1] / 32;
	int left2_bottom2 = 100000000; // 첫 철자만 보고 transpose 하지 않게끔 큰 숫자로 설정
	if(n>=2 && m>=2) left2_bottom2 = op_matrix[(m-2)*col_size + n - 2] / 32;

	int value = 0;
	int op_sum = 0;
	int transpose = 0;

	value = __GetMin3(left+INSERT_COST, bottom+DELETE_COST, left_bottom+cost);
	
	//Transpose
	if(n>1 && m>1 && str1[n-1] == str2[m-2] && str1[n-2] == str2[m-1]){
		if(value >= left2_bottom2 + TRANSPOSE_COST){
			value = (left2_bottom2 + TRANSPOSE_COST);
			transpose = 1;
		}
	}

	if(value == left+INSERT_COST) op_sum += INSERT_OP;
	if(value == bottom+DELETE_COST) op_sum += DELETE_OP;
	if(value == left_bottom+SUBSTITUTE_COST) op_sum += SUBSTITUTE_OP;
	if(value == left_bottom && cost == 0) op_sum += MATCH_OP;
	if(value == left2_bottom2+TRANSPOSE_COST && transpose) op_sum += TRANSPOSE_OP;

	op_matrix[m*col_size + n] = value*32 + op_sum;
	}

	else if(m==0 && n==0 && level!=-1){
		printf("[%d]===============================\n", rec_index);
		rec_index++;
		for(int k=level-1;k>=0;k--) printf("%s\n", align_str[k]);
	}
	else{
		int op = op_matrix[m*col_size + n] % 32;

		if((op>>2)%2 || (op>>3)%2){
			
			align_str[level][0] = str1[n-1];
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = str2[m-1];
			align_str[level][5] = '\0';
			if((op>>2)%2) backtrace_main(op_matrix, col_size, str1, str2, n-1, m-1, level+1, align_str);
			if((op>>3)%2) backtrace_main(op_matrix, col_size, str1, str2, n-1, m-1, level+1, align_str);
		}

		if(op%2){
			align_str[level][0] ='*';
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = str2[m-1];
			align_str[level][5] = '\0';
			backtrace_main(op_matrix, col_size, str1, str2, n, m-1, level+1, align_str);
		}

		if((op>>1)%2){
			
			align_str[level][0] = str1[n-1];
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = '*';
			align_str[level][5] = '\0';
			backtrace_main(op_matrix, col_size, str1, str2, n-1, m, level+1, align_str);
		}
		
		if((op>>4)%2){
			
			align_str[level][0] = str1[n-2];
			align_str[level][1] = str1[n-1];
			align_str[level][2] = ' ';
			align_str[level][3] = '-';
			align_str[level][4] = ' ';
			align_str[level][5] = str2[n-2];
			align_str[level][6] = str2[n-1];
			align_str[level][7] = '\0';
			backtrace_main(op_matrix, col_size, str1, str2, n-2, m-2, level+1, align_str);
		}
	}
}

// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// 각 연산자를  괄호 안의 기호로 표시한다. 삽입(I), 삭제(D), 교체(S), 일치(M), 전위(T)
void print_matrix( int *op_matrix, int col_size, int n, int m)
{
	for(int i=n;i>=1;i--){
		for(int j=1;j<=m;j++){
			int op = op_matrix[j*col_size + i] % 32;
			if(op % 2) printf("I");
			if((op>>1)%2) printf("D");
			if((op>>2)%2) printf("S");
			if((op>>3)%2) printf("M");
			if((op>>4)%2) printf("T");
			printf("\t");
		}
		printf("\n");
	}
}

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance( char *str1, char *str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int op_matrix[(len1+1)*(len2+1)];
	char tmp[30][8];
	memset(op_matrix, 0, sizeof(op_matrix));

	backtrace_main(op_matrix, len1+1, str1, str2, len1, len2, -1, tmp);
	print_matrix(op_matrix, len1+1, len1, len2);
	backtrace(op_matrix, len1+1, str1, str2, len1, len2);

	rec_index = 1;
	
	return op_matrix[(len1+1)*(len2+1)-1] / 32;
}
