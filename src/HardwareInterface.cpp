#include "HardwareInterface.h"
#include <iostream>

HardwareInterface::HardwareInterface()
    : connected(false), currentR(0), currentG(0), currentB(0) {
}

HardwareInterface::~HardwareInterface() {
    Shutdown();
}

void HardwareInterface::Initialize() {
    connected = false; // Stub - no hardware real, tentaria conectar via serial
    std::cout << "[HW] Hardware Interface initialized (STUB MODE)\n";
}

void HardwareInterface::Update() {
    // No hardware real: pollar estado dos botões, sensores, etc
}

void HardwareInterface::Shutdown() {
    if (connected) {
        SetLED(0, 0, 0); // Desliga LED
        connected = false;
    }
    std::cout << "[HW] Hardware Interface shutdown\n";
}

void HardwareInterface::SetLED(int r, int g, int b) {
    currentR = r;
    currentG = g;
    currentB = b;
    std::cout << "[HW] LED -> R:" << r << " G:" << g << " B:" << b << "\n";
}

void HardwareInterface::SetLEDColor(const std::string& color) {
    if (color == "red")        SetLED(255, 0, 0);
    else if (color == "green") SetLED(0, 255, 0);
    else if (color == "blue")  SetLED(0, 0, 255);
    else if (color == "yellow") SetLED(255, 255, 0);
    else if (color == "off")   SetLED(0, 0, 0);
    else std::cout << "[HW] Unknown color: " << color << "\n";
}

void HardwareInterface::BlinkLED(int r, int g, int b, int durationMs) {
    std::cout << "[HW] Blink LED R:" << r << " G:" << g << " B:" << b
              << " for " << durationMs << "ms\n";
}

void HardwareInterface::PlayBuzzer(int frequency, int durationMs) {
    std::cout << "[HW] Buzzer " << frequency << "Hz for " << durationMs << "ms\n";
}

void HardwareInterface::PlayBuzzerPattern(const std::string& pattern) {
    if (pattern == "success") {
        std::cout << "[HW] Buzzer Pattern: SUCCESS (ascending tones)\n";
        PlayBuzzer(440, 100);
        PlayBuzzer(880, 200);
    } else if (pattern == "error") {
        std::cout << "[HW] Buzzer Pattern: ERROR (descending tones)\n";
        PlayBuzzer(880, 100);
        PlayBuzzer(220, 300);
    } else if (pattern == "warning") {
        std::cout << "[HW] Buzzer Pattern: WARNING (repeated tone)\n";
        PlayBuzzer(660, 100);
        PlayBuzzer(660, 100);
    } else {
        std::cout << "[HW] Unknown buzzer pattern: " << pattern << "\n";
    }
}

bool HardwareInterface::IsHardwareConnected() const {
    return connected;
}
