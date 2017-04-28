//
// Created by prp on 5/19/16.
//

#ifndef GPIOINTERFACE_HPP
#define GPIOINTERFACE_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#define GPIO_SYSTEM_DIR "/sys/class/gpio/"

#define GPIO_DIR_OUT "out"
#define GPIO_DIR_IN "in"

class GPIOInterface {
protected:
    std::string pinID;
    std::fstream* pinIOStream = nullptr;
public:
    GPIOInterface(const std::string& pinID , const std::string& direction){
        this->pinID = pinID;
        this->enable();
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); //Delay for sysfs to finish setting up GPIO pin...
        this->setDirection(direction);
        this->initIOStream();
    }

    GPIOInterface(const std::string& pinID) : GPIOInterface(pinID, GPIO_DIR_IN){
        //Just add default value for GPIO direction to in.
    }

    ~GPIOInterface(){
        this->destroyIOStream();
        this->disable();
    }

    const std::string& getPinID(){
        return this->pinID;
    }

    bool enable(){
        std::string path = GPIO_SYSTEM_DIR "export";
        std::ofstream exportStream(path.c_str());
        std::cout << "Enabling GPIO pin #" << this->pinID << "..." << std::endl;
        if (!exportStream.is_open()){
            std::cout << "ERROR: Failed to enable GPIO pin #" << this->pinID << std::endl;
            return false;
        }
        exportStream << this->pinID;
        exportStream.close();
        std::cout << "Done." << std::endl;
        return true;
    }

    bool disable() {
        std::string path = GPIO_SYSTEM_DIR "unexport";
        std::ofstream unexportStream(path.c_str());
        std::cout << "Disabling GPIO pin #" << this->pinID << "..." << std::endl;
        if (!unexportStream.is_open()){
            std::cout << "ERROR: Failed to disable GPIO pin #" << this->pinID << std::endl;
            return false;
        }
        unexportStream << this->pinID;
        unexportStream.close();
        std::cout << "Done." << std::endl;
        return true;
    }

    bool destroyIOStream(){
        std::cout << "Closing I/O streams for GPIO pin #" << this->pinID << "..." << std::endl;
        if(this->pinIOStream){
            this->pinIOStream->close();
            delete this->pinIOStream;
        }
        std::cout << "Done." << std::endl;
        return true;
    }

    bool initIOStream(){
        std::string path = GPIO_SYSTEM_DIR "gpio" + this->pinID + "/value";
        std::cout << "Initializing I/O streams for GPIO pin #" << this->pinID << "..." << std::endl;
        this->pinIOStream = new std::fstream(path.c_str(), std::ios::out | std::ios::in);
        if (!this->pinIOStream || !this->pinIOStream->is_open()){
            std::cout << "ERROR: Failed to initialize I/O stream for GPIO pin #" << this->pinID << std::endl;
            return false;
        }
        std::cout << "Done." << std::endl;
        return true;
    }

    bool setDirection(const std::string& direction){
        std::string path = GPIO_SYSTEM_DIR "gpio" + this->pinID + "/direction";
        //GPIOInterface::WaitForFile(path);
        std::ofstream directionSetter(path.c_str());
        std::cout << "Setting GPIO pin #" << this->pinID << " direction..." << std::endl;
        if (!directionSetter.is_open()){
            std::cout << "ERROR: Failed to set direction of GPIO pin #" << this->pinID << std::endl;
            return false;
        }
        if (direction == GPIO_DIR_OUT){
            directionSetter << GPIO_DIR_OUT;
        } else {
            directionSetter << GPIO_DIR_IN;
        }
        directionSetter.close();
        std::cout << "Done." << std::endl;
        return true;
    }

    int getPinValue(){
        if (!this->pinIOStream) {
            return -1;
        }
        int pinValue;
        *(this->pinIOStream) >> pinValue;
        this->pinIOStream->flush();
        return pinValue;
    }

    bool setPinValue(const int& value){
        if (!this->pinIOStream){
            return false;
        }
        *(this->pinIOStream) << value;
        this->pinIOStream->flush();
        return true;
    }
};


#endif //GPIOINTERFACE_HPP
