// Нужно написать класс-матрицу, тип элементов int. В конструкторе задается количество рядов и строк.
// Поддерживаются оперции: получить количество строк(rows)/столбцов(columns), получить конкретный элемент, умножить на число(*=),
// сравнение на равенство/неравенство. В случае ошибки выхода за границы бросать исключение: throw std::out_of_range("")

// Для проверки на сайте класс должен быть оформлен как заголовочный файл, название класса - Matrix. Нужно скомпилировать файл test.cpp,
// в нем включается заголовочный файл matrix.h и тестируется. В случае успеха в выводе кроме строки done ничего больше не будет.

// Подсказка:
// Чтобы реализовать семантику [][] понадобится прокси-класс. Оператор матрицы возращает другой класс,
// в котором тоже используется оператор [] и уже этот класс возвращает значение. Нужно реализовать const и non-const версию.
// При этом проследите, чтобы константная версия не могла менять данные в матрице, это не так явно с прокси-классом.


#include<iostream>
#include<exception>

class Row{
    size_t columns;
    int* row_;
public:
    Row(int* const array, const size_t c)
    : columns(c), row_(array)
    {}

    const int operator[](const size_t) const;
    int& operator[](const size_t);
};

const int Row::operator[](const size_t j) const{
    if(j >= columns)
        throw std::out_of_range("");
    return row_[j];
}
int& Row::operator[](const size_t j){
    if(j >= columns)
        throw std::out_of_range("");
    return row_[j];
}

class Matrix{
    size_t rows;
    size_t columns;
    int* wholeMatrix;
public:
    Matrix(const size_t r, const size_t c)
    : rows(r), columns(c) {
        wholeMatrix = new int [rows * columns];
    }

    const size_t getRows() const;
    const size_t getColumns() const;
    const Row operator[](const size_t) const;
    Row operator[](const size_t);
    Matrix& operator*=(const int n);
    bool operator==(const Matrix&) const;
    bool operator!=(const Matrix&) const;

    ~Matrix(){
        delete[] wholeMatrix;
    }
};

const size_t Matrix::getRows() const{
    return rows;
}

const size_t Matrix::getColumns() const{
    return columns;
}

// m[][] --> r[] --> int
const Row Matrix::operator[](const size_t i) const{
    if(i >= rows)
        throw std::out_of_range("");
    return Row(wholeMatrix + (i * columns), columns);
}

Row Matrix::operator[](const size_t i){
    if(i >= rows)
        throw std::out_of_range("");
    return Row(wholeMatrix + (i * columns), columns);
}

Matrix& Matrix::operator*=(const int n){
    for(size_t i = 0; i < rows*columns; ++i)
        wholeMatrix[i] *= n;
    return *this;
}

bool Matrix::operator==(const Matrix& other) const{
    if(this == &other)
        return true;
    for(size_t i = 0; i < rows*columns; ++i)
        if(wholeMatrix[i] != other.wholeMatrix[i])
            return false;
    return true;
}

bool Matrix::operator!=(const Matrix& other) const{
    return !(*this == other);
}
