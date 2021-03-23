#pragma once

#include "AP_HAL_Linux.h"

namespace Linux {

class DigitalSource : public AP_HAL::DigitalSource {
public:
    DigitalSource(uint8_t v);
    void    mode(uint8_t output) override;
    uint8_t read() override;
    void    write(uint8_t value) override;
    void    toggle() override;
private:
    uint8_t _v;

};

}

