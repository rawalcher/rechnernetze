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
	    EV << "Stand der Dinge";
		this->handleUDPSegment((cPacket*)msg);
	}
}

void UDP::handleAppMessage(cPacket *msg) {
        EV << "UDP: Handling application layer message.\n";

        UDPControlInfo *controlInfo = check_and_cast<UDPControlInfo *>(msg->getControlInfo());
	EV << "From app: srcPort=" << controlInfo->getSrcPort()
   << ", destPort=" << controlInfo->getDestPort() << "\n";
        UDPSegment *segment = new UDPSegment();
        segment->setSrcPort(controlInfo->getSrcPort());
        segment->setDstPort(controlInfo->getDestPort());
        segment->setChecksum(0);
        segment->encapsulate(msg);
        segment->setLength(segment->getByteLength());

        send(segment, "toLowerLayer");
        return;
}

// TODO: raphael
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
	if (port == 80) {
		// Choose gate based on the server's ID
		if (getParentModule()->getFullName() == std::string("server")) {
			// This is the server - distribute to different servers round-robin or based on some logic
			// For simplicity, just use first server
			gateIndex = 0;
		} else {
			// This is the client - use the client's gate
			gateIndex = 0;
		}
	} else if (port == 6666) {
		// This is port for client
		gateIndex = 0;
	}

	send(encapsulatedMsg, "toUpperLayer", gateIndex);
	delete segment;
}
