#include <iostream>
#include <thread>
#include <chrono>
#include "GPIO/GPIOInterface.hpp"
#include "GPIO/SoftPWMControl.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    GPIOInterface blueLights("17", GPIO_DIR_OUT);
    GPIOInterface redLights("22", GPIO_DIR_OUT);
    GPIOInterface greenLights("27", GPIO_DIR_OUT);
    SoftPWMControl bluePWM(&blueLights);
    SoftPWMControl redPWM(&redLights);
    SoftPWMControl greenPWM(&greenLights);
    bluePWM.setDutyCycle(0);
    redPWM.setDutyCycle(0);
    greenPWM.setDutyCycle(0);
    bluePWM.init();
    redPWM.init();
    greenPWM.init();
    for (int i = 0; i < 100; ++i) {
        redPWM.setDutyCycle(float(i / 100.0));
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    for (int i = 0; i < 100; ++i) {
        redPWM.setDutyCycle(float((100.0 - i) / 100.0));
        greenPWM.setDutyCycle(float(i / 100.0));
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    for (int i = 0; i < 100; ++i) {
        greenPWM.setDutyCycle(float((100.0 - i) / 100.0));
        bluePWM.setDutyCycle(float(i / 100.0));
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    greenPWM.stop();
    bluePWM.stop();
    redPWM.stop();
    auto startBench = std::chrono::high_resolution_clock::now();
    redLights.setPinValue(1);
    auto midBench = std::chrono::high_resolution_clock::now();
    redLights.setPinValue(0);
    auto endBench = std::chrono::high_resolution_clock::now();
    std::cout << "Pin on time: " << (midBench - startBench).count() << "ns" << std::endl;
    std::cout << "Pin off time: " << (endBench - midBench).count() << "ns" << std::endl;
    std::cout << "Total pin cycle time: " << (endBench - startBench).count() << "ns" << std::endl;
    return 0;
}