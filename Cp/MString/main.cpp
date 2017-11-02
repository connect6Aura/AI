
#include "Global.H"
#include <iostream>
using namespace std;
#include "MString.h"

void main(void)
{
	MString aa("seoul"), bb;

	bb = "KOREA";
	// bb.operator =("KOREA");
	cout << bb.GetString() << endl;

	MString cc;

	cc = aa + bb;
	cout << cc << endl;

	cc = aa + "samsung";
	cout << cc << endl;

	cc = "samsung" + bb;
	cout << cc << endl;
	// cc.operator const char *();

	cout << StrLen(cc) << endl;

	if (aa == bb);
	if (aa == "seoul");
	if ("seoul" == aa);

	aa[1] = 'A';
	// aa.operator [](1);
	cout << aa[1] << endl;

	*aa = 'A';
	*(aa + 2) = 'Z';
	cout << aa << endl;
}




