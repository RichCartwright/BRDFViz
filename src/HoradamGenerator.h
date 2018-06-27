#pragma once

#include <complex>

template <class T>
class HoradamGenerator
{
private:

	std::complex<T> m_a;
	std::complex<T> m_b;
	std::complex<T> m_p;
	std::complex<T> m_q;
	int count;
	T bound;

public:
	HoradamGenerator();
	HoradamGenerator(std::complex<T> a, std::complex<T> b, std::complex<T> p, std::complex<T> q);
	~HoradamGenerator();

	void InitHoradamGenerator(std::complex<T> a, std::complex<T> b, std::complex<T> p, std::complex<T> q);

	std::complex<T> GetNext(int N);
	T GetNextAngle(int N);
	T GetNextRadius(int N);

	int itNext = 0;

	inline float GetBound() { return bound; }
};

template <class T>
HoradamGenerator<T>::HoradamGenerator()
{
}

template <class T>
HoradamGenerator<T>::HoradamGenerator(std::complex<T> a, std::complex<T> b, std::complex<T> p, std::complex<T> q)
{
	InitHoradamGenerator(a, b, p, q);
}

template <class T>
void HoradamGenerator<T>::InitHoradamGenerator(std::complex<T> a, std::complex<T> b, std::complex<T> p, std::complex<T> q)
{
	m_p = p;//z1 + z2;
	m_q = q;// z1 * z2;

	m_a = a;// std::complex<double>(2.0, 2.0 / 3.0);
	m_b = b;// std::complex<double>(3.0, 1.0);

	bound = abs(a)+abs(b);

	count = 0;
}

template <class T>
HoradamGenerator<T>::~HoradamGenerator()
{
}

template <class T>
std::complex<T> HoradamGenerator<T>::GetNext(int N)
{
	std::complex<T> next;
	for (int i = 0; i < N; i++)
	{
		next = m_p*m_b - m_q*m_a;
		m_a = m_b;
		m_b = next;
	}
	return next;
}

/*
std::complex<T> HoradamGenerator<T>::GetNext(int N)
{
	std::complex<T> next;
	
	next = m_a*std::pow(m_p, ++count) + m_b*std::pow(m_q, count);

	return next;
}*/

template <class T>
T HoradamGenerator<T>::GetNextAngle(int N)
{
	std::complex<T> next = GetNext(N);

	return std::arg(next);
}

template <class T>
T HoradamGenerator<T>::GetNextRadius(int N)
{
	std::complex<T> next = GetNext(N);

	return std::norm(next);
}

