#include "eggception.h"

Eggception::Eggception(const std::string& message)
    : message(message) 
{ 

}

const char* Eggception::what() const throw()
{
    return message.c_str();
}
