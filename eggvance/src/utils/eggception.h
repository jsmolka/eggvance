#pragma once

#include <string>
#include <exception>

class Eggception : public std::exception
{
public:
    explicit Eggception(const std::string& message);

    virtual const char* what() const throw();

private:
    std::string message;
};
