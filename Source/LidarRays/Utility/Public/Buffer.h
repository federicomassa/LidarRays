#pragma once

#include <vector>
#include <stdexcept>

template <class T>
class Buffer {
	int size;
	std::vector<T> data;

public:
	Buffer(int size = 1);
	void insert(T element);

	// Peek element at index, without popping the buffer
	T peek(int index) const;

	T pop();
	int capacity() const;
	void setSize(int size);
	void empty();
	bool isEmpty() const;
	bool isFull() const;
	typename std::vector<T>::iterator begin();
	typename std::vector<T>::iterator end();
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
void Buffer<T>::empty()
{
	data.clear();
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

template <class T>
bool Buffer<T>::isFull() const
{
	return (data.size() == size);
}

template <class T>
typename std::vector<T>::iterator Buffer<T>::begin()
{
	return data.begin();
}

template <class T>
typename std::vector<T>::iterator Buffer<T>::end()
{
	return data.end();
}

template <class T>
T Buffer<T>::peek(int index) const
{
	if (index < 0 || index >= size)
		throw std::out_of_range("Buffer<T>::peek --- out of range");
	
	return data[index];
}