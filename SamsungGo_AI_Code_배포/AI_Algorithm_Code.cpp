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
#include <ctime>

#include "Connect6Algo.h"

using namespace std;

// "샘플코드[C]"  -> 자신의 팀명 (수정)
// "AI부서[C]"  -> 자신의 소속 (수정)
// 제출시 실행파일은 반드시 팀명으로 제출!
char info[] = { "TeamName:aura,Department:IoT사업화팀]" };


#pragma region 변수초기화

#define mp make_pair
#define boardSize 19
#define numOfDir 8
#define numOfContinuousDir 4
#define numOfCanWin 6

enum direct { RIGHTDOWN = 0, RIGHTUP, LEFTDOWN, LEFTUP, DOWN, RIGHT, UP, LEFT };

int dy[] = { -1, 1, -1, 1, -1, 0, 1, 0 };
int dx[] = { 1, 1, -1, -1, 0, 1, 0, -1 };
//지금 보드에서 연속된 돌의 방향을 나타내는 enum
//YY : 세로방향, XX : 가로방향, YX : y = x방향, YMinusX : Y = -X방향
enum progressDir { YX = 0, YMinusX, YY, XX };

//돌의 타입 
enum DolType { EMPTY = 0, ME, ENERMY, BLOCKING };




//정답출력을 위한 정답 좌표.
int ansX[2], ansY[2];
vector<int> myBoard(91, 0);




//방향 탐색을 위한 graph
vector<vector<int> > dirAdj;
vector<vector<int> > dirContinueAdj;


//지금 판의 숫자를 계산하여 그 단계를 나눈다. 대략 돌의 개수가 30개정도 남았을 때에는 DP를 사용해 승리를 확정지을 수 있다.
//하지만 전체판의 크기가 361개이므로 후반전으로 가기전에 초중반을 잘 계산해야한다.
int totalMEandENERMY = 0;

//점점 깊어지는 DFS의 
int depthLimit;

//시간 측정을 위한 변수 값들
clock_t beginTime, nowTime;

//GlobalStopBecauseTimeLimit == true이면 모든 탐색이 중지됨.
bool GlobalStopBecauseTimeLimit;

#pragma endregion


#pragma region 시간 out 

//시간을 체크. 타임 아웃 100ms에 GlobalStopBecauseTimeLimit을 true로 만들어 모든 탐색을 중지시킨다.
void checkTime() {
	nowTime = clock();
	GlobalStopBecauseTimeLimit = (nowTime - beginTime) > (limitTime * 1000 - 100);
}

#pragma endregion


#pragma region myBoard utility

//(y, x)가 board 안이면 true, 밖이면 false
bool isIn(int y, int x) {
	return y >= 0 && x >= 0 && y < boardSize && x < boardSize;
}


//바둘돌을 두는 것 
//(y, x)
//who -> 1이면 나의 돌, 2이면 상대방의 돌
void setYX(int y, int x, int who) {
	int position = y * boardSize + x;
	//cout << position << " "<< (position >> 4);
	myBoard[position >> 4] |= (who << ((position % 16) * 2));
}


//(y, x)자리에 바둑돌을 없애는 것
void unSetYX(int y, int x, int who) {
	int position = y * boardSize + x;
	myBoard[position >> 4] &= ~(3 << ((position % 16) * 2));
}

//(y, x)자리에 아무 것도 없는지 확인
bool isEmpty(int y, int x) {
	int position = y * boardSize + x;
	return ((myBoard[position >> 4] >> (position % 16) * 2) & 3) == 0;
}

//(y, x)자리에 who가 있는지 확인
bool isWho(int y, int x, int who) {
	int position = y * boardSize + x;
	return ((myBoard[position >> 4] >> (position % 16) * 2) & 3) == who;
}

//비트마스크가 잘되었는지 테스트용 출력함수
void printMyBoard() {

	for (int y = 0; y < boardSize; y++) {
		for (int x = 0; x < boardSize; x++) {
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
void myBoardInitWhenMyTurnIsCalled() {

	for (int y = 0; y < boardSize; y++) {
		for (int x = 0; x < boardSize; x++) {
			//0 비어있다, 1 나의 돌, 2 상대방의 돌, 3 블러킹
			setYX(y, x, showBoard(x, y));
		}
	}
}



//방향 탐색을 위한 graph 초기화
void dirAdjInit() {
	//index가 방향. 0 = 아래, 1 = 오른쪽, 2 = 위, 3 = 왼쪽, 4 = 오른아래, 5 = 오른위, 6 = 왼아래, 7 = 왼위
	// (0과 2) / (1과 3) / (4와 7) / (5와 6)
	dirAdj = vector<vector<int> >(numOfDir);

	//자기자신 추가
	for (int dir = 0; dir < numOfDir; dir++) {
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

	//세로 가로 y=x y=-x방향을 추가
	dirContinueAdj = vector<vector<int> >(numOfContinuousDir);

	dirContinueAdj[YY].push_back(UP);
	dirContinueAdj[YY].push_back(DOWN);

	dirContinueAdj[XX].push_back(LEFT);
	dirContinueAdj[XX].push_back(RIGHT);

	dirContinueAdj[YX].push_back(RIGHTUP);
	dirContinueAdj[YX].push_back(LEFTDOWN);

	dirContinueAdj[YMinusX].push_back(LEFTUP);
	dirContinueAdj[YMinusX].push_back(RIGHTDOWN);
}

#pragma endregion


#pragma region 누가 이겼는지 판단

int DFSForIsWin(bool(&isVisited)[boardSize][boardSize][numOfContinuousDir], int who, int y, int x, int dir) {
	isVisited[y][x][dir] = true;

	int ret = 1;

	for (int i = 0; i < dirContinueAdj[dir].size(); i++) {
		int nextY = y + dy[dirContinueAdj[dir][i]];
		int nextX = x + dx[dirContinueAdj[dir][i]];

		if (isIn(nextY, nextX) && !isVisited[nextY][nextX][dir] && isWho(nextY, nextX, who)) {
			ret += DFSForIsWin(isVisited, who, nextY, nextX, dir);
		}
	}

	return ret;

}

int getMaximumContinueNum(int who) {

	bool isVisited[boardSize][boardSize][numOfContinuousDir];

	memset(isVisited, false, sizeof(isVisited));

	int ret = 0;

	for (int y = 0; y < boardSize; y++) {
		for (int x = 0; x < boardSize; x++) {
			for (int dir = 0; dir < numOfContinuousDir; dir++) {
				if (isWho(y, x, who) && !isVisited[y][x][dir]) {
					int temp = DFSForIsWin(isVisited, who, y, x, dir);
					ret = max(ret, temp);
				}
			}
		}
	}

	return ret;
}

//기저사례 지금 board의 상태가 who가 이기면 true를, 그렇지 않으면 false를
bool isWin(int who) {
	//6보다 커지면 어떻게 될지 생각해봐야해
	return getMaximumContinueNum(who) == numOfCanWin;
}


#pragma endregion


#pragma region 탐색 순서 조장을 위한 Reference를 만드는 Logic



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//많이 있지만 굳이 할 필요 없는 것도 삭제하게 만들어야한다
////////////////////////////////////////////////////////////////////////////////////////////////////////

//내돌 혹은 적 입장에서 참고테이블을 만든다.  search를 빠르게 도와주는 역할임!!   연속된 돌의 숫자의 개수를 기록, dfs를 사용
int DFSCalcReferenceTable(vector<vector<int> >& ReferenceBoard, bool(&isVisited)[boardSize][boardSize][numOfContinuousDir], int hereY, int hereX, int hereDir, int who, vector<pair<int, int> >& record) {

	isVisited[hereY][hereX][hereDir] = true;
	record.push_back(mp(hereY, hereX));

	int ret = 1;
	bool check = false;
	//hereDir = 0이면 세로 방향, hereDir = 1이면 가로방향, hereDir = 2이면 y = x방향, hereDir = 3이면 y = -x방향
	for (int i = 0; i < dirContinueAdj[hereDir].size(); i++) {
		int nextY = hereY + dy[dirContinueAdj[hereDir][i]];
		int nextX = hereX + dx[dirContinueAdj[hereDir][i]];
		//다음 탐색의 범위가 보드의 범위 안쪽이고, 거기가 내 것이여야만 탐색을 진행한다.
		if (isIn(nextY, nextX) && !isVisited[nextY][nextX][hereDir] && isWho(nextY, nextX, who)) {
			ret += DFSCalcReferenceTable(ReferenceBoard, isVisited, nextY, nextX, hereDir, who, record);
		}

		int nNextY = nextY + dy[dirContinueAdj[hereDir][i]];
		int nNextX = nextX + dx[dirContinueAdj[hereDir][i]];
		int enermy = (who == ME ? ENERMY : ME);
		if (isIn(nNextY, nNextX) && !isVisited[nNextY][nNextX][hereDir] && isWho(nNextY, nNextX, who) &&
			!isWho(nextY, nextX, enermy) && !isWho(nextY, nextX, BLOCKING)) {
			ret += DFSCalcReferenceTable(ReferenceBoard, isVisited, nNextY, nNextX, hereDir, who, record);
		}
	}
	return ret;
}

//우리편 입장에서 참고테이블을 만든다.  search를 빠르게 도와주는 역할임!!   연속된 돌의 숫자의 개수를 기록, dfs를 사용
//돌의 가중치를 만들어 이 것부터 탐색하게 만든다.
void calcMyReferenceTable(vector<vector<int> >& myReferenceBoard) {
	bool isVisited[boardSize][boardSize][numOfContinuousDir];
	memset(isVisited, false, sizeof(isVisited));
	for (int y = 0; y < boardSize; y++) {
		for (int x = 0; x < boardSize; x++) {
			for (int dir = 0; dir < numOfContinuousDir; dir++) {
				//아직 방문하지 않았고 그 위치가 지금 비어있으면 들어간다.
				if (!isVisited[y][x][dir] && isFree(x, y)) {
					//연속된 갯수가 numOfContinue에, 그 위치가 record에 반환
					vector<pair<int, int> > record;
					int numOfContinue = DFSCalcReferenceTable(myReferenceBoard, isVisited, y, x, dir, ME, record);
					for (int i = 0; i < record.size(); i++) {
						myReferenceBoard[record[i].first][record[i].second] = numOfContinue;
					}
				}
			}
		}
	}
}

//적 입장에서 참고 테이블을 만든다. search를 빠르게 도와주는 역할! 연속된 돌의 숫자의 개수를 기록, dfs를 사용!
//돌의 가중치를 만들어 이 것 부터 탐색하게 만든다.
void calcEnermyReferenceTable(vector<vector<int> >& enermyReferenceBoard) {
	bool isVisited[boardSize][boardSize][numOfContinuousDir];
	memset(isVisited, false, sizeof(isVisited));
	for (int y = 0; y < boardSize; y++) {
		for (int x = 0; x < boardSize; x++) {
			for (int dir = 0; dir < numOfContinuousDir; dir++) {
				//아직 방문하지 않았고 그 위치가 지금 비어있으면 들어간다.
				if (!isVisited[y][x][dir] && isFree(x, y)) {
					//연속된 갯수가 numOfContinue에, 그 위치가 record에 반환
					vector<pair<int, int> > record;
					int numOfContinue = DFSCalcReferenceTable(enermyReferenceBoard, isVisited, y, x, dir, ENERMY, record);
					for (int i = 0; i < record.size(); i++) {
						enermyReferenceBoard[record[i].first][record[i].second] = numOfContinue;
					}
				}
			}
		}
	}
}


//지금 놓은 Board에서 연속된 돌 주변부터 range만큼 떨어진 좌표를 순서대로 반환
vector<pair<int, int> > makeReference(int range) {

	//탐색을 위한 참고테이블. 연속으로 몇개가 놓여져 있는지를 그 위치에 기록 -> 이 것 우선 순위로 탐색을 위함
	//연속 되었으나 막혀 있으면 -1
	vector<vector<int> > myReferenceTable(boardSize, vector<int>(boardSize, -1));
	calcMyReferenceTable(myReferenceTable);

	//탐색을 위한 참고테이블. 적의 개수. 방어를 위함
	vector<vector<int> > enermyReferenceTable(boardSize, vector<int>(boardSize, -1));
	calcEnermyReferenceTable(enermyReferenceTable);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ReferenceTable 만든 것들 test해야함!
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<pair<int, pair<int, int> > > sumRefTable;

	//myReferenceTable 과 enermyReferenceTable에서 만든 가중치를 sorting하여 search에 참조하게 만든다.
	//-1인 것을 굳이 넣을 필요는 있을까? 생각해보아야 할 문제
	for (int y = 0; y < boardSize; y++) {
		for (int x = 0; x < boardSize; x++) {
			if (myReferenceTable[y][x] != -1) {
				sumRefTable.push_back(mp(myReferenceTable[y][x], mp(y, x)));
			}
			if (enermyReferenceTable[y][x] != -1) {
				sumRefTable.push_back(mp(enermyReferenceTable[y][x], mp(y, x)));
			}
		}
	}

	//sort(myReference.begin(), myReference.end()); sort(enermyReference.begin(), enermyReference.end());
	sort(sumRefTable.begin(), sumRefTable.end());

	queue<pair<int, int> > surroundCoordianteQueue;

	vector<vector<int> > dist = vector<vector<int> >(boardSize, vector<int>(boardSize, -1));

	vector<pair<int, int> > refCoordinate;
	for (int i = 0; i < sumRefTable.size(); i++) {
		//refCoordinate.push_back(sumRefTable[i].second);
		surroundCoordianteQueue.push(sumRefTable[i].second);
		dist[sumRefTable[i].second.first][sumRefTable[i].second.second] = 0;
	}
	
	while (!surroundCoordianteQueue.empty()) {

		int hereY = surroundCoordianteQueue.front().first;
		int hereX = surroundCoordianteQueue.front().second;

		int hereDist = dist[hereY][hereX];

		surroundCoordianteQueue.pop();

		//여기까지 탐색
		if (hereDist > range) break;

		for (int i = 0; i < numOfDir; i++) {
			int nextY = hereY + dy[i]; int nextX = hereX + dx[i];
			if (isIn(nextY, nextX) && dist[nextY][nextX] != -1) {
				surroundCoordianteQueue.push(mp(nextY, nextX));
			}
		}
	}

	return refCoordinate;
}

inline int getRange() {
	return 2;
}

#pragma endregion


#pragma region 본격 재귀 -> 점점 깊어지는 DFS로 구현. 필수적인 정보들을 위에 선언.

////DFS를 통해 먼저 탐색할 만한 것들을 순서대로 나열해봄
//vector<pair<int, int> > refCoordinate;

//본격 재귀 함수. 우선 백트래킹으로, 향후 가지치기, 부분 DP 등 확장
//who = 누구의 차례인지 who = 1 나의 차례, who = 2 적의 차례
//depth = 재귀의 깊이, 홀수이면 나의 차례, 짝수이면 적의 차례
//2개의 수를 놓음
double search(int who, int depth) {

	cout << depth << endl;

	checkTime();

	//제출할 때가 되었으면 지금까지 탐색한 것 결과 값 어떻게 유지해서 올려줄 수 있는지 생각해보자.
	if (GlobalStopBecauseTimeLimit) return -1.0;

	if (depth > depthLimit) return -1.0;

	int opposite = (who == ME ? ENERMY : ME);

	//적이 이겼나 내가 이겼나 판단 -> 탐색을 중지 가장 위급한 상황


	double ret = 1.0;









	//refTable을 통해 탐색의 범위 세팅
	vector<pair<int, int> > searchCoordinates = makeReference(getRange());
	vector<pair<int, int> > ansCoordinates = vector<pair<int, int> >(2);
	for (int i = 0; i < searchCoordinates.size() - 1; i++) {
		for (int j = i + 1; searchCoordinates.size(); j++) {
			
			int y1 = searchCoordinates[i].first, x1 = searchCoordinates[i].second;
			int y2 = searchCoordinates[j].first, x2 = searchCoordinates[j].second;


			if (isEmpty(y1, x1) && isEmpty(y2, x2)) {
				//2개를 둔다
				setYX(y1, x1, who); setYX(y2, x2, who); totalMEandENERMY += 2;

				//적에게 턴을 넘긴다.
				double temp = search(opposite, depth + 1);

				//값이 max일 때 (y1, x1) , (y2, x2)를 기록한다. -> 내 부모로 리턴
				if (temp > ret) {
					ret = temp;
					ansCoordinates[0] = searchCoordinates[i], ansCoordinates[1] = searchCoordinates[j];
				}
				//다음 탐색을 위해 놓았던 것을 없앤다
				unSetYX(y1, x1, who); unSetYX(y2, x2, who); totalMEandENERMY -= 2;
			}
		}
	}

	if (depth == 1) {
		ansY[0] = ansCoordinates[0].first, ansX[0] = ansCoordinates[0].second;
		ansY[1] = ansCoordinates[1].first, ansX[1] = ansCoordinates[1].second;
	}
	return ret * 0.9;
}



#pragma endregion



//myturn에서 불러줄 용도 초반 setting후 재귀 함수 호출 및 부분 dp초기화작업
//최종적으로 ansY[], ansX[]의 값을 넣어주는 역할
//시간에 대한 부분도 추후 넣어야할듯.
void AURAStart() {

	myBoardInitWhenMyTurnIsCalled();
	


	//search의 depth를 조정하며 점점 깊어지는 DFS를 구현
	//depth를 2씩 증가시키며 탐색
	//시간을 재고 시간에 따라 return 하면 됨.
	depthLimit = 3;
	/*while(1) {
		search(ENERMY, 1);
		depthLimit += 2;
	}*/

}


void myturn(int cnt) {
	beginTime = clock();

	//전체 돌의 개수를 저장해놓음.
	totalMEandENERMY += cnt;
	GlobalStopBecauseTimeLimit = false;

	// 이 부분에서 알고리즘 프로그램(AI)을 작성하십시오. 기본 제공된 코드를 수정 또는 삭제하고 본인이 코드를 사용하시면 됩니다.
	// 현재 Sample code의 AI는 Random으로 돌을 놓는 Algorithm이 작성되어 있습니다.

	if (cnt == 1) {
		dirAdjInit();
		//가운데 벽이 있을 경우 2칸띄어서 부터 넣음.
		if(isFree(9,9))
			ansX[0] = ansY[0] = 9;
		else {
			for (int j = 2; j < 10; j++) {
				for (int i = 0; i < numOfDir; i++) {
					int XXX = 9 + j * dx[i], YYY = 9 + j * dy[i];
					if (isFree(XXX, YYY)) {
						ansX[0] = XXX, ansY[0] = YYY;
						domymove(ansX, ansY, cnt);
						return;
					}
				}
			}
		}
	}

	AURAStart();


	// 이 부분에서 자신이 놓을 돌을 출력하십시오.
	// 필수 함수 : domymove(x배열,y배열,배열크기)
	// 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.

	//domymove(ansX, ansY, cnt);

}
