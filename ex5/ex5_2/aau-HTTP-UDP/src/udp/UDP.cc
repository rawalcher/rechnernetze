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
    // TODO implement as needed (or leave empty)
    EV << "Initialized UDP\n";
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
    if (auto *clientMsg = dynamic_cast<HTTPClientMsg *>(msg)) {
        EV << "UDP: Handling HTTPClientMsg.\n";

        UDPControlInfo *controlInfo = check_and_cast<UDPControlInfo *>(clientMsg->getControlInfo());
        UDPSegment *segment = new UDPSegment();
        segment->setSrcPort(controlInfo->getSrcPort());
        segment->setDstPort(controlInfo->getDestPort());
        segment->setChecksum(0);
        segment->encapsulate(clientMsg);
        segment->setLength(segment->getByteLength());

        send(segment, "toLowerLayer");
        return;
    }

    if (auto *serverMsg = dynamic_cast<HTTPServerMsg *>(msg)) {
        EV << "UDP: Handling HTTPServerMsg.\n";

        UDPControlInfo *controlInfo = check_and_cast<UDPControlInfo *>(serverMsg->getControlInfo());
        UDPSegment *segment = new UDPSegment();
        segment->setSrcPort(controlInfo->getSrcPort());
        segment->setDstPort(controlInfo->getDestPort());
        segment->setChecksum(0);
        segment->encapsulate(serverMsg);
        segment->setLength(segment->getByteLength());

        send(segment, "toLowerLayer");
        return;
    }
    EV << "UDP: Received unknown message type. Discarding.\n";
    delete msg;
}


void UDP::handleUDPSegment(cPacket *msg) {
    UDPSegment *segment = check_and_cast<UDPSegment *>(msg);
    UDPControlInfo *controlInfo = new UDPControlInfo();
    controlInfo->setSrcPort(segment->getSrcPort());
    controlInfo->setDestPort(segment->getDstPort());
    controlInfo->setSrcIPv4(inet::IPv4Address("192.168.1.1"));
    controlInfo->setDestIPv4(inet::IPv4Address("192.168.1.100"));

    // Decapsulate and handle the encapsulated message
    cPacket *encapsulatedMsg = segment->decapsulate();
    encapsulatedMsg->setControlInfo(controlInfo);

    send(encapsulatedMsg, "toUpperLayer");

    delete segment;
}
