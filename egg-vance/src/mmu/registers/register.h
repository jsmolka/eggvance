#pragma once

#include "field.h"

template<typename T>
using Register = Field<T, 0, 8 * sizeof(T)>;
