#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

private:
    std::condition_variable _conditionMessageQueue;
    std::mutex _mutexMessageQueue;
    std::deque<T> _queue;    
};


class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    // getters / setters

    // typical behaviour methods
    void waitForGreen();
    void simulate() override;
    enum TrafficLightPhase {red, green};
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _msgQueue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif