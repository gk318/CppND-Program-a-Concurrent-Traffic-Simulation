#include <iostream>
#include <random>
#include <thread>

#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lck(this->_mutexMessageQueue);
    _conditionMessageQueue.wait(lck);
    auto new_msg = std::move(_queue.back());
    _queue.pop_back();
    return new_msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lck(this->_mutexMessageQueue);
    _queue.push_back(msg);
    _conditionMessageQueue.notify_one(); 
}


/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        if (_msgQueue.receive() ==  TrafficLightPhase::green)
            return;
    }
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called.
    threads.emplace_back(std::move(std::thread(&TrafficLight::cycleThroughPhases, this)));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 

    auto t1 = std::chrono::high_resolution_clock::now();
    float duration = 4 + (rand() % static_cast<int>(6 - 4 + 1));
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto t2 = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count() > duration*1000000)    
        {
            t1 = std::chrono::high_resolution_clock::now();
            this->_currentPhase = (this->_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
            _msgQueue.send(std::move(this->_currentPhase));
        }
    }
}