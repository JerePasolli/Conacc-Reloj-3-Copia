#include "../include/statemanager.hpp"

void 
StateManager::enterClickedHandler(GtkWidget* widget, gpointer userData)
{
    StateManager* sm = static_cast<StateManager*>(userData);
    if (sm) {
        sm->handleEvent(StateManager::EventType::EnterClicked);
    }
}

void 
StateManager::checkInClickedHandler(GtkWidget* widget, gpointer userData)
{
    StateManager* sm = static_cast<StateManager*>(userData);
    if (sm) {
        sm->handleEvent(StateManager::EventType::CheckIn);
    }
}

// Método para cambiar de estado
void 
StateManager::changeState(BaseWindow* newState) {
    this->previousInterface = this->currentInterface;
    this->currentInterface = newState;
    this->currentInterface->show();
    this->previousInterface->hide();
}

// Método para manejar eventos
void
StateManager:: handleEvent(EventType eventType) {
    auto callback = callbacks.find(eventType);
    if (callback != callbacks.end()) {
        callback->second();
    }
}

// Asocia un tipo de evento con su respectiva función de callback
void
StateManager::registerCallback(EventType eventType, StateChangeCallback callback) {
    callbacks[eventType] = callback;
}