#pragma once

#include <memory>
#include <iostream>

namespace cstd {


	template<typename T>
	class Vector {

	private:
		size_t m_size;
		size_t m_capacity;
		T* m_data;

	public:
		Vector()
			: m_size(0), m_capacity(0), m_data(nullptr)
		{
		}

		Vector(const size_t capacity)
			: m_size(0), m_capacity(capacity)
		{
			m_data = new T[m_capacity];
		}

		Vector(const T* data, const size_t size)
			: m_size(size), m_capacity(size)
		{
			m_data = new T[m_capacity];

			for (size_t i = 0; i < m_size; i++)
				m_data[i] = data[i];
		}

		Vector(const Vector& other) noexcept
			: m_size(other.m_size), m_capacity(other.m_capacity)
		{
			m_data = new T[m_capacity];

			for (size_t i = 0; i < other.m_size; i++)
				m_data[i] = other.m_data[i];
		}

		Vector(Vector&& other) noexcept
			: m_size(other.m_size), m_capacity(other.m_capacity)
		{
			m_data = other.m_data;

			other.m_capacity = 0;
			other.m_size = 0;
			other.m_data = nullptr;
		}

		~Vector() {
			if (m_data == nullptr) return;

			delete[] m_data;
		}

		size_t size() const {
			return m_size;
		}

		size_t capacity() const {
			return m_capacity;
		}

		void reserve_and_copy(const size_t new_capacity) {

			if (new_capacity <= m_capacity)
				return;

			m_capacity = new_capacity;

			T* new_data = new T[m_capacity];

			if (m_data == nullptr) {
				m_data = new_data;
				return;
			}

			for (size_t i = 0; i < m_size; i++)
				new_data[i] = std::move(m_data[i]);

			delete[] m_data;

			m_data = new_data;
		}

		void push(const T& item) {
			if (m_capacity > m_size) {
				m_data[m_size++] = item;
				return;
			}

			if (m_capacity == 0)
				reserve_and_copy(1);
			else if (m_capacity <= m_size)
				reserve_and_copy(m_capacity * 2);

			m_data[m_size++] = item;
		}

		void push(T&& item) {
			push(item);
		}

		void pushToFront(const T& item) {

			if (m_capacity > m_size) {

				m_size++;

				for (size_t i = m_size - 1; i >= 1; i--)
					m_data[i] = std::move(m_data[i - 1]);

				m_data[0] = item;

				return;
			}
						
			if (m_capacity == 0)
				m_capacity = 1;
			else if (m_capacity <= m_size)
				m_capacity = m_capacity * 2;

			T* new_data = new T[m_capacity];

			m_size++;

			for (size_t i = m_size - 1; i >= 1; i--)
				new_data[i] = std::move(m_data[i - 1]);

			delete[] m_data;

			m_data = new_data;

			m_data[0] = item;
		}

		void pushToFront(T&& item) {
			if (m_capacity > m_size) {

				for (size_t i = m_size - 1; i >= 1; i--)
					m_data[i] = std::move(m_data[i - 1]);

				m_data[0] = std::move(item);
				m_size++;

				return;
			}

			size_t prev_capacity = m_capacity;

			if (m_capacity == 0)
				m_capacity = 1;
			else if (m_capacity <= m_size)
				m_capacity = m_capacity * 2;

			T* new_data = new T[m_capacity];

			for (size_t i = m_size - 1; i >= 1; i--)
				new_data[i] = std::move(m_data[i - 1]);

			delete[] m_data;

			m_data = new_data;

			m_data[0] = std::move(item);

			m_size++;
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
				
				for (int i = m_size - 1; i > index; i--)
					m_data[i] = std::move(m_data[i - 1]);

				m_data[index] = item;
				m_size++;
				return;
			}

			if (m_capacity == 0)
				m_capacity = 1;
			else if (m_capacity == m_size)
				m_capacity *= 2;

			T* new_data = new T[m_capacity];

			for (size_t i = 0; i < m_size; i++) {
				if (i < index)
					new_data[i] = std::move(m_data[i]);
				else if (i == index)
					new_data[i] = item;
				else
					new_data[i + 1] = std::move(m_data[i]);
			}			

			delete[] m_data;

			m_data = new_data;

			m_size++;
		}

		void insert(T&& item, const size_t index) {
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

				for (int i = m_size - 1; i > index; i--)
					m_data[i] = std::move(m_data[i - 1]);

				m_data[index] = std::move(item);
				m_size++;
				return;
			}

			if (m_capacity == 0)
				m_capacity = 1;
			else if (m_capacity == m_size)
				m_capacity *= 2;

			T* new_data = new T[m_capacity];

			for (size_t i = 0; i < m_size; i++) {
				if (i < index)
					new_data[i] = std::move(m_data[i]);
				else if (i == index)
					new_data[i] = std::move(item);
				else
					new_data[i + 1] = std::move(m_data[i]);
			}

			delete[] m_data;

			m_data = new_data;

			m_size++;
		}

		T removeAt(const size_t index) {
			if (index >= m_size)
				throw std::out_of_range("Remove could not be executed, index is out of range!");

			T result = m_data[index];

			for (size_t i = index; i < (size_t)(m_size - 1); i++)
				m_data[i] = std::move(m_data[i + 1]);

			m_size--;

			return result;
		}

		T pop() {

			if (m_size == 0 || m_data == nullptr)
				throw std::out_of_range("Cannot pop from empty vector!");

			T result = m_data[--m_size];

			m_data[m_size] = T(); // this will call the destructor and leave the memory fresh for new data in the future

			return result;
		}

		T popFront() {

			if (m_size == 0 || m_data == nullptr)
				throw std::out_of_range("Cannot pop from empty vector!");

			T result = m_data[0];

			for (size_t i = 0; i < m_data - 1; i++)
				m_data[i] = std::move(m_data[i + 1]);

			m_size--;

			return result;
		}

		T& operator[] (const size_t index) {
			if (index >= m_size)
				throw std::out_of_range("Index is out of range!");

			return m_data[index];
		}

		const T& operator[] (const size_t index) const {
			if (index >= m_size)
				throw std::out_of_range("Index is out of range!");

			return m_data[index];
		}

		T& last() {
			if (m_size == 0)
				throw std::out_of_range("Cannot get last element of empty vector!");

			return m_data[m_size - 1];
		}
		
		const T& last() const {
			if (m_size == 0)
				throw std::out_of_range("Cannot get last element of empty vector!");

			return m_data[m_size - 1];
		}

		Vector& operator= (const Vector& other) {
			
			if (this == &other) return *this;

			if (m_data != nullptr)
				delete[] m_data;

			m_size = other.m_size;
			m_capacity = other.m_capacity;

			m_data = new T[m_capacity];

			for (size_t i = 0; i < m_capacity; i++)
				m_data[i] = other.m_data[i];

			return *this;
		}

		Vector& operator= (Vector&& other) noexcept {

			if (m_data != nullptr)
				delete[] m_data;

			m_size = other.m_size;
			m_capacity = other.m_capacity;

			m_data = other.m_data;

			other.m_size = 0;
			other.m_capacity = 0;
			other.m_data = nullptr;

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

