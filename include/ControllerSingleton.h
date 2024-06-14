// ControllerSingleton.h
#ifndef CONTROLLER_SINGLETON_H
#define CONTROLLER_SINGLETON_H

#include "Controller.h"

class ControllerSingleton {
public:
    static Controller& GetInstance() {
        static Controller instance;
        return instance;
    }

private:
    ControllerSingleton() {}
    ControllerSingleton(const ControllerSingleton&) = delete;
    void operator=(const ControllerSingleton&) = delete;
};

#endif // CONTROLLER_SINGLETON_H
