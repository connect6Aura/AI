#pragma once
class MIStream
{
	const char * format;
public:
	void SetFormat(const char * f);
	MIStream();
	~MIStream();
	MIStream & operator >>(int & n);
	MIStream & operator >>(char & c);
	MIStream & operator >>(char * str);
	MIStream & operator >>(double & d);
	MIStream & operator >>(void ( * fp)(MIStream & rcin));

	friend void hex(MIStream & rcin);
	friend void dec(MIStream & rcin);
	friend void oct(MIStream & rcin);
};

extern MIStream cin;

