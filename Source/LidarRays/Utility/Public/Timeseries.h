#pragma once

#include <vector>
#include <utility>
#include <iostream>

template <class T>
class Timeseries {
	class iterator
	{
		typename std::vector < typename std::pair<double, T> >::iterator itr;
	public:
		double time();
		T value();
		iterator operator++();
		iterator operator++(int);
		bool operator ==(iterator i);
		bool operator !=(iterator i);

		friend class Timeseries<T>;
	};

	// Vector of pairs (time, value)
	std::vector<std::pair<double, T> > data;

public:
	Timeseries();
	void insert(double time, T element);
	int size();
	void clear();
	iterator begin();
	iterator end();
	iterator operator[](int);
};


template <class T>
double Timeseries<T>::iterator::time()
{
	return itr->first;
}

template <class T>
T Timeseries<T>::iterator::value()
{
	return itr->second;
}

template <class T>
typename Timeseries<T>::iterator Timeseries<T>::iterator::operator++()
{
	++itr;
	return *this;
}

template <class T>
typename Timeseries<T>::iterator Timeseries<T>::iterator::operator++(int)
{
	Timeseries<T>::iterator copy = *this;
	++itr;
	return copy;
}

template <class T>
typename bool Timeseries<T>::iterator::operator==(Timeseries<T>::iterator i)
{
	return (itr == i.itr);
}

template <class T>
typename bool Timeseries<T>::iterator::operator!=(Timeseries<T>::iterator i)
{
	return !(*this == i);
}

template <class T>
typename Timeseries<T>::iterator Timeseries<T>::begin()
{
	Timeseries<T>::iterator itr;
	itr.itr = data.begin();
	return itr;
}

template <class T>
typename Timeseries<T>::iterator Timeseries<T>::end()
{
	Timeseries<T>::iterator itr;
	itr.itr = data.end();
	return itr;
}

template <class T>
typename Timeseries<T>::iterator Timeseries<T>::operator[](int index)
{
	Timeseries<T>::iterator itr;
	itr.itr = data.begin() + index;
	return itr;
}


template <class T>
Timeseries<T>::Timeseries()
{
}

template <class T>
void Timeseries<T>::insert(double time, T element)
{
	std::pair<double, T> p;
	p.first = time;
	p.second = element;

	for (int i = data.size() - 1; i >= 0; i--)
	{
		if (time > data[i].first)
		{
			data.insert(data.begin() + i + 1, p);
			return;
		}
	}

	// if it gets here, it means that time was the smallest OR the vector was empty
	data.insert(data.begin(), p);
}

template <class T>
int Timeseries<T>::size()
{
	return data.size();
}

template <class T>
void Timeseries<T>::clear()
{
	return data.clear();
}