// Samsung Go Tournament Form C (g++-4.8.3)

/*
[AI �ڵ� �ۼ� ���]

1. char info[]�� �迭 �ȿ�					"TeamName:�ڽ��� ����,Department:�ڽ��� �Ҽ�"					������ �ۼ��մϴ�.
( ���� ) Teamname:�� Department:�� �� ���� �մϴ�.
"�ڽ��� ����", "�ڽ��� �Ҽ�"�� �����ؾ� �մϴ�.

2. �Ʒ��� myturn() �Լ� �ȿ� �ڽŸ��� AI �ڵ带 �ۼ��մϴ�.

3. AI ������ �׽�Ʈ �Ͻ� ���� "���� �˰����ȸ ��"�� ����մϴ�.

4. ���� �˰��� ��ȸ ���� �����ϱ⿡�� �ٵϵ��� ���� ��, �ڽ��� "����" �� �� �˰����� �߰��Ͽ� �׽�Ʈ �մϴ�.



[���� �� �Լ�]
myturn(int cnt) : �ڽ��� AI �ڵ带 �ۼ��ϴ� ���� �Լ� �Դϴ�.
int cnt (myturn()�Լ��� �Ķ����) : ���� �� �� �־��ϴ��� ���ϴ� ����, cnt�� 1�̸� ���� ���� ��  �� ����  �δ� ��Ȳ(�� ��), cnt�� 2�̸� �� ���� ���� �δ� ��Ȳ(�� ��)
int  x[0], y[0] : �ڽ��� �� ù �� ° ���� x��ǥ , y��ǥ�� ����Ǿ�� �մϴ�.
int  x[1], y[1] : �ڽ��� �� �� �� ° ���� x��ǥ , y��ǥ�� ����Ǿ�� �մϴ�.
void domymove(int x[], int y[], cnt) : �� ������ ��ǥ�� �����ؼ� ���


//int board[BOARD_SIZE][BOARD_SIZE]; �ٵ��� �����Ȳ ��� �־� �ٷλ�� ������. ��, ���������ͷ� ���� �������
// ������ ���� ��ġ�� �ٵϵ��� ������ �ǰ��� ó��.

boolean ifFree(int x, int y) : ���� [x,y]��ǥ�� �ٵϵ��� �ִ��� Ȯ���ϴ� �Լ� (������ true, ������ false)
int showBoard(int x, int y) : [x, y] ��ǥ�� ���� ���� �����ϴ��� �����ִ� �Լ� (1 = �ڽ��� ��, 2 = ����� ��, 3 = ��ŷ)


<-------AI�� �ۼ��Ͻ� ��, ���� �̸��� �Լ� �� ���� ����� �������� �ʽ��ϴ�----->
*/

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <iostream>
#include <vector>
#include "Connect6Algo.h"

using namespace std;

// "�����ڵ�[C]"  -> �ڽ��� ���� (����)
// "AI�μ�[C]"  -> �ڽ��� �Ҽ� (����)
// ����� ���������� �ݵ�� �������� ����!
char info[] = { "TeamName:aura,Department:IoT���ȭ��]" };




//�ٵѵ��� �δ� �� 
//(y, x)
//who -> 1�̸� ���� ��, 2�̸� ������ ��
void setYX(vector<int>& myBoard, int y, int x, int who) {
	int position = y * 19 + x;
	//cout << position << " "<< (position >> 4);
	myBoard[position >> 4] |= (who << (position % 16 * 2));
}


//(y, x)�ڸ��� �ٵϵ��� ���ִ� ��
void unSetYX(vector<int>& myBoard, int y, int x, int who) {
	int position = y * 19 + x;
	myBoard[position >> 4] &= ~(3 << (position % 16 * 2));
}

//(y, x)�ڸ��� �ƹ� �͵� ������ Ȯ��
bool isEmpty(vector<int>& myBoard, int y, int x) {
	int position = y * 19 + x;
	return (myBoard[position >> 4] | (3 << (position % 16 * 2))) == 0;
}


//board[][]�� ���¸� bitmask�� ���̴� ��. myturn�� �� ������ �����ؼ� ����� �ʱ�ȭ
//int 91���� board�� ���¸� �� ǥ�������ϴ�
void myBoardSetting(vector<int>& myBoard) {

	for (int y = 0; y < 19; y++) {
		for (int x = 0; x < 19; x++) {
			//0 ����ִ�, 1 ���� ��, 2 ������ ��, 3 ��ŷ
			setYX(myBoard, y, x, showBoard(x, y));
		}
	}
}




void myturn(int cnt) {

	int x[2], y[2];

	// �� �κп��� �˰��� ���α׷�(AI)�� �ۼ��Ͻʽÿ�. �⺻ ������ �ڵ带 ���� �Ǵ� �����ϰ� ������ �ڵ带 ����Ͻø� �˴ϴ�.
	// ���� Sample code�� AI�� Random���� ���� ���� Algorithm�� �ۼ��Ǿ� �ֽ��ϴ�.
	
	cout << cnt << endl;

	if (cnt == 1) {
		x[0] = y[0] = 9;
		domymove(x, y, cnt);
		return;
	}

	vector<int> myBoard(91, 0);

	myBoardSetting(myBoard);

	setYX(myBoard, 0, 1, 1);

	cout << myBoard[0] << endl;

	unSetYX(myBoard, 0, 1, 1);
	cout << myBoard[0] << endl;

	/*srand((unsigned)time(NULL));

	for (int i = 0; i < cnt; i++) {
		do {
			x[i] = rand() % width;
			y[i] = rand() % height;
			if (terminateAI) return;
		} while (!isFree(x[i], y[i]));

		if (x[1] == x[0] && y[1] == y[0]) i--;
	}*/


	// �� �κп��� �ڽ��� ���� ���� ����Ͻʽÿ�.
	// �ʼ� �Լ� : domymove(x�迭,y�迭,�迭ũ��)
	// ���⼭ �迭ũ��(cnt)�� myturn()�� �Ķ���� cnt�� �״�� �־���մϴ�.
	//domymove(x, y, cnt);
}


