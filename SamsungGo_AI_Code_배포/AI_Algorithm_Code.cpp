// Samsung Go Tournament Form C (g++-4.8.3)

/*
[AI 코드 작성 방법]

1. char info[]의 배열 안에					"TeamName:자신의 팀명,Department:자신의 소속"					순서로 작성합니다.
( 주의 ) Teamname:과 Department:는 꼭 들어가야 합니다.
"자신의 팀명", "자신의 소속"을 수정해야 합니다.

2. 아래의 myturn() 함수 안에 자신만의 AI 코드를 작성합니다.

3. AI 파일을 테스트 하실 때는 "육목 알고리즘대회 툴"을 사용합니다.

4. 육목 알고리즘 대회 툴의 연습하기에서 바둑돌을 누른 후, 자신의 "팀명" 이 들어간 알고리즘을 추가하여 테스트 합니다.



[변수 및 함수]
myturn(int cnt) : 자신의 AI 코드를 작성하는 메인 함수 입니다.
int cnt (myturn()함수의 파라미터) : 돌을 몇 수 둬야하는지 정하는 변수, cnt가 1이면 육목 시작 시  한 번만  두는 상황(한 번), cnt가 2이면 그 이후 돌을 두는 상황(두 번)
int  x[0], y[0] : 자신이 둘 첫 번 째 돌의 x좌표 , y좌표가 저장되어야 합니다.
int  x[1], y[1] : 자신이 둘 두 번 째 돌의 x좌표 , y좌표가 저장되어야 합니다.
void domymove(int x[], int y[], cnt) : 둘 돌들의 좌표를 저장해서 출력


//int board[BOARD_SIZE][BOARD_SIZE]; 바둑판 현재상황 담고 있어 바로사용 가능함. 단, 원본데이터로 수정 절대금지
// 놓을수 없는 위치에 바둑돌을 놓으면 실격패 처리.

boolean ifFree(int x, int y) : 현재 [x,y]좌표에 바둑돌이 있는지 확인하는 함수 (없으면 true, 있으면 false)
int showBoard(int x, int y) : [x, y] 좌표에 무슨 돌이 존재하는지 보여주는 함수 (1 = 자신의 돌, 2 = 상대의 돌, 3 = 블럭킹)


<-------AI를 작성하실 때, 같은 이름의 함수 및 변수 사용을 권장하지 않습니다----->
*/

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <queue>

#include "Connect6Algo.h"

using namespace std;

// "샘플코드[C]"  -> 자신의 팀명 (수정)
// "AI부서[C]"  -> 자신의 소속 (수정)
// 제출시 실행파일은 반드시 팀명으로 제출!
char info[] = {"TeamName:aura,Department:IoT사업화팀]"};

#pragma region 변수초기화

#define mp make_pair

#define boardSize 19
#define numOfDir 8

enum direct { DOWN = 0, RIGHT, UP, LEFT, RIGHTDOWN, RIGHTUP, LEFTDOWN, LEFTUP };

//지금 보드에서 연속된 돌의 방향을 나타내는 enum
//YY : 세로방향, XX : 가로방향, YX : y = x방향, YMinusX : Y = -X방향
enum progressDir {YY = 0, XX, YX, YMinusX};


//정답출력을 위한 정답 좌표.
int ansX[2], ansY[2];


//index가 방향. 0 = 아래, 1 = 오른쪽, 2 = 위, 3 = 왼쪽, 4 = 오른아래, 5 = 오른위, 6 = 왼아래, 7 = 왼위
int dx[] = { 0, 1, 0, -1, 1, 1, -1, -1 };
int dy[] = { 1, 0, -1, 0, 1, -1, 1, -1 };

#pragma endregion


#pragma region myBoard utility




//바둘돌을 두는 것 
//(y, x)
//who -> 1이면 나의 돌, 2이면 상대방의 돌
void setYX(vector<int>& myBoard, int y, int x, int who) {
	int position = y * boardSize + x;
	//cout << position << " "<< (position >> 4);
	myBoard[position >> 4] |= (who << ((position % 16) * 2));
}


//(y, x)자리에 바둑돌을 없애는 것
void unSetYX(vector<int>& myBoard, int y, int x, int who) {
	int position = y * boardSize + x;
	myBoard[position >> 4] &= ~(3 << ((position % 16) * 2));
}

//(y, x)자리에 아무 것도 없는지 확인
bool isEmpty(vector<int>& myBoard, int y, int x) {
	int position = y * boardSize + x;
	return ((myBoard[position >> 4] >> (position % 16) * 2) & 3) == 0;
}

//비트마스크가 잘되었는지 테스트용 출력함수
void printMyBoard(vector<int>& myBoard){

	for (int y = 0; y < boardSize; y++){
		for (int x = 0; x < boardSize; x++){
			int position = y * boardSize + x;
			//0이면 비었다, 1이면 나다, 2이면 적이다, 3이면 블로킹이다
			int who = (myBoard[position >> 4] >> ((position % 16) * 2)) & 3;
			cout << who << " ";
		}
		cout << endl;
	}
}

//board[][]의 상태를 bitmask로 줄이는 것. myturn이 올 때마다 실행해서 계산의 초기화
//int 91개면 board의 상태를 다 표현가능하다
void myBoardInit(vector<int>& myBoard) {

	for (int y = 0; y < boardSize; y++) {
		for (int x = 0; x < boardSize; x++) {
			//0 비어있다, 1 나의 돌, 2 상대방의 돌, 3 블러킹
			setYX(myBoard, y, x, showBoard(x, y));
		}
	}
}

#pragma endregion


#pragma region 재귀를 효율적으로 도와주는 애들

//방향 탐색을 위한 graph
vector<vector<int> > dirAdj;


//방향 탐색을 위한 graph 초기화
void dirAdjInit(){
	//index가 방향. 0 = 아래, 1 = 오른쪽, 2 = 위, 3 = 왼쪽, 4 = 오른아래, 5 = 오른위, 6 = 왼아래, 7 = 왼위
	// (0과 2) / (1과 3) / (4와 7) / (5와 6)
	dirAdj = vector<vector<int> >(numOfDir);

	//자기자신 추가
	for (int dir = 0; dir < numOfDir; dir++){
		dirAdj[dir].push_back(dir);
	}

	//반대방향 추가
	dirAdj[LEFT].push_back(RIGHT);
	dirAdj[RIGHT].push_back(LEFT);

	dirAdj[UP].push_back(DOWN);
	dirAdj[DOWN].push_back(UP);

	dirAdj[LEFTUP].push_back(RIGHTDOWN);
	dirAdj[RIGHTDOWN].push_back(LEFTUP);

	dirAdj[RIGHTUP].push_back(LEFTDOWN);
	dirAdj[LEFTDOWN].push_back(RIGHTUP);
}


//(y, x)가 board 안이면 true, 밖이면 false
bool isIn(int y, int x) {
	return y >= 0 && x >= 0 && y < boardSize && x < boardSize;
}


//우리편 입장에서 참고테이블을 만든다.  search를 빠르게 도와주는 역할임!!   연속된 돌의 숫자의 개수를 기록 dfs를 사용
void DFSCalcMyReferenceTable(vector<vector<int> >& myReferenceBoard, vector<int>& myBoard, bool ***isVisited, int hereY, int hereX, int hereDir){

	


}
//우리편 입장에서 참고테이블을 만든다.  search를 빠르게 도와주는 역할임!!   연속된 돌의 숫자의 개수를 기록 dfs를 사용
void calcMyReferenceTable(vector<vector<int> >& myReferenceBoard, vector<int>& myBoard){

	bool isVisited[boardSize][boardSize][4];
	memset(isVisited, false, sizeof(isVisited));

	for (int y = 0; y < boardSize; y++){
		for (int x = 0; x < boardSize; x++){
			for (int dir = 0; dir < 4; dir++){
				if (!isVisited[y][x][dir])
					DFSCalcMyReferenceTable(myReferenceBoard, myBoard, isVisited, y, x, dir);
			}
		}
	}
}


  
void calcEnermyReferenceTable(vector<vector<int> >& enermyReferenceBoard, vector<int>& myBoard){




}

#pragma endregion






//본격 재귀 함수. 우선 백트래킹으로, 향후 가지치기, 부분 DP 등 확장
//who = 누구의 차례인지 who = 1 나의 차례, who = 2 적의 차례
//depth = 재귀의 깊이 , 홀수이면 나의 차례, 짝수이면 적의 차례
//2개의 수를 놓음
void search(vector<int>& myBoard, int who, int depth) {

	cout << depth << endl;
	int enermy = (who == 1 ? 2 : 1);

	//앞으로 10수까지만 생각한다. 향후 조정. 시간 및 탐색의 범위를 설정하면 더 많은 수를 계산할 수 있으므로
	if (depth >= 3) return;


	//탐색만 함. 상황 판단에 대한 코드를 집어 넣어야함.
	for (int y = 0; y < boardSize; y++){
		for (int x = 0; x < boardSize; x++){
			//비어있으면
			if (isEmpty(myBoard, y, x)){

				setYX(myBoard, y, x, who);
				search(myBoard, enermy, depth + 1);
				unSetYX(myBoard, y, x, who);

			}
		}
	}
}





//myturn에서 불러줄 용도 초반 setting후 재귀 함수 호출 및 부분 dp초기화작업
//최종적으로 ansY[], ansX[]의 값을 넣어주는 역할
//시간에 대한 부분도 추후 넣어야할듯.
void AURAStart(){

	vector<int> myBoard(91, 0);
	myBoardInit(myBoard);


	//탐색을 위한 참고테이블. 연속으로 몇개가 놓여져 있는지를 그 위치에 기록 -> 이 것 우선 순위로 탐색을 위함
	//연속 되었으나 막혀 있으면 -1
	vector<vector<int> > myReferenceTable(boardSize, vector<int>(boardSize, -1));

	//탐색을 위한 참고테이블. 적의 개수. 방어를 위함
	vector<vector<int> > enermyReferenceTable(boardSize, vector<int>(boardSize, -1));



	//search(myBoard, 1, 0);
	

}



void myturn(int cnt) {


	// 이 부분에서 알고리즘 프로그램(AI)을 작성하십시오. 기본 제공된 코드를 수정 또는 삭제하고 본인이 코드를 사용하시면 됩니다.
	// 현재 Sample code의 AI는 Random으로 돌을 놓는 Algorithm이 작성되어 있습니다.

	cout << cnt << endl;

	if (cnt == 1) {
		dirAdjInit();


		ansX[0] = ansY[0] = 9;
		domymove(ansX, ansY, cnt);
		return;
	}

	cout << dirAdj.size() << endl;
	AURAStart();


	/*srand((unsigned)time(NULL));

	for (int i = 0; i < cnt; i++) {
	do {
	x[i] = rand() % width;
	y[i] = rand() % height;
	if (terminateAI) return;
	} while (!isFree(x[i], y[i]));

	if (x[1] == x[0] && y[1] == y[0]) i--;
	}*/


	// 이 부분에서 자신이 놓을 돌을 출력하십시오.
	// 필수 함수 : domymove(x배열,y배열,배열크기)
	// 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.
	//domymove(ansX, ansY, cnt);

}

