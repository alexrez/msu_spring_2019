// Простой сериализатор поддерживающий два типа: uint64_t и bool.
// Сериализовать в текстовый вид с разделением пробелом, bool сериализуется как true и false
// Deserializer реализуется аналогично Serializer, только принимает std::istream, а не std::ostream

// Ts... args expand to int E1, double E2, const char* E3
// &args... expands to &E1, &E2, &E3
// Us... pargs expand to int* E1, double* E2, const char** E3

// serializer.h
#pragma once

enum class Error{
    NoError,
    CorruptedArchive
};

class Serializer{
    static constexpr char Separator = ' ';
    std::ostream& out_;
public:
    explicit Serializer(std::ostream& out)
        : out_(out)
    {}

    template <class T>
    Error save(T& object){
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT... args){
        return process(args...);
    }
    
private:
    Error process(uint64_t val){
        out_ << val;
        return Error::NoError;
    }

    Error process(bool val){
        out_ << (val? "true" : "false");
        return Error::NoError;
    }

    template <class T, class... ArgsT>
    Error process(T val, ArgsT... args){
        if (process(val) == Error::NoError){
            out_ << Separator;
            return process(args...);
        }
        else       
            return Error::CorruptedArchive;
    }
};


class Deserializer{
    std::istream& in_;
public:
    explicit Deserializer(std::istream& in)
        : in_(in)
    {}

    template <class T>
    Error load(T& object){
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args){
        return process(args...);
    }
    
private:
    Error process(uint64_t& val){
        std::string text;
        in_ >> text;
        std::string::iterator text_it = text.begin();
        if (text_it != text.end()){
            bool flag = isdigit(*text_it++);
            while(text_it != text.end() && flag){
                flag = isdigit(*text_it++);
            }
            if (flag){
                int64_t number = stoi(text);
                if (number >= 0)
                    val = number;
                else
                    return Error::CorruptedArchive;
            }
            else
                return Error::CorruptedArchive;
        }
        else
            return Error::CorruptedArchive;
        return Error::NoError;
    }

    Error process(bool& val){
        std::string text;
        in_ >> text;
        if (text == "true")
            val = true;
        else if (text == "false")
            val = false;
        else
            return Error::CorruptedArchive;

        return Error::NoError;
    }

    template <class T, class... ArgsT>
    Error process(T&& val, ArgsT&&... args){
        if (process(std::forward<T>(val)) != Error::NoError)
            return Error::CorruptedArchive;
        else
            return process(std::forward<ArgsT>(args)...);
    }
};
