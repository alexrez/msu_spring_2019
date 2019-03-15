#include<iostream>
#include "numbers.dat"

// Генерация решета Эратосфена
void eratosphen(bool* array, size_t size){
    array[0] = false; array[1] = false;
    for (size_t i = 2; i < size; ++i)
    	array[i] = true;

    for (size_t i = 2; i*i < size; ++i){
        if (array[i]){
            for (size_t j = i*i; j < size; j += i)
                array[j] = false;
        }
    }
}

//Поиск границ отрезка
int indexSearch(int number, int position, const int* Data, const int Size){
    for(int i = position; i < Size; ++i){
        if(number == Data[i]){
            return i;
        }
        else if (number < Data[i])
            break;
    }
    return -1; //нет заданного числа в массиве Data
}

//Подсчет числа простых чисел в отрезке
int counter(int in, int out, const int* Data, const int Size, bool* primes, const size_t primesSize){
    int count = 0;
    int i = in;
    for( ; i <= out; ++i){
        int number = Data[i];
        if (primes[number])
            ++count;
    }
    while(Data[i] == Data[i-1]){
        if (primes[Data[i]])
            ++count;
        ++i;
    }

    return count;
}

int main(int argc, char* argv[])
{
    if(argc == 1 || argc % 2 == 0){
        return -1;
    }

    const size_t arraySize = 100001;
    bool *primeArray = new bool[arraySize]; //массив для решета Эратосфена
    eratosphen(primeArray, arraySize);

    for (int i = 1; i < argc; ++i)
    {
        int start = std::atoi(argv[i]);
        int stop = std::atoi(argv[++i]);
        int indexIn = indexSearch(start, 0, Data, Size);
        int indexOut = indexSearch(stop, indexIn, Data, Size);

        if(start > stop || indexIn == -1 || indexOut == -1){
            std::cout <<  0 << std::endl;
            continue;
        }
        int count = 0;
        count = counter(indexIn, indexOut, Data, Size, primeArray, arraySize);
        std::cout <<  count << std::endl;
    }

    delete[] primeArray;
    return 0;
}