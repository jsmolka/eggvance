#include "inputdevice.h"

class NoInputDevice : public InputDevice
{
public:
    void init() override
    {
    
    }

    void deinit() override
    {
    
    }

    int state() override
    {
        return 0;
    }
};

std::shared_ptr<InputDevice> input_device = std::make_shared<NoInputDevice>();
