
#pragma once

class MString {
	char * m_str;
public:
	MString();
	explicit MString(const char * str);
	MString(const MString & rstr);
	~MString();
	const char * GetString(void) const;

	MString & operator =(const MString & rstr);
	MString & operator =(const char * str);

	MString operator +(const MString & rstr) const;
	MString operator +(const char * str) const;
	friend MString operator +(const char * str, const MString & rstr);

	operator const char *(void) const;

	bool operator ==(const MString & rstr) const;
	bool operator ==(const char * str) const;
	friend bool operator ==(const char * str, const MString & rstr);
	bool operator !=(const MString & rstr) const;
	bool operator !=(const char * str) const;
	friend bool operator !=(const char * str, const MString & rstr);

	char & operator [](int idx) const;
	char & operator *(void) const;
	char * operator +(int idx) const;
};



