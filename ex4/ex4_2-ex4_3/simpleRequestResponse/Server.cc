#include <omnetpp.h>
#include "messages_m.h"

using namespace omnetpp;

class Server : public cSimpleModule {
protected:
    virtual void handleMessage(cMessage *msg) override {
        if (Request *req = dynamic_cast<Request *>(msg)) {
            // Simulate processing time
            double processingTime = truncnormal(0.001, 0.0002); // μ=1ms, σ=0.2ms
            if (processingTime < 0) processingTime = 0;         // Truncated at 0

            // Create a self-message for processing
            cMessage *processEvent = new cMessage("processEvent");
            processEvent->setContextPointer(req);

            // Schedule processing
            scheduleAt(simTime() + processingTime, processEvent);
        } else if (strcmp(msg->getName(), "processEvent") == 0) {

            // Retrieve the associated Request
            Request *req = (Request *)msg->getContextPointer();

            // Create a Response
            Response *resp = new Response("Response");

            if (uniform(0, 1) < 0.5) { // lowered to 50%
                resp->setStatusCode(OK);
                resp->setAddress(0, intuniform(0, 255));
                resp->setAddress(1, intuniform(0, 255));
                resp->setAddress(2, intuniform(0, 255));
                resp->setAddress(3, intuniform(0, 255));
            } else {
                resp->setStatusCode(ERROR);
                resp->setAddress(0, 255);
                resp->setAddress(1, 255);
                resp->setAddress(2, 255);
                resp->setAddress(3, 255);
            }

            // Send response back
            send(resp, "gate$o");

            // Clean up
            delete req;  // Delete the original Request
            delete msg;  // Delete the self-message
        } else {
            // Unexpected message
            delete msg;
        }
    }
};

Define_Module(Server);
