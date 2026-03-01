#ifndef HARDWARE_INTERFACE_H
#define HARDWARE_INTERFACE_H

#include <string>

// Stub do hardware real (ESP32/Arduino)
// No PC, apenas loga no console. No hardware real, controla LEDs e buzzer.
class HardwareInterface {
public:
    HardwareInterface();
    ~HardwareInterface();

    void Initialize();
    void Update();
    void Shutdown();

    // Controle de LED RGB
    void SetLED(int r, int g, int b);
    void SetLEDColor(const std::string& color); // "red", "green", "blue", "off"
    void BlinkLED(int r, int g, int b, int durationMs);

    // Buzzer
    void PlayBuzzer(int frequency, int durationMs);
    void PlayBuzzerPattern(const std::string& pattern); // "success", "error", "warning"

    // Estado
    bool IsHardwareConnected() const;

private:
    bool connected;
    int currentR, currentG, currentB;
};

#endif // HARDWARE_INTERFACE_H
