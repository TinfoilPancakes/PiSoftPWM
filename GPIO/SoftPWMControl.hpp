//
// Created by prp on 5/21/16.
//

#ifndef SOFTPWMCONTROL_HPP
#define SOFTPWMCONTROL_HPP

#include <chrono>
#include <thread>
#include <mutex>
#include "GPIOInterface.hpp"

class SoftPWMControl {
protected:
    bool toContinue = true;
    std::mutex lock;
    std::thread pinControlThread;
    GPIOInterface* pin = nullptr;
    std::chrono::duration<long long, std::nano> period = std::chrono::milliseconds(8); //Defaults to 8ms period time.
    float dutyCycle = 0; //Defaults to fully off. ranges from 0.0 - 1.0

    static void Execute(SoftPWMControl* controller, std::mutex* m){
        bool shouldContinue = controller->toContinue;
        while (shouldContinue) {
            m->lock();
            controller->pin->setPinValue(1);
            m->unlock();
            std::this_thread::sleep_for(controller->period * controller->dutyCycle);
            m->lock();
            controller->pin->setPinValue(0);
            m->unlock();
            std::this_thread::sleep_for(controller->period - (controller->period * controller->dutyCycle));
            m->lock();
            shouldContinue = controller->toContinue;
            m->unlock();
        }
    }

    void launchThread(){
        this->pinControlThread = std::thread(SoftPWMControl::Execute, this, &this->lock);
    }

    void joinThread(){
        this->lock.lock();
        this->toContinue = false;
        this->lock.unlock();
        this->pinControlThread.join();
    }

public:
    SoftPWMControl(GPIOInterface* pin) {
        this->pin = pin;
    }

    SoftPWMControl(GPIOInterface* pin, const float& dutyCycle) : SoftPWMControl(pin) {
        this->dutyCycle = dutyCycle;
    }

    SoftPWMControl(GPIOInterface* pin, const float& dutyCycle, const std::chrono::nanoseconds& period) : SoftPWMControl(pin, dutyCycle) {
        this->period = period;
    }

    void init(){
        this->launchThread();
    }

    void stop(){
        this->joinThread();
    }

    void setDutyCycle(const float& dutyCycle){
        this->lock.lock();
        this->dutyCycle = dutyCycle;
        this->lock.unlock();
    }

    void setPeriodLength(const std::chrono::nanoseconds& period){
        this->lock.lock();
        this->period = period;
        this->lock.unlock();
    }

    float getDutyCycle(){
        return this->dutyCycle;
    }
};


#endif //SOFTPWMCONTROL_HPP
