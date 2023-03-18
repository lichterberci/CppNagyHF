#pragma once

#include <memory>
#include <iostream>

namespace cstd {


	template<typename T>
	class Vector {

	private:
		T* m_data;
		size_t m_capacity;
		size_t m_size;

	public:
		Vector()
			: m_size(0), m_capacity(0), m_data(nullptr)
		{
		}

		Vector(const size_t capacity)
			: m_size(0), m_capacity(0)
		{
			reserve(capacity);
		}

		Vector(const T* data, const size_t size)
			: m_size(size), m_capacity(size)
		{
			m_data = new T[m_capacity];
			memcpy(m_data, data, size);
		}

		Vector(const Vector& other)
			: m_capacity(other.m_capacity), m_size(other.m_size)
		{
			m_data = new T[other.m_capacity];
			memcpy(m_data, other.m_data, m_size * sizeof(T));
		}

		Vector(Vector&& other) noexcept
			: m_capacity(other.m_capacity), m_size(other.m_size)
		{
			m_data = other.m_data;

			other.m_capacity = 0;
			other.m_size = 0;
			other.m_data = nullptr;
		}

		~Vector() {
			delete[] m_data;
		}

		size_t size() const {
			return m_size;
		}

		size_t capacity() const {
			return m_capacity;
		}

		void reserve(const size_t new_capacity) {

			if (new_capacity <= m_capacity)
				return;

			m_capacity = new_capacity;

			T* new_data = new T[m_capacity];

			memcpy(new_data, m_data, m_size * sizeof(T));

			delete[] m_data;

			m_data = new_data;
		}

		void push(const T& item) {
			if (m_capacity > m_size) {
				m_data[m_size++] = item;
				return;
			}

			if (m_capacity == 0)
				reserve(1);
			else if (m_capacity <= m_size)
				reserve(m_capacity * 2);

			m_data[m_size++] = item;
		}

		void push(T&& item) {
			push(item);
		}

		void pushToFront(const T& item) {

			if (m_capacity > m_size) {

				memcpy(m_data + 1, m_data, m_size * sizeof(T));

				m_data[0] = item;
				m_size++;

				return;
			}
			
			if (m_capacity == 0)
				m_capacity = 1;
			else if (m_capacity <= m_size)
				m_capacity = m_capacity * 2;

			T* new_data = new T[m_capacity];

			memcpy(new_data + 1, m_data, m_size * sizeof(T));

			delete[] m_data;

			m_data = new_data;

			m_data[0] = item;

			m_size++;
		}

		void pushToFront(T&& item) {
			pushToFront(std::move(item));
		}

		void insert(const T& item, const size_t index) {
			if (index > m_size)
				throw std::out_of_range("Could not insert! (index out of range)");

			if (index == m_size) {
				push_back(item);
				return;
			}

			if (index == 0) {
				push_front(item);
				return;
			}

			if (m_capacity > m_size) {
				memcpy(m_data + index + 1, m_data + index, (m_size - index) * sizeof(T));
				m_data[index] = item;
				m_size++;
				return;
			}

			if (m_capacity == 0)
				m_capacity = 1;
			else if (m_capacity == m_size)
				m_capacity *= 2;

			T* new_data = new T[m_capacity];

			memcpy(new_data, m_data, index * sizeof(T));
			new_data[index] = item;
			memcpy(new_data + index + 1, m_data + index, (m_size - index) * sizeof(T));

			delete[] m_data;

			m_data = new_data;

			m_size++;
		}

		void insert(T&& item, const size_t index) {
			insert(std::move(item), index);
		}

		T removeAt(const size_t index) {
			if (index >= m_size)
				throw std::out_of_range("Remove could not be executed, index is out of range!");

			T result = m_data[index];

			memcpy(m_data + index, m_data + index + 1, (m_size - index) * sizeof(T));

			m_size--;

			return result;
		}

		T pop() {
			return m_data[--m_size];
		}

		T popFront() {
			T result = m_data[0];

			memcpy(m_data, m_data + 1, m_size * sizeof(T));
			m_size--;

			return result;
		}

		T& operator[] (const size_t index) const {
			if (index >= m_size)
				throw std::out_of_range("Index is out of range!");

			return m_data[index];
		}

		Vector& operator= (const Vector& other) {
			
			if (m_size > 0)
				delete[] m_data;

			m_size = other.m_size;
			m_capacity = other.m_capacity;
			m_data = new T[m_capacity];
			memcpy(m_data, other.m_data, m_size * sizeof(T));

			return *this;
		}

		Vector& operator= (Vector&& other) noexcept {

			if (m_size > 0)
				delete[] m_data;

			m_size = other.m_size;
			m_capacity = other.m_capacity;
			m_data = new T[m_capacity];
			memcpy(m_data, other.m_data, m_size * sizeof(T));

			return *this;
		}

		Vector& operator+= (const T& rhs) {
			this->push(rhs);
			return *this;
		}

		Vector& operator+= (T&& rhs) {
			this->push(rhs);
			return *this;
		}

		Vector& operator+ (const T& rhs) {
			this->operator+=(rhs);
			return *this;
		}

		Vector& operator+ (T&& rhs) {
			this->operator+=(rhs);
			return *this;
		}

		friend std::ostream& operator<< (std::ostream& os, const Vector<T>& v) {
			os << "Vector<" << typeid(T).name() << ">(size=" << v.m_size << ", capacity=" << v.m_capacity << ")";
			return os;
		}

		const T* begin() const noexcept {
			return m_data;
		}

		T* begin() noexcept {
			return m_data;
		}

		const T* end() const noexcept {
			return m_data + m_size;
		}

		T* end() noexcept {
			return m_data + m_size;
		}
	};
}

