#include <omnetpp.h>

using namespace omnetpp;

class Client : public cSimpleModule {
protected:
    virtual void initialize() override {
        // Initialisierungscode (leer)
    }

    virtual void handleMessage(cMessage *msg) override {
        // Nachrichtenbehandlungslogik (leer)
    }
};

Define_Module(Client);
