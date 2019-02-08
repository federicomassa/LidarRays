#pragma once

#include <vector>

template <class T>
class Buffer {
	int size;
	std::vector<T> data;

public:
	Buffer(int size = 1);
	void insert(T element);
	T pop();
	int capacity() const;
	void setSize(int size);
	bool isEmpty() const;
};

template <class T>
Buffer<T>::Buffer(int size)
{
	this->size = size;
}

template <class T>
int Buffer<T>::capacity() const
{
	return size;
}


template <class T>
void Buffer<T>::insert(T element)
{
	data.insert(data.begin(), element);

	if (data.size() > size)
		data.pop_back();
}


template <class T>
T Buffer<T>::pop()
{
	if (!isEmpty())
	{
		T last = data.at(data.size() - 1);
		data.pop_back();
		return last;
	}
	else
		throw std::out_of_range("Buffer::pop --- buffer is empty");

	return T();
}

template <class T>
void Buffer<T>::setSize(int size)
{
	this->size = size;
}

template <class T>
bool Buffer<T>::isEmpty() const
{
	return (data.size() == 0);
}
