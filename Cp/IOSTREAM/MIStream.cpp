
#include <stdio.h>
#include "MIStream.h"

MIStream cin;

MIStream::MIStream()
{
	format = "%d";
}

MIStream::~MIStream()
{
}

void MIStream::SetFormat(const char * f)
{
	format = f;
}

MIStream & MIStream::operator >>(int & n)
{
	::scanf(format, &n);
	return *this;
}

MIStream & MIStream::operator >>(char & c)
{
	::scanf("%c", &c);
	return *this;
}

MIStream & MIStream::operator >>(char * str)
{
	::scanf("%s", str);
	return *this;
}

MIStream & MIStream::operator >>(double & d)
{
	::scanf("%lf", &d);
	return *this;
}

MIStream & MIStream::operator >>(void(*fp)(MIStream & rcin))
{
	fp(*this);
	return *this;
}

void hex(MIStream & rcin)
{
	rcin.SetFormat("%x");
}

void dec(MIStream & rcin)
{
	rcin.SetFormat("%d");
}

void oct(MIStream & rcin)
{
	rcin.SetFormat("%o");
}
