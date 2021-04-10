#include "background.h"

Background::Background(uint id)
    : id(id), control(id)
{

}

uint Background::flag() const
{
    return 1 << id;
}
