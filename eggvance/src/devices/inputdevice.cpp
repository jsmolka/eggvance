#include "inputdevice.h"

class NoInputDevice : public InputDevice
{
public:
    void init() override {}
    void deinit() override {}
    void poll(u16& state) override {}
};

std::shared_ptr<InputDevice> input_device = std::make_shared<NoInputDevice>();
