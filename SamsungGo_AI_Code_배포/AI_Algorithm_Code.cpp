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
#define _CRT_SECURE_NO_WARNINGS
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
char info[] = { "TeamName:아우라,Department:IoT사업화팀" };

typedef long long int ll;


#pragma region 변수초기화

#define mp make_pair
#define boardSize 19
#define numOfDir 8
#define numOfContinuousDir 4
#define numOfCanWin 6
#define numOfRange 1

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
vector<int> myBoard(200, 0);
int setNum;

//내가 둔 것, 상대방이 둔 것 모두 기록을 한다.
//first => 누구, second.first => y좌표, second.second => x좌표
vector < pair<int, pair<int, int> > > recordOfSet;

//각 상황에서 방어해야하는 유력한 후보의 점수를 계산하기 위한 좌표
vector<pair<int, int> > cand;


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


//점수 계산을 위한 점수들 각각 1칸, 2칸, 3칸, 4칸, 5칸, 빈칸이 있을 때의 곱해지는 점수들이다.
int ScoreValues[7] = { 0, 20, 14, 10, 7, 4, 2};


#pragma endregion



#pragma region time out check

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

//(y, x)자리에 누가 있는지 확인 DolType을 반환
int isWho(int y, int x) {
	int position = y * boardSize + x;
	return ((myBoard[position >> 4] >> (position % 16) * 2) & 3);
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
			if (showBoard(x, y) != isWho(y, x)) {
				setYX(y, x, ENERMY);
				recordOfSet.push_back(mp(ENERMY, mp(y, x)));
			}
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


#pragma region 탐색 순서 조작을 위한 Reference를 만드는 Logic




//지금 놓은 Board에서 연속된 돌 주변부터 range만큼 떨어진 좌표를 순서대로 반환
vector<pair<int, int> > makeReference() {

	vector<pair<int, int> > ret;

	vector<pair<int, int> > existCoor;
	queue<pair<int, int> > SurroundingQueue;

	vector<vector<int> > dist = vector<vector<int> >(boardSize, vector<int>(boardSize, -1));

	for (int y = 0; y < boardSize; y++) {
		for (int x = 0; x < boardSize; x++) {
			if (isWho(y, x, ME) || isWho(y, x, ENERMY)) {
				SurroundingQueue.push(mp(y, x));
				dist[y][x] = 0;
			}
		}
	}

	while (!SurroundingQueue.empty()) {
		
		int hereY = SurroundingQueue.front().first, hereX = SurroundingQueue.front().second;
		
		int hereDist = dist[hereY][hereX];
		SurroundingQueue.pop();
		
		if (hereDist > numOfRange) break;
		for (int i = 0; i < numOfDir; i++) {
			int nextY = hereY + dy[i];
			int nextX = hereX + dx[i];
			if (isIn(nextY, nextX) && isWho(nextY, nextX, EMPTY) &&dist[nextY][nextX] == -1) {
				SurroundingQueue.push(mp(nextY, nextX));
				dist[nextY][nextX] = hereDist + 1;
				ret.push_back(mp(nextY, nextX));
			}
		}
	}
	return ret;
}


#pragma endregion



#pragma region 가지치기를 위한 함수들 1 . 내가 공격하는 경우, 2. 내가 방어해야하는 경우, 3. 주변부돌들의 점수를 계산
//who 입장에서 무조건 공격하면 이기는 경우를 살피고 그 좌표를 return
vector<pair<int, int> > shouldCheckAttack(int turn) {

	int pre_x[2], pre_y[2];
	vector<pair<int, int> > ret;
	if (recordOfSet.size() < 2) return ret;

	pre_x[0] = recordOfSet[recordOfSet.size() - 1].second.second, pre_y[0] = recordOfSet[recordOfSet.size() - 1].second.first;
	pre_x[1] = recordOfSet[recordOfSet.size() - 2].second.second, pre_y[1] = recordOfSet[recordOfSet.size() - 2].second.first;

	int x, y;
	// 가로 방향
	for (int i = 0; i < 2; i++) { // 돌 2개 중 한개 체크
		for (int j = 0; j < 6; j++) {   // 슬라이딩 윈도우 위치
			x = pre_x[i] - 5 + j;
			y = pre_y[i];
			if ((x >= 0) && (x + 5 < boardSize)) {
				int cnt = 0;
				int k;
				for (k = 0; k < 6; k++) {   // 슬라이딩 윈도우 내에서 체크
					if (isWho(y, x + k) == turn) {
						cnt++;
					}
					else if (isWho(y, x + k) == EMPTY) {
						continue;
					}
					else {
						break;
					}
				}
				if ((cnt >= 4) && k == 6) {
					if (x - 1 >= 0) {   // 양쪽에 내 돌이 있는지 체크
						if (isWho(y, x - 1, turn)) {
							break;
						}
					}
					if (x + 6 < boardSize) {
						if (isWho(y, x + 6, turn)) {
							break;
						}
					}
					for (int k = 0; k < 6; k++) {   // 빈 곳에 돌 두기
						if (isWho(y, x + k, EMPTY)) {
							ret.push_back(mp(y, x + k));
						}
					}
					return ret;
				}
			}
		}
	}

	// 세로 방향
	for (int i = 0; i < 2; i++) { // 돌 2개 중 한개 체크
		for (int j = 0; j < 6; j++) {   // 슬라이딩 윈도우 위치
			x = pre_x[i];
			y = pre_y[i] - 5 + j;
			if ((y >= 0) && (y + 5 < boardSize)) {
				int cnt = 0;
				int k;
				for (k = 0; k < 6; k++) {   // 슬라이딩 윈도우 내에서 체크
					if (isWho(y + k, x) == turn) {
						cnt++;
					}
					else if (isWho(y + k, x) == EMPTY) {
						continue;
					}
					else {
						break;
					}
				}
				if ((cnt >= 4) && (k == 6)) {
					if (y - 1 >= 0) {   // 양쪽에 내 돌이 있는지 체크
						if (isWho(y - 1, x, turn)) {
							break;
						}
					}
					if (y + 6 < boardSize) {
						if (isWho(y + 6, x, turn)) {
							break;
						}
					}
					for (int k = 0; k < 6; k++) {   // 빈 곳에 돌 두기
						if (isWho(y + k, x, EMPTY)) {
							ret.push_back(mp(y + k, x));
						}
					}
					return ret;
				}
			}
		}
	}
	// 오른아래쪽 방향
	for (int i = 0; i < 2; i++) { // 돌 2개 중 한개 체크
		for (int j = 0; j < 6; j++) {   // 슬라이딩 윈도우 위치
			x = pre_x[i] - 5 + j;
			y = pre_y[i] - 5 + j;
			if (((x >= 0) && (x + 5 < boardSize)) && ((y >= 0) && (y + 5 < boardSize))) {
				int cnt = 0;
				int k;
				for (k = 0; k < 6; k++) {   // 슬라이딩 윈도우 내에서 체크
					if (isWho(y + k, x + k) == turn) {
						cnt++;
					}
					else if (isWho(y + k, x + k) == EMPTY) {
						continue;
					}
					else {
						break;
					}
				}
				if ((cnt >= 4) && k == 6) {
					if ((x - 1 >= 0) && (y - 1 >= 0)) {   // 양쪽에 내 돌이 있는지 체크
						if (isWho(y - 1, x - 1, turn)) {
							break;
						}
					}
					if ((x + 6 < boardSize) && (y + 6 < boardSize)) {
						if (isWho(y + 6, x + 6, turn)) {
							break;
						}
					}
					for (int k = 0; k < 6; k++) {   // 빈 곳에 돌 두기
						if (isWho(y + k, x + k, EMPTY)) {
							ret.push_back(mp(y + k, x + k));
						}
					}
					return ret;
				}
			}
		}
	}
	// 왼아래쪽 방향
	for (int i = 0; i < 2; i++) { // 돌 2개 중 한개 체크
		for (int j = 0; j < 6; j++) {   // 슬라이딩 윈도우 위치
			x = pre_x[i] + 5 - j;
			y = pre_y[i] - 5 + j;
			if (((x < boardSize) && (x - 5 >= 0)) && ((y >= 0) && (y + 5 < boardSize))) {
				int cnt = 0;
				int k;
				for (k = 0; k < 6; k++) {   // 슬라이딩 윈도우 내에서 체크
					if (isWho(y + k, x - k) == turn) {
						cnt++;
					}
					else if (isWho(y + k, x - k) == EMPTY) {
						continue;
					}
					else {
						break;
					}
				}
				if ((cnt >= 4) && k == 6) {
					if ((x + 1 < boardSize) && (y - 1 >= 0)) {   // 양쪽에 내 돌이 있는지 체크
						if (isWho(y - 1, x + 1, turn)) {
							break;
						}
					}
					if ((x - 6 >= 0) && (y + 6 < boardSize)) {
						if (isWho(y + 6, x - 6, turn)) {
							break;
						}
					}
					for (int k = 0; k < 6; k++) {   // 빈 곳에 돌 두기
						if (isWho(y + k, x - k, EMPTY)) {
							ret.push_back(mp(y + k, x - k));

						}
					}
					return ret;
				}
			}
		}
	}
	return ret;
}



//who 입장에서 무조건 방어해야하는 경우를 살피고 그 좌표를 return.
vector<pair<int, int> > shouldCheckDefence(int who) {
	vector<pair<int, int> > ret;


	int opposite = (who == ME ? ENERMY : ME);

	//지금 기록한게 2개 이하면 볼 것도 없이 리턴
	if (recordOfSet.size() < 2) return ret;

	vector<pair<int, int> > checkCoordinates(2);

	checkCoordinates[0] = recordOfSet[recordOfSet.size() - 2].second;
	checkCoordinates[1] = recordOfSet[recordOfSet.size() - 1].second;


	//가로방향
	for (int i = 0; i < checkCoordinates.size(); i++) {

		int checkY = checkCoordinates[i].first, checkX = checkCoordinates[i].second;
		
		vector<bool> ToRightMarked(boardSize, false);
		vector<pair<int, int> > ToRightMarkedCoor;
		//오른쪽방향
		for (int left = checkX - 5; left <= checkX; left++) {
			int right = left + 5;
			//left나 right중 하나라도 범위 밖이면 탐색하지 않는다.
			if (!isIn(checkY, left) || !isIn(checkY, right)) continue;
			
			int BlockTypeCnt[4] = { 0, 0, 0, 0 };

			int maxEmptyX = -1; bool isMarkExist = false;
			for (int x = left; x <= right; x++) {
				if (isWho(checkY, x, EMPTY)) maxEmptyX = max(maxEmptyX, x);
				BlockTypeCnt[isWho(checkY, x)]++;
				if (ToRightMarked[x]) isMarkExist = true;
			}
			if (BlockTypeCnt[who] >= 4 && BlockTypeCnt[opposite] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isMarkExist
				  && (!isIn(checkY, left - 1) || (isIn(checkY, left - 1) && !isWho(checkY, left - 1, opposite)))
	    		    && (!isIn(checkY, right + 1) ||(isIn(checkY, right + 1) && !isWho(checkY, right + 1, opposite)))) {
				ToRightMarked[maxEmptyX] = true;
				ToRightMarkedCoor.push_back(mp(checkY, maxEmptyX));
			}
		}

		vector<bool> ToLeftMarked(boardSize, false);
		vector<pair<int, int> > ToLeftMarkedCoor;

		//왼쪽방향
		for (int left = checkX; left >= checkX - 5; left--) {
			int right = left + 5;
			//left나 right중 하나라도 범위 밖이면 탐색하지 않는다.
			if (!isIn(checkY, left) || !isIn(checkY, right)) continue;

			int BlockTypeCnt[4] = { 0, 0, 0, 0 };
			int minEmptyX = 30; bool isMarkExist = false;
			for (int x = left; x <= right; x++) {
				if (isWho(checkY, x, EMPTY)) minEmptyX = min(minEmptyX, x);
				BlockTypeCnt[isWho(checkY, x)]++;
				if (ToLeftMarked[x]) isMarkExist = true;
			}

			if (BlockTypeCnt[who] >= 4 && BlockTypeCnt[opposite] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isMarkExist
				&& (!isIn(checkY, left - 1) ||(isIn(checkY, left - 1) && !isWho(checkY, left - 1, opposite)))
				&& (!isIn(checkY, right + 1) || (isIn(checkY, right + 1) && !isWho(checkY, right + 1, opposite)))) {
				ToLeftMarked[minEmptyX] = true;
				ToLeftMarkedCoor.push_back(mp(checkY,minEmptyX));
			}
		}

		sort(ToRightMarkedCoor.begin(), ToRightMarkedCoor.end());
		sort(ToLeftMarkedCoor.begin(), ToLeftMarkedCoor.end());


		if (ToRightMarkedCoor.size() == 1) {
			if (ToRightMarkedCoor[0] == ToLeftMarkedCoor[0]) {
				ret.push_back(ToRightMarkedCoor[0]);
				if (ret.size() == 2) return ret;
			}
			else {
				ret.push_back(ToRightMarkedCoor[0]);
				cand.push_back(ToLeftMarkedCoor[0]);
				if (ret.size() == 2) return ret;
			}
		}
		else if (ToRightMarkedCoor.size() == 2) {
			if (ToRightMarkedCoor == ToLeftMarkedCoor) {
				return ToRightMarkedCoor;
			}
			else {
				ret.push_back(ToRightMarkedCoor[0]); 
				ret.push_back(ToLeftMarkedCoor[0]);
				return ret;
			}
		}
	}

	//세로방향
	for (int i = 0; i < checkCoordinates.size(); i++) {

		int checkY = checkCoordinates[i].first, checkX = checkCoordinates[i].second;

		vector<bool> ToDownMarked(boardSize, false);
		vector<pair<int, int> > ToDownMarkedCoor;

		//아래방향
		for (int up = checkY - 5; up <= checkY; up++) {
			int down = up + 5;
			//up나 down중 하나라도 범위 밖이면 탐색하지 않는다.
			if (!isIn(up, checkX) || !isIn(down, checkX)) continue;

			int BlockTypeCnt[4] = { 0, 0, 0, 0 };

			int maxEmptyY = -1; bool isMarkExist = false;
			for (int y = up; y <= down; y++) {
				if (isWho(y, checkX, EMPTY)) maxEmptyY = max(maxEmptyY, y);
				BlockTypeCnt[isWho(y, checkX)]++;
				if (ToDownMarked[y]) isMarkExist = true;
			}
			if (BlockTypeCnt[who] >= 4 && BlockTypeCnt[opposite] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isMarkExist
				&& ((!isIn(up - 1, checkX)) || (isIn(up -1, checkX) && !isWho(up - 1, checkX, opposite)))
				&& (!isIn(down + 1, checkX) || (isIn(down + 1, checkX) && !isWho(down + 1, checkX, opposite)))) {
				ToDownMarked[maxEmptyY] = true;
				ToDownMarkedCoor.push_back(mp(maxEmptyY, checkX));
			}
		}

		vector<bool> ToUpMarked(boardSize, false);
		vector<pair<int, int> > ToUpMarkedCoor;

		//위쪽 방향
		for (int up = checkY; up >= checkY - 5; up--) {
			int down = up + 5;
			//left나 right중 하나라도 범위 밖이면 탐색하지 않는다.
			if (!isIn(checkY, up) || !isIn(checkY, down)) continue;

			int BlockTypeCnt[4] = { 0, 0, 0, 0 };
			int minEmptyY = 30; bool isMarkExist = false;
			for (int y = up; y <= down; y++) {
				if (isWho(y, checkX, EMPTY)) minEmptyY = min(minEmptyY, y);
				BlockTypeCnt[isWho(y, checkX)]++;
				if (ToUpMarked[y]) isMarkExist = true;
			}

			if (BlockTypeCnt[who] >= 4 && BlockTypeCnt[opposite] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isMarkExist
				&& (!isIn( up- 1, checkX) || (isIn(up - 1,checkX) && !isWho( up- 1, checkX, opposite)))
				&& (!isIn( down + 1, checkX) || (isIn(down + 1, checkX) && !isWho(down + 1, checkX, opposite)))) {
				ToUpMarked[minEmptyY] = true;
				ToUpMarkedCoor.push_back(mp(minEmptyY, checkX));
			}
		}

		sort(ToDownMarkedCoor.begin(), ToDownMarkedCoor.end());
		sort(ToUpMarkedCoor.begin(), ToUpMarkedCoor.end());


		if (ToUpMarkedCoor.size() == 1) {
			if (ToDownMarkedCoor[0] == ToUpMarkedCoor[0]) {
				ret.push_back(ToDownMarkedCoor[0]);
				if (ret.size() == 2) return ret;
			}
			else {
				ret.push_back(ToDownMarkedCoor[0]);
				cand.push_back(ToUpMarkedCoor[0]);
				if (ret.size() == 2) return ret;
			}
		}
		else if (ToUpMarkedCoor.size() == 2) {
			if (ToDownMarkedCoor == ToUpMarkedCoor) {
				return ToDownMarkedCoor;
			}
			else {
				ret.push_back(ToDownMarkedCoor[0]);
				ret.push_back(ToUpMarkedCoor[0]);
				return ret;
			}
		}
	}
	
	//오른쪽아래 대각선방향
	for (int i = 0; i < checkCoordinates.size(); i++) {
		int checkY = checkCoordinates[i].first, checkX = checkCoordinates[i].second;

		vector<bool> ToDownMarked(boardSize, false);
		vector<pair<int, int> > ToDownMarkedCoor;

		//아래방향
		for (int prev = -5; prev <= 0;  prev++) {
			int next = prev + 5;

			if (!isIn(checkY + prev, checkX + prev) || !isIn(checkY + next, checkX + next)) continue;

			int BlockTypeCnt[4] = { 0, 0, 0, 0 };
			int maxEmptyY = -1, maxEmptyX = -1;  bool isMarkExist = false;
			
			for (int inc = prev; inc <= next; inc++) {
				int yy = checkY + inc, xx = checkX + inc;
				if (isWho(yy, xx, EMPTY)) { maxEmptyY = max(maxEmptyY, yy); maxEmptyX = max(maxEmptyX, xx); }
				BlockTypeCnt[isWho(yy, xx)]++;
				if (ToDownMarked[yy]) isMarkExist = true;
			}
		
			if (BlockTypeCnt[who] >= 4 && BlockTypeCnt[opposite] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isMarkExist
				&& ((!isIn(checkY + prev - 1, checkX + prev - 1)) || (isIn(checkY + prev - 1, checkX + prev - 1) && !isWho(checkY + prev - 1, checkX + prev -1, opposite)))
				&& (!isIn(checkY + next + 1, checkX + next + 1) || (isIn(checkY + next + 1,checkX + next + 1) && !isWho(checkY + next + 1, checkX + next + 1, opposite)))) {
				ToDownMarked[maxEmptyY] = true;
				ToDownMarkedCoor.push_back(mp(maxEmptyY, maxEmptyX));
			}
		}


		vector<bool> ToUpMarked(boardSize, false);
		vector<pair<int, int> > ToUpMarkedCoor;

		//위쪽 방향
		for (int prev = 0; prev >= -5; prev--) {
			int next = prev + 5;

			if (!isIn(checkY + prev, checkX + prev) || !isIn(checkY + next, checkX + next)) continue;

			int BlockTypeCnt[4] = { 0, 0, 0, 0 };
			int minEmptyY = -1, minEmptyX = -1;  bool isMarkExist = false;

			for (int inc = next; inc >= prev; inc--) {
				int yy = checkY + inc, xx = checkX + inc;
				if (isWho(yy, xx, EMPTY)) { minEmptyY = min(minEmptyY, yy); minEmptyX = min(minEmptyX, xx); }
				BlockTypeCnt[isWho(yy, xx)]++;
				if (ToDownMarked[yy]) isMarkExist = true;
			}

			if (BlockTypeCnt[who] >= 4 && BlockTypeCnt[opposite] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isMarkExist
				&& ((!isIn(checkY + prev - 1, checkX + prev - 1)) ||(isIn(checkY +prev -1, checkX +prev - 1) && !isWho(checkY + prev - 1, checkX + prev - 1, opposite)))
				&& (!isIn(checkY + next + 1, checkX + next + 1) ||(isIn(checkY + next + 1, checkX + next + 1) && !isWho(checkY + next + 1, checkX + next + 1, opposite)))) {
				ToDownMarked[minEmptyY] = true;
				ToDownMarkedCoor.push_back(mp(minEmptyY, minEmptyX));
			}
		}


		sort(ToDownMarkedCoor.begin(), ToDownMarkedCoor.end());
		sort(ToUpMarkedCoor.begin(), ToUpMarkedCoor.end());


		if (ToUpMarkedCoor.size() == 1) {
			if (ToDownMarkedCoor[0] == ToUpMarkedCoor[0]) {
				ret.push_back(ToDownMarkedCoor[0]);
				if (ret.size() == 2) return ret;
			}
			else {
				ret.push_back(ToDownMarkedCoor[0]);
				cand.push_back(ToUpMarkedCoor[0]);
				if (ret.size() == 2) return ret;
			}
		}
		else if (ToUpMarkedCoor.size() == 2) {
			if (ToDownMarkedCoor == ToUpMarkedCoor) {
				return ToDownMarkedCoor;
			}
			else {
				ret.push_back(ToDownMarkedCoor[0]);
				ret.push_back(ToUpMarkedCoor[0]);
				return ret;
			}
		}
	}

	//왼쪽아래 방향
	for (int i = 0; i < checkCoordinates.size(); i++) {
		int checkY = checkCoordinates[i].first, checkX = checkCoordinates[i].second;

		vector<bool> ToDownMarked(boardSize, false);
		vector<pair<int, int> > ToDownMarkedCoor;

		//아래방향
		for (int prev = -5; prev <= 0; prev++) {
			int next = prev + 5;

			if (!isIn(checkY + prev, checkX - prev) || !isIn(checkY + next, checkX - next)) continue;

			int BlockTypeCnt[4] = { 0, 0, 0, 0 };
			int minEmptyY = -1, maxEmptyX = -1;  bool isMarkExist = false;

			for (int inc = prev; inc <= next; inc++) {
				int yy = checkY + inc, xx = checkX + inc;
				if (isWho(yy, xx, EMPTY)) { minEmptyY = min(minEmptyY, yy); maxEmptyX = max(maxEmptyX, xx); }
				BlockTypeCnt[isWho(yy, xx)]++;
				if (ToDownMarked[yy]) isMarkExist = true;
			}

			if (BlockTypeCnt[who] >= 4 && BlockTypeCnt[opposite] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isMarkExist
				&& ((!isIn(checkY + prev - 1, checkX - prev + 1)) || (isIn(checkY - prev + 1, checkX - prev + 1) && !isWho(checkY - prev + 1, checkX - prev + 1, opposite)))
				&& (!isIn(checkY + next + 1, checkX - next - 1) || (isIn(checkY + next + 1, checkX - next - 1) && !isWho(checkY + next + 1, checkX - next - 1, opposite)))) {
				ToDownMarked[minEmptyY] = true;
				ToDownMarkedCoor.push_back(mp(minEmptyY, maxEmptyX));
			}
		}

		vector<bool> ToUpMarked(boardSize, false);
		vector<pair<int, int> > ToUpMarkedCoor;

		//위쪽 방향 x커지고 y작아지고
		for (int prev = 0; prev >= -5; prev--) {
			int next = prev + 5;

			if (!isIn(checkY + prev, checkX - prev) || !isIn(checkY + next, checkX - next)) continue;

			int BlockTypeCnt[4] = { 0, 0, 0, 0 };
			int maxEmptyY = -1, minEmptyX = -1;  bool isMarkExist = false;

			for (int inc = next; inc >= prev; inc--) {
				int yy = checkY + inc, xx = checkX + inc;
				if (isWho(yy, xx, EMPTY)) { maxEmptyY = max(maxEmptyY, yy); minEmptyX = min(minEmptyX, xx); }
				BlockTypeCnt[isWho(yy, xx)]++;
				if (ToDownMarked[yy]) isMarkExist = true;
			}

			if (BlockTypeCnt[who] >= 4 && BlockTypeCnt[opposite] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isMarkExist
				&& ((!isIn(checkY + prev - 1, checkX - prev + 1)) || ( isIn(checkY - prev + 1, checkX - prev + 1)&& !isWho(checkY - prev + 1, checkX - prev + 1, opposite)))
				&& (!isIn(checkY + next + 1, checkX - next - 1) || (isIn(checkY + next + 1, checkX - next - 1) && !isWho(checkY + next + 1, checkX - next - 1, opposite)))) {
				ToDownMarked[maxEmptyY] = true;
				ToDownMarkedCoor.push_back(mp(maxEmptyY, minEmptyX));
			}
		}

		sort(ToDownMarkedCoor.begin(), ToDownMarkedCoor.end());
		sort(ToUpMarkedCoor.begin(), ToUpMarkedCoor.end());


		if (ToUpMarkedCoor.size() == 1) {
			if (ToDownMarkedCoor[0] == ToUpMarkedCoor[0]) {
				ret.push_back(ToDownMarkedCoor[0]);
				if (ret.size() == 2) return ret;
			}
			else {
				ret.push_back(ToDownMarkedCoor[0]);
				cand.push_back(ToUpMarkedCoor[0]);
				if (ret.size() == 2) return ret;
			}
		}
		else if (ToUpMarkedCoor.size() == 2) {
			if (ToDownMarkedCoor == ToUpMarkedCoor) {
				return ToDownMarkedCoor;
			}
			else {
				ret.push_back(ToDownMarkedCoor[0]);
				ret.push_back(ToUpMarkedCoor[0]);
				return ret;
			}
		}

	}


	return ret;
}


int CalculateScoreLogic(int who, int y, int x) {

	int opposite = (who == ME ? ENERMY : ME);

	int totalScore = 0;
	int myScore1 = 1;
	int myScore2 = 1;
	//가로방향
	for (int i = 1; i <= 5; i++) {
		if (!isIn(y, x + i)) break;
		int nearWho = isWho(y, x + i);
		if (nearWho == BLOCKING || nearWho == opposite) break;
		if (nearWho == who) myScore1 *= ScoreValues[i];
		if (nearWho == EMPTY) myScore1 *= ScoreValues[6];
	}
	for (int i = 1; i <= 5; i++) {
		if (!isIn(y, x - i)) break;
		int nearWho = isWho(y, x - i);
		if (nearWho == BLOCKING || nearWho == opposite) break;
		if (nearWho == who) myScore2 *= ScoreValues[i];
		if (nearWho == EMPTY) myScore2 *= ScoreValues[6];
	}
	myScore1 *= myScore2;
	totalScore += myScore1;
	myScore1 = 1;
	myScore2 = 1;
	//세로방향
	for (int i = 1; i <= 5; i++) {
		if (!isIn(y + i, x)) break;
		int nearWho = isWho(y + i, x);
		if (nearWho == BLOCKING || nearWho == opposite) break;
		if (nearWho == who) myScore1 *= ScoreValues[i];
		if (nearWho == EMPTY) myScore1 *= ScoreValues[6];
	}
	for (int i = 1; i <= 5; i++) {
		if (!isIn(y - i, x)) break;
		int nearWho = isWho(y - i, x);
		if (nearWho == BLOCKING || nearWho == opposite) break;
		if (nearWho == who) myScore2 *= ScoreValues[i];
		if (nearWho == EMPTY) myScore2 *= ScoreValues[6];
	}
	myScore1 *= myScore2;
	totalScore += myScore1;
	myScore1 = 1;
	myScore2 = 1;
	//오른아래 대각선방향
	for (int i = 1; i <= 5; i++) {
		if (!isIn(y + i, x + i)) break;
		int nearWho = isWho(y + i, x + i);
		if (nearWho == BLOCKING || nearWho == opposite) break;
		if (nearWho == who) myScore1 *= ScoreValues[i];
		if (nearWho == EMPTY) myScore1 *= ScoreValues[6];
	}
	for (int i = 1; i <= 5; i++) {
		if (!isIn(y - i, x - i)) break;
		int nearWho = isWho(y - i, x - i);
		if (nearWho == BLOCKING || nearWho == opposite) break;
		if (nearWho == who) myScore2 *= ScoreValues[i];
		if (nearWho == EMPTY) myScore2 *= ScoreValues[6];
	}
	myScore1 *= myScore2;
	totalScore += myScore1;
	myScore1 = 1;
	myScore2 = 1;
	//왼쪽아래 대각선방향
	for (int i = 1; i <= 5; i++) {
		if (!isIn(y + i, x - i)) break;
		int nearWho = isWho(y + i, x - i);
		if (nearWho == BLOCKING || nearWho == opposite) break;
		if (nearWho == who) myScore1 *= ScoreValues[i];
		if (nearWho == EMPTY) myScore1 *= ScoreValues[6];
	}
	for (int i = 1; i <= 5; i++) {
		if (!isIn(y - i, x + i)) break;
		int nearWho = isWho(y - i, x + i);
		if (nearWho == BLOCKING || nearWho == opposite) break;
		if (nearWho == who) myScore2 *= ScoreValues[i];
		if (nearWho == EMPTY) myScore2 *= ScoreValues[6];
	}
	myScore1 *= myScore2;
	totalScore += myScore1;
	return totalScore;
}

int CalculateScore(int who, int y, int x) {
	int opposite = who == ME ? ENERMY : ME;

	//who 입장에서 공격효과와 반대편의 방어효과를 동시에 고려하기 위함
	return CalculateScoreLogic(who, y, x) + CalculateScoreLogic(opposite, y, x);
}

#pragma endregion

#pragma region 본격 재귀 -> 점점 깊어지는 DFS로 구현. 필수적인 정보들을 위에 선언.   알파 베타

////DFS를 통해 먼저 탐색할 만한 것들을 순서대로 나열해봄
//vector<pair<int, int> > refCoordinate;
//who = 누구의 차례인지 who = 1 나의 차례, who = 2 적의 차례
//depth = 재귀의 깊이, 홀수이면 나의 차례, 짝수이면 적의 차례
//2개의 수를 놓음
ll search(int who, int depth) {
	checkTime();

	if (GlobalStopBecauseTimeLimit) return -10000000;
	if (depth > depthLimit) return -10000000;

	int opposite = (who == ME ? ENERMY : ME);


	ll ret = -10000000;

	//1. 공격 point가 있나 보자! 여기서는 무조건 내가 이기는 경우의 좌표를 반환한다.
	vector<pair<int, int> > attackPoint = shouldCheckAttack(who);
	//내가 이기는 경우가 있다! 여기 놓으면 이긴다! 이기는 점수를 반환
	if (attackPoint.size() > 0) {

		if (depth == 1) {
			ansY[0] = attackPoint[0].first; ansX[0] = attackPoint[0].second;
			if(attackPoint.size() == 2)
				ansY[1] = attackPoint[1].first; ansX[1] = attackPoint[1].first;
		}
		return ret = 20000000;
	}
	//2. 방어 point가 있나보자!. 여기서는 무조건 내가 지지 않기 위한 방어의 좌표가 있는지 탐색하고 그 좌표를 반환한다.
	//defencePoint에 반환되는 결과 값은 무조건 막아야 하는 경우 
	//cand에는 막아야하는데 상대적 결과를 통해 선택되어야 할 것들
	cand.clear();
	vector<pair<int, int> > defencePoint = shouldCheckDefence(who);

	//일단 방어를 하고 봐야하기 때문에 이 좌표를 둔다.
	if (defencePoint.size() >= 2) {
		int y1 = defencePoint[0].first, x1 = defencePoint[0].second;
		int y2 = defencePoint[1].first, x2 = defencePoint[1].second;

		ret = 20000000;

		//다음 탐색을 위해 놓았던 것을 없앤다
		if (depth == 1) {
			ansY[0] = defencePoint[0].first; ansX[0] = defencePoint[0].second;
			ansY[1] = defencePoint[1].first; ansX[1] = defencePoint[1].first;
		}
		return ret * 0.7;
	}

	if (defencePoint.size() == 1) {
		if (cand.size() >= 1) {
			vector<pair<int, pair<int, int> > > searchScoreAndCoor;
			for (int i = 0; i < cand.size(); i++) {
				searchScoreAndCoor.push_back(mp(-CalculateScore(who, cand[i].first, cand[i].second), cand[i]));
			}
			sort(searchScoreAndCoor.begin(), searchScoreAndCoor.end());

			for (int i = 0; i < min(3, searchScoreAndCoor.size()); i++) {
				int y1 = defencePoint[0].first, x1 = defencePoint[0].second;
				int y2 = searchScoreAndCoor[i].second.first, x2 = searchScoreAndCoor[i].second.second;
				int score1 = 10000000, score2 = -searchScoreAndCoor[i].first;

				if (isIn(y1,x1) && isIn(y2,x2) && isEmpty(y1, x1) && isEmpty(y2, x2)) {
					ll temp = score1 + score2;
					if (temp > ret) {
						ret = temp;
						if (depth == 1) {
							ansY[0] = y1, ansX[0] = x1, ansY[1] = y2, ansX[1] = x2;
						}
					}
				}
			}
		}
		else {
			vector<pair<int, pair<int, int> > > searchScoreAndCoor;
			vector<pair<int, int> > searchCoordinates = makeReference();
			for (int i = 0; i < searchCoordinates.size(); i++) {
				searchScoreAndCoor.push_back(mp(-CalculateScore(who, searchCoordinates[i].first, searchCoordinates[i].second), searchCoordinates[i]));
			}
			sort(searchScoreAndCoor.begin(), searchScoreAndCoor.end());

			for (int i = 0; i < min(50, searchScoreAndCoor.size()); i++) {

				int y1 = defencePoint[0].first, x1 = defencePoint[0].second;
				int y2 = searchScoreAndCoor[i].second.first, x2 = searchScoreAndCoor[i].second.second;

				int score1 = 10000000, score2 = -searchScoreAndCoor[i].first;

				if (isEmpty(y1, x1) && isEmpty(y2, x2)) {
					setYX(y1, x1, who); setYX(y2, x2, who); totalMEandENERMY += 2;
					recordOfSet.push_back(mp(who, mp(y1, x1))); recordOfSet.push_back(mp(who, mp(y2, x2)));

					ll temp = score1 + score2 - search(opposite, depth + 1);
					if (temp > ret) {
						ret = temp;
						if (depth == 1) {
							ansY[0] = y1, ansX[0] = x1, ansY[1] = y2, ansX[1] = x2;
						}
					}
					//다음 탐색을 위해 놓았던 것을 없앤다
					unSetYX(y1, x1, who); unSetYX(y2, x2, who); totalMEandENERMY -= 2; recordOfSet.pop_back(); recordOfSet.pop_back();
				}
			}
		}
		return ret * 0.7;
	}
	if (cand.size() >= 2) {

		vector<pair<int, pair<int, int> > > searchScoreAndCoor;
		for (int i = 0; i < cand.size(); i++) {
			searchScoreAndCoor.push_back(mp(-1.2 *CalculateScore(who, cand[i].first, cand[i].second), cand[i]));
		}
		sort(searchScoreAndCoor.begin(), searchScoreAndCoor.end());

		for (int i = 0; i < searchScoreAndCoor.size() - 1; i++) {
			for (int j = i + 1; j < searchScoreAndCoor.size(); j++) {
				int y1 = searchScoreAndCoor[j].second.first, x1 = searchScoreAndCoor[j].second.second;
				int y2 = searchScoreAndCoor[i].second.first, x2 = searchScoreAndCoor[i].second.second;

				int score1 = -searchScoreAndCoor[j].first, score2 = -searchScoreAndCoor[i].first;

				if (isIn(y1, x1) && isIn(y2, x2) && isEmpty(y1, x1) && isEmpty(y2, x2)) {
					setYX(y1, x1, who); setYX(y2, x2, who); totalMEandENERMY += 2;
					recordOfSet.push_back(mp(who, mp(y1, x1))); recordOfSet.push_back(mp(who, mp(y2, x2)));

					ll temp = score1 + score2 - search(opposite, depth + 1);
					if (temp > ret) {
						ret = temp;
						if (depth == 1) {
							ansY[0] = y1, ansX[0] = x1, ansY[1] = y2, ansX[1] = x2;
						}
					}
					//다음 탐색을 위해 놓았던 것을 없앤다
					unSetYX(y1, x1, who); unSetYX(y2, x2, who); totalMEandENERMY -= 2; recordOfSet.pop_back(); recordOfSet.pop_back();
				}
			}
		}
		return ret * 0.7;
	}
	if (cand.size() == 1) {

		vector<pair<int, int> > searchCoordinates = makeReference();

		vector<pair<int, pair<int, int> > > searchScoreAndCoor;

		for (int i = 0; i < searchCoordinates.size(); i++) {
			searchScoreAndCoor.push_back(mp(-CalculateScore(who, searchCoordinates[i].first, searchCoordinates[i].second), searchCoordinates[i]));
		}
		int score1 = CalculateScore(who, cand[0].first, cand[0].second) * 1.2;

		sort(searchScoreAndCoor.begin(), searchScoreAndCoor.end());

		for (int i = 0; i < min(50, searchScoreAndCoor.size()); i++) {

			int y1 = cand[0].first, x1 = cand[0].second;
			int y2 = searchScoreAndCoor[i].second.first, x2 = searchScoreAndCoor[i].second.second;

			int score2 = -searchScoreAndCoor[i].first;

			if (isIn(y1, x1) && isIn(y2, x2) && isEmpty(y1, x1) && isEmpty(y2, x2)) {
				setYX(y1, x1, who); setYX(y2, x2, who); totalMEandENERMY += 2;
				recordOfSet.push_back(mp(who, mp(y1, x1))); recordOfSet.push_back(mp(who, mp(y2, x2)));

				ll temp = score1 + score2 - search(opposite, depth + 1);
				if (temp > ret) {
					ret = temp;
					if (depth == 1) {
						ansY[0] = y1, ansX[0] = x1, ansY[1] = y2, ansX[1] = x2;
					}
				}
				//다음 탐색을 위해 놓았던 것을 없앤다
				unSetYX(y1, x1, who); unSetYX(y2, x2, who); totalMEandENERMY -= 2; recordOfSet.pop_back(); recordOfSet.pop_back();
			}
		}
		return ret * 0.7;
	}

	//refTable을 통해 탐색의 범위 세팅
	vector<pair<int, int> > searchCoordinates = makeReference();

	vector<pair<int, pair<int, int> > > searchScoreAndCoor;
	

	for (int i = 0; i < searchCoordinates.size(); i++) {
		searchScoreAndCoor.push_back(mp(-CalculateScore(who, searchCoordinates[i].first, searchCoordinates[i].second), searchCoordinates[i]));
	}
	sort(searchScoreAndCoor.begin() , searchScoreAndCoor.end());

	for (int i = 0; i < min(9, searchScoreAndCoor.size() - 1); i++) {
		for (int j = i + 1; j < min(10, searchScoreAndCoor.size()); j++) {
			int y1 = searchScoreAndCoor[i].second.first, x1 = searchScoreAndCoor[i].second.second;
			int y2 = searchScoreAndCoor[j].second.first, x2 = searchScoreAndCoor[j].second.second;
			int score1 = -searchScoreAndCoor[i].first, score2 = -searchScoreAndCoor[j].first;

			if (isIn(y1, x1) && isIn(y2, x2) && isEmpty(y1, x1) && isEmpty(y2, x2)) {

				setYX(y1, x1, who); setYX(y2, x2, who); totalMEandENERMY += 2;
				recordOfSet.push_back(mp(who, mp(y1, x1))); recordOfSet.push_back(mp(who, mp(y2, x2)));

				ll temp = score1 + score2 - search(opposite, depth + 1);

				if (temp > ret) {

					ret = temp;
					if (depth == 1) {
						ansY[0] = y1, ansX[0] = x1, ansY[1] = y2, ansX[1] = x2;
					}
				}
				//다음 탐색을 위해 놓았던 것을 없앤다
				unSetYX(y1, x1, who); unSetYX(y2, x2, who); totalMEandENERMY -= 2; recordOfSet.pop_back(); recordOfSet.pop_back();
			}
		}
	}
	return ret * 0.7;
}

#pragma endregion


//내가 무조건 이기는 경우가 있는지 찾는다. 무조건 이기는 경우 바로 리턴을 한다.
bool checkMyAttack() {

	//가로방향
	for (int y = 0; y < boardSize; y++) {
		//[0]-> EMPTY 숫자, [1] -> ME 숫자, [2] -> ENERMY 숫자, [3] -> BLOCKING 숫자 
		int BlockTypeCnt[4] = { 0 , 0, 0, 0 };

		//ShouldAttack이 -1이 아니면 내가 무조건 이기는 경우이다. 낄낄
		int ShouldAttack = -1;
		
		for (int x = 0; x < 6; x++) {
			BlockTypeCnt[isWho(y, x)]++;
		}

		if ((BlockTypeCnt[ME] >= 4) && (BlockTypeCnt[ENERMY] == 0 || BlockTypeCnt[BLOCKING] == 0) && !(isWho(y, 6, ME))) {
			ShouldAttack = 0;
		}

		if (ShouldAttack == -1) {
			for (int sub = 0; sub < boardSize - 6; sub++) {
				int add = sub + 6;
				BlockTypeCnt[isWho(y, sub)]--; BlockTypeCnt[isWho(y, add)]++;
				if (BlockTypeCnt[ME] >= 4 && (BlockTypeCnt[ENERMY] == 0 && BlockTypeCnt[BLOCKING] == 0)
					&& !isWho(y, sub, ME) && ((isIn(y, add + 1) && !isWho(y, add + 1, ME)) || !isIn(y, add + 1 )) ) {
					ShouldAttack = sub + 1;
					break;
				}
			}
		}
		//공격을 해야하는 경우! 무조건 내가 이기는 경우다
		if (ShouldAttack != -1) {
			
			if (BlockTypeCnt[ME] == 4) {
				int index = 0;
				for (int x = ShouldAttack; x < ShouldAttack + 6; x++) {
					if (isWho(y, x, EMPTY)) ansY[index] = y, ansX[index++] = x;
				}
			}
			else if (BlockTypeCnt[ME] == 5) {
				for (int x = ShouldAttack; x < ShouldAttack + 6; x++) {
					if (isWho(y, x, EMPTY)) ansY[0] = y, ansX[0] = x;
				}

				srand((unsigned)time(NULL));
				while (1) {
					ansX[1] = rand() % boardSize;
					ansY[1] = rand() % boardSize;
					if ((isWho(ansX[1], ansY[1], EMPTY)) && ((ansY[1] != ansY[0])
						 || ((ansY[0] == ansY[1]) && (ansX[1] != ShouldAttack - 1 && ansX[1] != ShouldAttack + 7))) ) {
						return true;
					}
				}
			}
			return true;
		}
	}

	//세로방향
	for (int x = 0; x < boardSize; x++) {
		//[0]-> EMPTY 숫자, [1] -> ME 숫자, [2] -> ENERMY 숫자, [3] -> BLOCKING 숫자 
		int BlockTypeCnt[4] = { 0 , 0, 0, 0 };

		//ShouldAttack이 -1이 아니면 내가 무조건 이기는 경우이다. 낄낄
		int ShouldAttack = -1;

		for (int y = 0; y < 6; y++) {
			BlockTypeCnt[isWho(y, x)]++;
		}

		if ((BlockTypeCnt[ME] >= 4) && (BlockTypeCnt[ENERMY] == 0 || BlockTypeCnt[BLOCKING] == 0) && !(isWho(6, x, ME))) {
			ShouldAttack = 0;
		}

		if (ShouldAttack == -1) {
			for (int sub = 0; sub < boardSize - 6; sub++) {
				int add = sub + 6;
				BlockTypeCnt[isWho(sub, x)]--; BlockTypeCnt[isWho(add, x)]++;
				if (BlockTypeCnt[ME] >= 4 && (BlockTypeCnt[ENERMY] == 0 && BlockTypeCnt[BLOCKING] == 0)
					&& !isWho(sub, x, ME) && ((isIn(add + 1, x) && !isWho(add + 1, x, ME)) || !isIn(add + 1 , x))) {

					ShouldAttack = sub + 1;
					break;
				}
			}
		}
		//공격을 해야하는 경우! 무조건 내가 이기는 경우다
		if (ShouldAttack != -1) {

			if (BlockTypeCnt[ME] == 4) {
				int index = 0;
				for (int y = ShouldAttack; y < ShouldAttack + 6; y++) {
					if (isWho(y, x, EMPTY)) ansY[index] = y, ansX[index++] = x;
				}
			}
			else if (BlockTypeCnt[ME] == 5) {
				for (int y = ShouldAttack; y < ShouldAttack + 6; y++) {
					if (isWho(y, x, EMPTY)) ansY[0] = y, ansX[0] = x;
				}

				srand((unsigned)time(NULL));
				while (1) {
					ansX[1] = rand() % boardSize;
					ansY[1] = rand() % boardSize;
					if ((isWho(ansX[1], ansY[1], EMPTY)) && ((ansX[1] != ansX[0])
						 || ((ansX[0] == ansX[1]) && (ansY[1] != ShouldAttack - 1 && ansY[1] != ShouldAttack + 7)))) {
						return true;
					}
				}
			}
			return true;
		}
	}
	
	bool check = true;
	//오른아래쪽 대각선 방향
	for (int y = 0; y < boardSize; y++) {
		check = true;
		
		for (int x = 0; x < boardSize; x++) {
			//[0]-> EMPTY 숫자, [1] -> ME 숫자, [2] -> ENERMY 숫자, [3] -> BLOCKING 숫자 
			int BlockTypeCnt[4] = { 0 , 0, 0, 0 };
			for (int num = 0; num < 6; num++) {
				if (!isIn(y + num, x + num)) {
					check = false;
					break;
				}
				BlockTypeCnt[isWho(y + num, x + num)]++;
			}

			if (!check) break;
			if (BlockTypeCnt[ME] >= 4 && BlockTypeCnt[ENERMY] == 0 && BlockTypeCnt[BLOCKING] == 0) {
				if ((!isIn(y - 1, x - 1) || !(isWho(y - 1, x - 1, ME))) 
					  && ( !isIn(y + 6, x + 6) || !(isWho(y + 6, x + 6, ME)))) {
					if (BlockTypeCnt[ME] == 4) {
						int index = 0;
						for (int num = 0; num < 6; num++) {
							if (isWho(y + num, x + num, EMPTY)) {
								ansY[index] = y + num, ansX[index++] = x + num;
							}
						}
						return true;
					}
					else if (BlockTypeCnt[ME] == 5) {
						for (int num = 0; num < 6; num++) {
							if (isWho(y + num, x + num, EMPTY)) {
								ansY[0] = y + num, ansX[0] = x + num;
								break;
							}
						}
						srand((unsigned)time(NULL));
						while (1) {
							ansX[1] = rand() % boardSize;
							ansY[1] = rand() % boardSize;
							if ((isWho(ansY[1], ansX[1], EMPTY)) && (ansY[1] < y || ansY[1] > y + 6) && (ansX[1] < x || ansX[1] > x + 6)) {
								return true;
							}
						
						}
					}
				}
			}
		}
		if (!check) continue;
	}

	
	//왼쪽아래 대각선 방향
	for (int y = boardSize - 1; y >= 0; y--) {
		check = true;
		for (int x = boardSize - 1; x >= 0; x--) {

			int BlockTypeCnt[4] = { 0 , 0, 0, 0 };
			for (int num = 0; num < 6; num++) {
				if (!isIn(y + num, x - num)) {
					check = false;
					break;
				}
				BlockTypeCnt[isWho(y + num, x - num)]++;
			}

			if (!check) break;

			if (BlockTypeCnt[ME] >= 4 && BlockTypeCnt[ENERMY] == 0 && BlockTypeCnt[BLOCKING] == 0) {
				if ((!isIn(y - 1, x + 1) || !(isWho(y - 1, x + 1, ME)))
					&& (!isIn(y + 6, x - 6) || !(isWho(y + 6, x - 6, ME)))) {
					if (BlockTypeCnt[ME] == 4) {
						int index = 0;
						for (int num = 0; num < 6; num++) {
							if (isWho(y + num, x - num, EMPTY)) {
								ansY[index] = y + num, ansX[index++] = x - num;
							}
						}
						return true;
					}
					else if (BlockTypeCnt[ME] == 5) {
						for (int num = 0; num < 6; num++) {
							if (isWho(y + num, x - num, EMPTY)) {
								ansY[0] = y + num, ansX[0] = x - num;
								break;
							}
						}
						srand((unsigned)time(NULL));
						while (1) {
							ansX[1] = rand() % boardSize;
							ansY[1] = rand() % boardSize;
							if ((isWho(ansY[1], ansX[1], EMPTY)) && (ansY[1] < y || ansY[1] > y + 6) && (ansX[1] < x || ansX[1] > x + 6)) {
								return true;
							}
						}
					}
				}
			}
		}
		if (!check) continue;
	}
	return false;
}



//내가 무조건 방어해야하는 경우가 있는지 찾는다. 무조건 방어하고 바로 리턴한다.
bool checkMyDefence() {

	//가로방향
	for (int y = 0; y < boardSize; y++) {
		int BlockTypeCnt[4] = { 0,0,0,0 };
		
		int maxEmptyX = -1;
		for (int x = 0; x < 6; x++) {
			BlockTypeCnt[isWho(y, x)]++;
			if (isWho(y, x, EMPTY)) maxEmptyX = max(maxEmptyX, x);
		}

		//무조건 둬야하는 경우 양옆에 적이아니고..
		if (BlockTypeCnt[ENERMY] >= 4 && BlockTypeCnt[ME] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isWho(y, 6, ENERMY)) {
			ansY[setNum] = y, ansX[setNum++] = maxEmptyX;
			if (setNum >= 2) return true;
		}

		for (int sub = 0; sub < boardSize - 6; sub++) {
			int add = sub + 7;

			BlockTypeCnt[isWho(y, sub)]--; BlockTypeCnt[isWho(y, add)]++;
			if (isWho(y, add, EMPTY)) maxEmptyX = max(maxEmptyX, add);
			//무조건 둬야하는 경우 양옆에 적이아니고..
			if (BlockTypeCnt[ENERMY] >= 4 && BlockTypeCnt[ME] == 0 && BlockTypeCnt[BLOCKING] == 0
				  && (!isIn(y, add + 1) || (isIn(y, add + 1) && !isWho(y, add + 1, ENERMY)))) {
				ansY[setNum] = y, ansX[setNum++] = maxEmptyX;
				if (setNum >= 2) return true;
			}
		}
	}


	//세로방향
	for (int x = 0; x < boardSize; x++) {
		int BlockTypeCnt[4] = { 0,0,0,0 };

		int maxEmptyY = -1;
		for (int y = 0; y < 6; y++) {
			BlockTypeCnt[isWho(y, x)]++;
			if (isWho(y, x, EMPTY)) maxEmptyY = max(maxEmptyY, y);
		}

		//무조건 둬야하는 경우 양옆에 적이아니고..
		if (BlockTypeCnt[ENERMY] >= 4 && BlockTypeCnt[ME] == 0 && BlockTypeCnt[BLOCKING] == 0 && !isWho(6, x, ENERMY)) {
			ansY[setNum] = maxEmptyY, ansX[setNum++] = x;
			if (setNum >= 2) return true;
		}

		for (int sub = 0; sub < boardSize - 6; sub++) {
			int add = sub + 7;

			BlockTypeCnt[isWho(sub, x)]--; BlockTypeCnt[isWho(add, x)]++;
			if (isWho(add, x, EMPTY)) maxEmptyY = max(maxEmptyY, add);
			//무조건 둬야하는 경우 양옆에 적이아니고..
			if (BlockTypeCnt[ENERMY] >= 4 && BlockTypeCnt[ME] == 0 && BlockTypeCnt[BLOCKING] == 0
				&& (!isIn(add + 1, x) || (isIn(add + 1, x) && !isWho(add + 1, x, ENERMY)))) {
				ansY[setNum] = maxEmptyY, ansX[setNum++] = x;
				if (setNum >= 2) return true;
			}
		}
	}
	//오른쪽아래 대각선인 경우
	bool check = true;
	for (int y = 0; y < boardSize; y++) {
		check = false;
		for (int x = 0; x < boardSize; x++) {
			int BlockTypeCnt[4] = { 0 , 0, 0, 0 };
			int maxEmptyY = -1, maxEmptyX = -1;
			for (int num = 0; num < 6; num++) {
				if (!isIn(y + num, x + num)) {
					check = false;
					break;
				}
				if (isWho(y + num, x + num, EMPTY))
					maxEmptyY = max(maxEmptyY, y + num); maxEmptyX = max(maxEmptyX, x + num);
				BlockTypeCnt[isWho(y + num, x + num)]++;
			}
			if (!check) break;

			if (BlockTypeCnt[ENERMY] >= 4 && BlockTypeCnt[ME] == 0 && BlockTypeCnt[BLOCKING] == 0
				&& (!isIn(y - 1, x - 1) || (isIn(y - 1, x - 1) && !isWho(y - 1, x - 1, ENERMY)))
				&& (!isIn(y + 6, x + 6) || (isIn(y + 6, x + 6) && !isWho(y + 6, x + 6, ENERMY)))) {
				ansY[setNum] = maxEmptyY, ansX[setNum++] = maxEmptyX;
				if (setNum >= 2) return true;
			}
		}
		if (!check) continue;
	}

	//왼쪽 대각선 아래인 경우
	for (int y = boardSize - 1; y >= 0; y--) {
		check = true;
		for (int x = boardSize - 1; x >= 0; x--) {
			int BlockTypeCnt[4] = { 0 , 0, 0, 0 };
			int maxEmptyY = -1, minEmptyX = 30;
			for (int num = 0; num < 6; num++) {
				if (!isIn(y + num, x + num)) {
					check = false;
					break;
				}
				if(isWho(y + num, x - num , EMPTY))
					maxEmptyY = max(maxEmptyY, y + num); minEmptyX = min(minEmptyX, x - num);
				BlockTypeCnt[isWho(y + num, x + num)]++;
			}
			if (!check) break;

			if (BlockTypeCnt[ENERMY] >= 4 && BlockTypeCnt[ME] == 0 && BlockTypeCnt[BLOCKING] == 0
				&& (!isIn(y - 1, x + 1) || (isIn(y - 1, x + 1) && !isWho(y - 1, x + 1, ENERMY)))
				&& (!isIn(y + 6, x - 6) || (isIn(y + 6, x - 6) && !isWho(y + 6, x - 6, ENERMY)))) {
				ansY[setNum] = maxEmptyY, ansX[setNum++] = minEmptyX;
				if (setNum >= 2) return true;
			}
		}
		if (!check) continue;
	}
	if (setNum == 1) return true;
	return false;

}

//최종적으로 ansY[], ansX[]의 값을 넣어주는 역할
//시간에 대한 부분도 추후 넣어야할듯.
void AURAStart() {

	myBoardInitWhenMyTurnIsCalled();
	
	//가지치기전 내가 무조건 이기는 경우 혹은 내가 무조건 지는 경우를 먼저 판단하고 공격 혹은 방어를 한다.
	if (checkMyAttack()) {
		return;
	}

	setNum = 0;
	if (checkMyDefence()) {
		if (setNum == 2) return;
	}
	//search의 depth를 조정하며 점점 깊어지는 DFS를 구현
	//depth를 2씩 증가시키며 탐색
	//시간을 재고 시간에 따라 return 하면 됨.
	depthLimit = 3; 
	while(1) {
		search(ME, 1);
		depthLimit += 2;
		if (GlobalStopBecauseTimeLimit) return;
	}
}

void myturn(int cnt) {
	beginTime = clock();

	//전체 돌의 개수를 저장해놓음.
	totalMEandENERMY += cnt;
	GlobalStopBecauseTimeLimit = false;

	if (cnt == 1) {
		dirAdjInit();
		//가운데 벽이 있을 경우 2칸띄어서 부터 넣음.
		if(isFree(9,9))
			ansX[0] = ansY[0] = 9; 
		else {
			for (int j = 2; j < 8; j++) {
				for (int i = 0; i < numOfDir; i++) {
					int XXX = 9 + j * dx[i], YYY = 9 + j * dy[i];
					if (isFree(XXX, YYY)) {
						ansX[0] = XXX, ansY[0] = YYY;
						setYX(ansY[0], ansX[0], ME);
						recordOfSet.push_back(mp(ME, mp(ansY[0], ansX[0])));
						domymove(ansX, ansY, cnt);
						return;
					}
				}
			}
		}
		setYX(ansY[0], ansX[0], ME);
		recordOfSet.push_back(mp(ME, mp(ansY[0], ansX[0])));
		domymove(ansX, ansY, cnt);
		return;
	}

	AURAStart();

	if (setNum == 1) {

	}

	// 이 부분에서 자신이 놓을 돌을 출력하십시오.
	// 필수 함수 : domymove(x배열,y배열,배열크기)
	// 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.
	setYX(ansY[0], ansX[0], ME); setYX(ansY[1], ansX[1], ME); 
	recordOfSet.push_back(mp(ME, mp(ansY[0], ansX[0]))); recordOfSet.push_back(mp(ME, mp(ansY[1], ansX[1])));
	domymove(ansX, ansY, cnt);
	return;
}
