
#include "Global.H"
#include "MString.h"

MString::MString()
{
	m_str = "";
}

MString::MString(const char * str)
{
	size_t len;
	len = ::StrLen(str);
	if (len == 0) m_str = "";
	else{
		len++;
		m_str = new char[len];
		::StrCpy(m_str, len, str);
	}
}

MString::~MString()
{
	if (*m_str != '\0') delete[] m_str;
}

const char * MString::GetString(void) const
{
	return m_str;
}

MString & MString::operator =(const MString & rstr)
{
	size_t len;

	if (this == &rstr) return *this;

	if (*m_str != '\0') delete[] m_str;

	len = ::StrLen(rstr.m_str);
	if (len == 0) m_str = "";
	else{
		len++;
		m_str = new char[len];
		::StrCpy(m_str, len, rstr.m_str);
	}
	return *this;
}

MString & MString::operator =(const char * str)
{
	size_t len;

	if (*m_str != '\0') delete[] m_str;

	len = ::StrLen(str);
	if (len == 0) m_str = "";
	else{
		len++;
		m_str = new char[len];
		::StrCpy(m_str, len, str);
	}
	return *this;
}

MString::MString(const MString & rstr)
{
	size_t len;
	len = ::StrLen(rstr.m_str);
	if (len == 0) m_str = "";
	else{
		len++;
		m_str = new char[len];
		::StrCpy(m_str, len, rstr.m_str);
	}
}

// cc = aa + bb;
MString MString::operator +(const MString & rstr) const
{
	MString temp;
	size_t len;
	len = ::StrLen(m_str) + ::StrLen(rstr.m_str);
	if (len == 0) return temp;
	else{
		len++;
		temp.m_str = new char[len];
		::StrCpy(temp.m_str, len, m_str);
		::StrCat(temp.m_str, len, rstr.m_str);
	}
	return temp;
}

// cc = aa + "samsung";
MString MString::operator +(const char * str) const
{
	MString temp;
	size_t len;
	len = ::StrLen(m_str) + ::StrLen(str);
	if (len == 0) return temp;
	else{
		len++;
		temp.m_str = new char[len];
		::StrCpy(temp.m_str, len, m_str);
		::StrCat(temp.m_str, len, str);
	}
	return temp;
}

// cc = "samsung" + bb;
MString operator +(const char * str, const MString & rstr)
{
	MString temp(str);
	return temp + rstr;
}

MString::operator const char *(void) const
{
	return m_str;
}

bool MString::operator ==(const MString & rstr) const
{
	return (bool)(!::StrCmp(*this, rstr));
}

bool MString::operator ==(const char * str) const
{
	return (bool)(!::StrCmp(*this, str));
}

bool operator ==(const char * str, const MString & rstr)
{
	return (bool)(!::StrCmp(str, rstr));
}

bool MString::operator !=(const MString & rstr) const
{
	return (bool)(::StrCmp(m_str, rstr));
}

bool MString::operator !=(const char * str) const
{
	return (bool)(::StrCmp(m_str, str));
}

bool operator !=(const char * str, const MString & rstr)
{
	return (bool)(::StrCmp(str, rstr));
}

char & MString::operator [](int idx) const
{
	return m_str[idx];
}

char & MString::operator *(void) const
{
	return *m_str;
}

char * MString::operator +(int idx) const
{
	return m_str + idx;
}
