// Используя метод рекурсивного спуска, написать калькулятор. Поддерживаемые операции:
// - умножение
// - деление
// - сложение
// - вычитание
// - унарный минус
// Для вычислений использовать тип int64_t, приоритет операций стандартный.
// Передача выражения осуществляется через аргумент командной строки, поступаемые числа целые, результат выводится в cout. 
// Должна быть обработка ошибок, в случае некорректного выражения выводить в консоль error

#include<iostream>
#include<exception>

class Calc{
    std::string s;
    std::string::iterator s_it;
    char c;

public:
    Calc(const char *expr): s(expr){
        s_it = s.begin();
        c = '0';}
    int64_t calculate();
    int64_t summand();
    int64_t multiplier();
    char get_c();
    ~Calc(){
    }
};

int64_t Calc::calculate(){
    int64_t result = summand();
    while(c == '+' || c == '-'){
        if (c == '+')
            result += summand();
        else
            result -= summand();
    }
    return result;
}

int64_t Calc::summand(){
    int64_t result = multiplier();
    while(c == '*' || c == '/'){
        if (c == '*')
            result *= multiplier();
        else {
            int64_t divider = multiplier();
            if(divider)
                result /= divider;
            else
                throw std::exception();
        }
    }
    return result;
}

int64_t Calc::multiplier(){
    int64_t number = 0;
    int64_t i = 1;
    bool sign = false;
    bool digit = false;
    c = get_c();
    if(c == '-'){
        sign = true;
        c = get_c();
    }
    while(isdigit(c)){
        digit = true;
        number = number * i + (c - '0');
        i *= 10;
        c = get_c();
    }
    if(! digit)
        throw std::exception();
    if(sign)
        number *= -1;
    return number;
}

char Calc::get_c(){

    while (s_it != s.end() && isspace(*s_it))
        ++s_it;
    if(s_it != s.end()){
        if(! isdigit(*s_it) && *s_it != '+'&& *s_it != '-'
            && *s_it != '*' && *s_it != '/')
            throw std::exception();
        return *s_it++;
    }
    return EOF;
}


int main(int argc, char* argv[]){
    try{
        if(argc != 2)
            throw std::exception();
        int64_t result = 0;
        Calc expression(argv[1]);
        result = expression.calculate();
        std::cout << result << std::endl;
    }
    catch (std::exception &err){
        std::cout << "error" << std::endl;
        return 1;
    }

    return 0;
}

// calc "2 + 3 * 4 - -2"