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

#include <omnetpp.h>
#include "UDP.h"
#include "UDPSegment_m.h"
#include "../app/HTTPServerMsg_m.h"
#include "../app/HTTPClientMsg_m.h"
#include "UDPControlInfo_m.h"
#include "../3rdParty/IPv4Address.h"
#include "../3rdParty/IPv6Address.h"

Define_Module(UDP);

void UDP::initialize() {
    EV << "Initialized UDP" << endl;
}

void UDP::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("fromUpperLayer")) {
       // comes from application
       this->handleAppMessage((cPacket*)msg);
    }
    else if (msg->arrivedOn("fromLowerLayer")) {
       //comes from lower layer
       this->handleUDPSegment((cPacket*)msg);
    }
}

void UDP::handleAppMessage(cPacket *msg) {
    EV << "UDP: Handling application layer message." << endl;

    UDPControlInfo *controlInfo = check_and_cast<UDPControlInfo *>(msg->getControlInfo());
    EV << "From app: srcPort=" << controlInfo->getSrcPort()
       << ", destPort=" << controlInfo->getDestPort() << endl;

    UDPSegment *segment = new UDPSegment();
    segment->setSrcPort(controlInfo->getSrcPort());
    segment->setDstPort(controlInfo->getDestPort());
    segment->setChecksum(0);
    segment->encapsulate(msg);
    segment->setLength(segment->getByteLength());

    send(segment, "toLowerLayer");
}

void UDP::handleUDPSegment(cPacket *msg) {
    UDPSegment *segment = check_and_cast<UDPSegment *>(msg);
    UDPControlInfo *controlInfo = new UDPControlInfo();
    EV << "UDP received segment: srcPort=" << segment->getSrcPort()
       << ", dstPort=" << segment->getDstPort() << "\n";

    controlInfo->setSrcPort(segment->getSrcPort());
    controlInfo->setDestPort(segment->getDstPort());

    // Decapsulate the message
    cPacket *encapsulatedMsg = segment->decapsulate();
    encapsulatedMsg->setControlInfo(controlInfo);

    // Map port to gate index - simple mapping for this simulation
    int gateIndex = 0;  // Default to first gate
    int port = segment->getDstPort();

    // Map ports to gate indices
    if (getParentModule()->getFullName() == std::string("server")) {
        // This is the server side UDP

        // Determine which server should receive the message
        if (port == 80) {
            gateIndex = 0;  // Server 1
        }
        else if (port == 81) {
            gateIndex = 1;  // Server 2
        }
        else if (port == 82) {
            gateIndex = 2;  // Server 3
        }
        else {
            EV << "Warning: Unknown destination port for server: " << port << endl;
        }
    }
    else {
        // This is the client side UDP

        // Client receives all responses regardless of source port
        if (port == 6666) {
            gateIndex = 0;  // Client application
        }
        else {
            EV << "Warning: Unknown destination port for client: " << port << endl;
        }
    }

    send(encapsulatedMsg, "toUpperLayer", gateIndex);
    delete segment;
}
