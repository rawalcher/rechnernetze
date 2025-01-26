#include <omnetpp.h>
#include "messages_m.h"
#include <string>

using namespace omnetpp;

class Client : public cSimpleModule {
private:
    int numRequests;       // Total number of requests to send
    int requestsSent = 0;  // Counter for requests sent
    double intReq;         // Mean interval between requests
    cMessage *sendRequestEvent; // Self-message to schedule next request

    std::string generateRandomHostname() {
        int length = uniform(5, 50); // Random length between 5 and 50
        std::string hostname;
        static const char charset[] =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (int i = 0; i < length; i++) {
            hostname += charset[intuniform(0, sizeof(charset) - 2)];
        }
        return hostname;
    }

protected:
    virtual void initialize() override {
        numRequests = par("n").intValue();
        intReq = par("intReq").doubleValue();
        intReq = intReq/1000;
        sendRequestEvent = new cMessage("sendRequestEvent");

        // Schedule the first request
        scheduleAt(simTime() + exponential(intReq), sendRequestEvent);
    }

    virtual void handleMessage(cMessage *msg) override {
        if (msg == sendRequestEvent) {
            if (requestsSent < numRequests) {
                // Create and send a Request
                Request *req = new Request("Request");
                req->setHostname(generateRandomHostname().c_str());
                send(req, "gate$o");

                requestsSent++;

                // Schedule the next request
                scheduleAt(simTime() + exponential(intReq), sendRequestEvent);
            } else {
                // All requests sent, stop scheduling
                delete sendRequestEvent;
                sendRequestEvent = nullptr;
            }
        } else if (Response *resp = dynamic_cast<Response *>(msg)) {
            // Handle incoming Response
            if (resp->getStatusCode() == 0) { // OK
                EV << "Received Response: OK, Address: "
                   << resp->getAddress(0) << "." << resp->getAddress(1) << "."
                   << resp->getAddress(2) << "." << resp->getAddress(3) << "\n";
            } else {
                EV << "Received Response: ERROR, Address: FF.FF.FF.FF\n";
            }
            delete resp;
        }
    }

    virtual void finish() override {
        if (sendRequestEvent)
            cancelAndDelete(sendRequestEvent);
    }
};

Define_Module(Client);
