#pragma once
#include <iostream>
#include <vector>
#include "exception.hpp"
#include "algorithm"
#include "nonCopyable.hpp"

template<class EventInstance>
class EventListener {
public:
    virtual void onEvent(EventInstance& event) = 0;
};

template<class EventInstance>
class EventSender : public NonCopyable {
    std::vector<EventListener<EventInstance>*> listeners;
public:
    void addListener(EventListener<EventInstance>* listener) {
        listeners.push_back(listener);
    }
    
    void removeListener(EventListener<EventInstance>* listener) {
        auto pos = std::find(listeners.begin(), listeners.end(), listener);
        if(pos == listeners.end())
            throw Exception("Removed non-existing listener.");
        listeners.erase(pos);
    }
    
    void call(EventInstance& event) {
        for(int i = 0; i < listeners.size(); i++)
            listeners[i]->onEvent(event);
    }
    
    ~EventSender() {
        if(!listeners.empty())
            std::cout << "Warning: Event destructed with listeners!" << std::endl;
    }
};
