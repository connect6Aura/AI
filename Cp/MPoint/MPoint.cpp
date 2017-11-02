
#include <iostream>
using namespace std;
#include "MPoint.H"

MPoint::MPoint()
{
}

MPoint::MPoint(int x, int y)// :x(x), y(y)
{
	this->x = x, this->y = y;
}

MPoint::MPoint(int n)
{
	x = y = n;
}

void MPoint::PrintPos(void) const
{
	cout << x << " " << y;
}

MPoint MPoint::operator +(const MPoint  & pt) const
{
	MPoint temp(x + pt.x, y + pt.y);
	return temp;
}

MPoint MPoint::operator +(int n) const
{
	MPoint temp(x + n, y + n);
	return temp;
}

MPoint operator +(int n, const MPoint & pt)
{
	return pt + n;
}

MPoint MPoint::operator -(const MPoint  & pt) const
{
	MPoint temp(x - pt.x, y - pt.y);
	return temp;
}

MPoint MPoint::operator -(int n) const
{
	MPoint temp(x - n, y - n);
	return temp;
}

ostream & operator <<(ostream & rcout, const MPoint & pt)
{
	pt.PrintPos();
	return rcout;
}

istream & operator >>(istream & rcin, MPoint & pt)
{
	rcin >> pt.X() >> pt.Y();
	return rcin;
}

MPoint MPoint::operator ++(void)
{
	*this = *this + 1;
	return *this;
}

MPoint MPoint::operator ++(int dummy)
{
	*this = *this + 1;
	return (*this - 1);
}

MPoint MPoint::operator --(void)
{
	*this = *this - 1;
	return *this;
}

MPoint MPoint::operator --(int dummy)
{
	*this = *this - 1;
	return (*this + 1);
}

MPoint & MPoint::operator +=(const MPoint & pt)
{
	*this = *this + pt;
	return *this;
}

MPoint & MPoint::operator +=(int n)
{
	*this = *this + n;
	return *this;
}

MPoint & MPoint::operator -=(const MPoint & pt)
{
	*this = *this - pt;
	return *this;
}

MPoint & MPoint::operator -=(int n)
{
	*this = *this - n;
	return *this;
}
