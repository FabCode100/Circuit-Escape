#ifndef HARDWARE_INTERFACE_H
#define HARDWARE_INTERFACE_H

class HardwareInterface {
public:
    HardwareInterface();
    ~HardwareInterface();

    void Initialize();
    void Update();
    void Shutdown();

private:
    // Hardware interface implementation
};

#endif // HARDWARE_INTERFACE_H
