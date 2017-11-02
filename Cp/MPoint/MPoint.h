
#ifndef _MPOINT_H_
#define _MPOINT_H_

class MPoint{
	int x, y;
public:
	int & X(void){ return x; }
	int & Y(void){ return y; }
	MPoint();
	MPoint(int x, int y);
	explicit MPoint(int n);
	void PrintPos(void) const;
	MPoint operator +(const MPoint  & pt) const;
	MPoint operator +(int n) const;
	friend MPoint operator +(int n, const MPoint & pt);

	MPoint operator -(const MPoint  & pt) const;
	MPoint operator -(int n) const;

	MPoint operator ++(void);
	MPoint operator ++(int dummy);
	MPoint operator --(void);
	MPoint operator --(int dummy);

	MPoint & operator +=(const MPoint & pt);
	MPoint & operator +=(int n);
	MPoint & operator -=(const MPoint & pt);
	MPoint & operator -=(int n);

	friend ostream & operator <<(ostream & rcout, const MPoint & pt);
	friend istream & operator >>(istream & rcin, MPoint & pt);
};

#endif // _MPOINT_H_


