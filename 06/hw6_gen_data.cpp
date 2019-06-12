#include <iostream>
#include <fstream>
#include <random>

template <size_t MaxNum>
uint64_t getNumber()
{
    static std::random_device rd;
    static std::uniform_int_distribution<uint64_t> dist(0, MaxNum);
    return dist(rd);
}



int main()
{
    const char* fileName = "file_to_sort.bin";
    std::ofstream out(fileName, std::ios::binary);
    if (!out)
    {
        std::cerr << "Can't open " << fileName << '\n';
        return 1;
    }

    const size_t ExprNum = 1900000;
    const size_t MaxNumber = std::numeric_limits<uint64_t>::max();;

    for (size_t i = 0; i < ExprNum; ++i)
    {
        // uint64_t tmp = ExprNum - i;
        uint64_t tmp = getNumber<MaxNumber>();
        out.write((char *)&tmp, sizeof(tmp)); 
    }

    return 0;
}
