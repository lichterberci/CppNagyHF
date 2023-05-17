#pragma once

#include <iostream>

namespace cstd {

	template <typename T>
	class Matrix {
	private:
		T* m_data;
		size_t m_size_x, m_size_y;
	public:
		Matrix()
			: m_data(nullptr), m_size_x(0), m_size_y(0)
		{ }

		Matrix(const size_t size_x, const size_t size_y)
			: m_size_x(size_x), m_size_y(size_y)
		{
			m_data = new T[m_size_x * m_size_y];
		}

		Matrix(const size_t size_x, const size_t size_y, const T& value)
			: m_size_x(size_x), m_size_y(size_y)
		{
			m_data = new T[m_size_x * m_size_y];
			setAll(value);
		}

		Matrix(const Matrix& other)
			: m_size_x(other.m_size_x), m_size_y(other.m_size_y)
		{
			m_data = new T[m_size_x * m_size_y];
		}

		Matrix(Matrix&& other)
			: m_size_x(other.m_size_x), m_size_y(other.m_size_y)
		{
			m_data = other.m_data;
			other.m_data = nullptr;
		}

		~Matrix() {
			delete[] m_data;
		}

		size_t size() const {
			return m_size_x * m_size_y;
		}

		size_t sizeX() const {
			return m_size_x;
		}

		size_t sizeY() const {
			return m_size_y;
		}

		T& operator[] (const size_t index) const {

			if (index >= m_size_x * m_size_y)
				throw std::out_of_range("Index out of range!");

			return m_data[index];
		}

		T get(const size_t x, const size_t y) const {

			if (x > m_size_x || y > m_size_y)
				throw std::out_of_range("Index out of range!");

			return m_data[x * m_size_y + y];
		}

		T& get(const size_t x, const size_t y) {

			if (x > m_size_x || y > m_size_y)
				throw std::out_of_range("Index out of range!");

			return m_data[x * m_size_y + y];
		}

		void set(const size_t x, const size_t y, const T& value) {

			if (x > m_size_x || y > m_size_y)
				throw std::out_of_range("Index out of range!");

			m_data[x * m_size_y + y] = value;
		}

		void set(const size_t x, const size_t y, T&& value) {
			set(x, y, value);
		}

		void setAll(const T& value) {
			for (size_t i = 0; i < m_size_x; i++)
				for (size_t j = 0; j < m_size_y; j++)
					set(i, j, value);
		}

		void setAll(T&& value) {
			setAll(value);
		}

		void resize(const size_t new_x, const size_t new_y) {

			T* new_data = new T[new_x * new_y];

			for (size_t i = 0; i < std::min(new_x, m_size_x); i++)
				for (rsize_t j = 0; j < std::min(new_y, m_size_y); j++)
					new_data[i * new_y + j] = get(i, j);

			delete[] m_data;

			m_data = new_data;

			m_size_x = new_x;
			m_size_y = new_y;

			return;
		}

		void resize(const size_t new_x, const size_t new_y, const T& default_value) {

			T* new_data = new T[new_x * new_y];

			std::fill_n(new_data, new_x * new_y, default_value);

			for (size_t i = 0; i < std::min(new_x, m_size_x); i++)
				for (rsize_t j = 0; j < std::min(new_y, m_size_y); j++)
					new_data[i * new_y + j] = get(i, j);

			delete[] m_data;

			m_data = new_data;

			m_size_x = new_x;
			m_size_y = new_y;

			return;
		}

		void printToContole(const char* sep = ", ", const char* line_sep = "\n\r") const {
			for (size_t i = 0; i < m_size_x; i++)
			{
				for (size_t j = 0; j < m_size_y; j++)
				{
					std::cout << get(i, j);

					if (j < m_size_y - 1)
						std::cout << sep;
				}

				std::cout << line_sep;
			}
		}

		friend std::ostream& operator<< (std::ostream& os, const Matrix<T>& m) {
			os << "Matrix<" << typeid(T).name() << ">(" << m.sizeX() << ", " << m.sizeY() << ")";
			return os;
		}

		const T* begin() const {
			return m_data;
		}

		T* begin() {
			return m_data;
		}

		const T* end() const {
			return m_data + size();
		}

		T* end() {
			return m_data + size();
		}
	};
}