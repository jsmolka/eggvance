#pragma once

class AudioContext
{
public:
    friend class Context;

private:
    void init();
    void deinit();
};
