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

#include "HTTPClient.h"
#include "HTTPClientMsg_m.h"
#include "HTTPServerMsg_m.h"
#include "../udp/UDPControlInfo_m.h"

Define_Module(HTTPClient);

void HTTPClient::initialize() {
    srcPort = par("srcPort").intValue();

    EV << "HTTPClient initialized on port " << srcPort << ". Waiting to send the first request.\n";

    cMessage *startMsg = new cMessage("startRequest");
    scheduleAt(simTime(), startMsg);
}

void HTTPClient::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage() && strcmp(msg->getName(), "startRequest") == 0) {
        sendRequest("/index.html",0);
        EV << "First request sent: Method=GET, Resource=/index.html\n";
        delete msg;
    } else {
        HTTPServerMsg *serverMsg = dynamic_cast<HTTPServerMsg *>(msg);
        if (serverMsg) {
            EV << "HTTPClient received response from server:\n";
            EV << "-------------------\n";
            EV << serverMsg->getResponse() << "\n";
            EV << "-------------------\n";
            if (strstr(serverMsg->getResponse(), "<html>") != nullptr) {
                sendRequest("/images/logo.gif",1);
                sendRequest("/images/TechnikErleben.png",2);
            }
            delete serverMsg;
        } else {
            EV << "HTTPClient received an unknown message type. Discarding.\n";
            delete msg;
        }
    }
}

void HTTPClient::sendRequest(const char *ressource, int destinationport) {
    HTTPClientMsg *msg = new HTTPClientMsg();
    msg->setMethod("GET");
    msg->setRessource(ressource);

    UDPControlInfo *controlInfo = new UDPControlInfo();
    controlInfo->setSrcPort(srcPort);
    controlInfo->setDestPort(destinationport);

    // Add IP addresses
    inet::IPv4Address srcIP("192.168.1.2");  // Client IP (dummy)
    inet::IPv4Address destIP("192.168.1.1"); // Server IP (dummy)
    controlInfo->setSrcIPv4(srcIP);
    controlInfo->setDestIPv4(destIP);

    msg->setControlInfo(controlInfo);

    send(msg, "toLowerLayer");
    EV << "Request sent: Method=GET, Resource=" << msg->getRessource() << "\n";
}

