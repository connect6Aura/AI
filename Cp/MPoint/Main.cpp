
#include <iostream>
using namespace std;
#include "MPoint.H"

void main(void)
{
	MPoint aa(5, 3), bb(3), cc;

	cc = aa + bb;
	// aa.operator +(bb);
	cc = 3 + aa;
	//cc = operator +(3, aa);
	cc.PrintPos();

	cout << "MPoint : ";
	cin >> cc;
	// operator >>(cin, cc);
	cout << cc << endl;
	// operator <<(cout, cc);

	aa = cc++; // cc.operator ++(0);
	bb = ++cc; // cc.operator ++();

	cc = aa += bb;
	cc = aa += 3;
}








