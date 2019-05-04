#pragma once

#include "common/field.h"

template<typename T>
using Register = Field<T, 0, 8 * sizeof(T)>;
