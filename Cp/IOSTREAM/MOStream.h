#pragma once
class MOStream
{
	const char * format;
public:
	MOStream();
	~MOStream();
	void SetFormat(const char * f);
	MOStream & operator <<(int n);
	MOStream & operator <<(char c);
	MOStream & operator <<(const char * str);
	MOStream & operator <<(double d);
	MOStream & operator <<(const void * p);
	MOStream & operator <<(void(*fp)(MOStream & rcout));

	friend void hex(MOStream & rcout);
	friend void dec(MOStream & rcout);
	friend void oct(MOStream & rcout);
	friend void endl(MOStream & rcout);
};

extern MOStream cout;
