#ifndef ERROR_HPP
#define ERROR_HPP

class my_error{
private:
    std::string error;
public:
    my_error(){}

    my_error(const std::string& _error){}

    my_error(const my_error& other){}

    ~my_error(){}

    std::string what(){}
}

#endif