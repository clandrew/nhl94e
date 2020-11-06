#pragma once

template <typename T>
class Optional
{
	T m_value;
	bool m_hasValue;
public:
	Optional()
	{
		m_hasValue = true;
	}

	bool HasValue() const
	{
		return m_hasValue;
	}

	T const& Value() const
	{
		assert(m_hasValue);
		return m_value;
	}

	void operator= (const T& v) 
	{
		m_value = v;
		m_hasValue = true;
	}
};