//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
// (c) 2019 Christian Timmerer, Alpen-Adria-Universität Klagenfurt / Bitmovin Inc.
//          christian.timmerer@itec.aau.at / christian.timmerer@bitmovin.com
//
// 621.800 (19W) Computer Networks and Network Programming
//
// HTTPClient Implementation for Multi-Port Requests
//

#include <string>
#include <omnetpp.h>

#include "HTTPClient.h"
#include "HTTPClientMsg_m.h"
#include "HTTPServerMsg_m.h"
#include "../udp/UDPControlInfo_m.h"
#include "../3rdParty/IPv4Address.h"

Define_Module(HTTPClient);

void HTTPClient::initialize() {
    clientPort = par("clientPort").intValue(); // Usually 6666

    // Initialize IP addresses
    clientIP = inet::IPv4Address("192.168.1.2"); // Client IP
    serverIP = inet::IPv4Address("192.168.1.1"); // Server IP

    EV << "HTTPClient initialized on port " << clientPort << ". Waiting to send the first request." << endl;

    // Schedule the first request
    cMessage *startMsg = new cMessage("startRequest");
    scheduleAt(simTime(), startMsg);
}

void HTTPClient::handleMessage(cMessage *msg) {
    // Check if this is our trigger to start sending requests
    if (msg->isSelfMessage()) {
        // Send request to the current port
        sendRequest("/index.html", currentPort);

        // Schedule next request to a different port
        if (currentPort == 80) {
            currentPort = 81;
            cMessage *nextMsg = new cMessage("startRequest");
            scheduleAt(simTime() + 1.0, nextMsg);
        }
        else if (currentPort == 81) {
            currentPort = 82;
            cMessage *nextMsg = new cMessage("startRequest");
            scheduleAt(simTime() + 1.0, nextMsg);
        }

        delete msg;
    }
    // Handle server response
    else if (dynamic_cast<HTTPServerMsg*>(msg) != nullptr) {
        HTTPServerMsg *response = check_and_cast<HTTPServerMsg*>(msg);

        // Get source port from control info
        UDPControlInfo *controlInfo = check_and_cast<UDPControlInfo*>(response->getControlInfo());
        int srcPort = controlInfo->getSrcPort();

        // Log the response
        EV << "HTTPClient received response from server (port " << srcPort << "):" << endl;
        EV << "-------------------" << endl;
        EV << response->getResponse() << endl;
        EV << "-------------------" << endl;

        delete msg;
    }
    else {
        // Unknown message type
        EV << "Received unknown message type. Discarding." << endl;
        delete msg;
    }
}

void HTTPClient::sendRequest(const char *resource, int destinationPort) {
    HTTPClientMsg *msg = new HTTPClientMsg();
    msg->setMethod("GET");
    msg->setResource(resource);

    UDPControlInfo *controlInfo = new UDPControlInfo();
    controlInfo->setSrcPort(clientPort);
    controlInfo->setDestPort(destinationPort);

    // Add IP addresses
    controlInfo->setSrcIPv4(clientIP);
    controlInfo->setDestIPv4(serverIP);

    msg->setControlInfo(controlInfo);

    send(msg, "toLowerLayer");
    EV << "Request sent: Method=GET, Resource=" << msg->getResource() << ", DestPort=" << destinationPort << endl;
}