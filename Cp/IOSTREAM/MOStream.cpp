
#include <stdio.h>
#include "MOStream.h"

MOStream cout;

MOStream::MOStream()
{
	format = "%d";
}

MOStream::~MOStream()
{
}

void MOStream::SetFormat(const char * f)
{
	format = f;
}

MOStream & MOStream::operator <<(int n)
{
	::printf(format, n);
	return *this;
}

MOStream & MOStream::operator << (char c)
{
	::printf("%c", c);
	return *this;
}

MOStream & MOStream::operator <<(const char * str)
{
	::printf("%s", str);
	return *this;
}

MOStream & MOStream::operator <<(double d)
{
	::printf("%lf", d);
	return *this;
}

MOStream & MOStream::operator <<(const void * p)
{
	::printf("%p", p);
	return *this;
}

MOStream & MOStream::operator <<(void(*fp)(MOStream & rcout))
{
	fp(*this);
	return *this;
}

void hex(MOStream & rcout)
{
	rcout.SetFormat("%x");
}

void dec(MOStream & rcout)
{
	rcout.SetFormat("%d");
}

void oct(MOStream & rcout)
{
	rcout.SetFormat("%o");
}

void endl(MOStream & rcout)
{
	::printf("\n");
	::fflush(stdout);
}






