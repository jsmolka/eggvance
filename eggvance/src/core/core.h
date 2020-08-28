#pragma once

namespace core
{

void init(int argc, char* argv[]);

void reset();
void frame();

void updateTitle();
void updateTitle(double fps);

};
