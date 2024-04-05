#include "view.hpp"
#include <functional>  // Para std::function
#include <unordered_map>  // Para std::unordered_map

/*
** La clase StateManager se encarga de manejar las interfaces y ventanas cuando éstas
** deben cambiar. Por ejemplo, cambiar de ventana, hacer visible una ventana emergente,
** y cualquier funcionalidad que implique una interacción entre dos interfaces.
*/




class StateManager {
public:
    StateManager(BaseWindow* initialInterface) : currentInterface(initialInterface) {}

    using StateChangeCallback = std::function<void()>;

    // Define los tipos de eventos que pueden provocar un cambio de estado
    enum EventType {
        EnterClicked,
        CheckIn
    };

    // Asocia un tipo de evento con su respectiva función de callback
    void registerCallback(EventType eventType, StateChangeCallback callback);

    // Método para cambiar de estado
    void changeState(BaseWindow* newState);

    // Método para manejar eventos
    void handleEvent(EventType eventType);

    static void enterClickedHandler(GtkWidget* widget, gpointer userData);

    static void checkInClickedHandler(GtkWidget* widget, gpointer userData);

private:
    BaseWindow* currentInterface;
    BaseWindow* previousInterface;
    std::unordered_map<EventType, StateChangeCallback> callbacks;
};