#pragma once

class Emulator
{
public:
    void reset();

    bool init(int argc, char* argv[]);
    
    void run();

private:
    void frame();
};
