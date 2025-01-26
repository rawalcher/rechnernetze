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

#include "IP.h"

Define_Module(IP);

void IP::initialize() {
	// Check if this IP impl is a router or not:
	isRouter = 0;
	if (strcmp(string(getParentModule()->getName()).substr(0, 7).c_str(), "router0") == 0) {
		isRouter = 1;
		EV << "[Router] " << getParentModule()->getName()
				<< " is initializing, configuring routing table ...\n";
		int i = 0;
		for (i = 0; i < routingTableSize; i++) {
			if (strcmp((routingtable[i][0]).c_str(),
					this->getParentModule()->getName()) == 0) {
				// this is an entry for this router, we add it to our table:
				forwardingtable[routingtable[i][1]] = atoi(
						(routingtable[i][4]).c_str());
				// you'll find the whole forwarding table in IP.h
			}
		}
	}
}


void IP::handleMessage(cMessage *msg) {
	if (msg->arrivedOn("inUpperLayer")) { //comes from transport layer (instance cannot be a router)
		// TODO:
		// * Use IPControlInfo to create IPDatagram
		// * send to network

		EV << "[IP] Received message from Transport Layer.\n";

		IPControlInfo *controlInfo = check_and_cast<IPControlInfo *>(msg->removeControlInfo());
		IPDatagram *datagram = new IPDatagram();

		// Set protocol
		datagram->setProtocol(controlInfo->getProtocol());


		datagram->setSrcIP(controlInfo->getSrcIP());
		EV << "[IP] Source IP set to: " << controlInfo->getSrcIP() << "\n";

		datagram->setDestIP(controlInfo->getDestIP());
		EV << "[IP] Destination IP set to: " << controlInfo->getDestIP() << "\n";

		// Set default TTL (e.g., 64)
		datagram->setTtl(64);

		// Set payload length and encapsulate payload
		datagram->setPayloadLength(check_and_cast<cPacket *>(msg)->getByteLength());
		datagram->encapsulate(check_and_cast<cPacket *>(msg));

		EV << "[IP] Datagram created with TTL = " << datagram->getTtl()
		   << ", Payload Length = " << datagram->getPayloadLength() << "\n";

		// Send datagram to the lower layer
		send(datagram, "outLowerLayer", 0);
		EV << "[IP] Datagram sent to lower layer.\n";

	}

	else if (msg->arrivedOn("inLowerLayer")) {
		EV << "[IP] Received message from Lower Layer.\n";

		IPDatagram *datagram = check_and_cast<IPDatagram *>(msg);

		//message comes from the network:
		if (isRouter == 1) {
			// This is a router and we have to forward the datagram

			// TODO:
			// * Find out the destination network
			//   (IP address which ends with '.0', use IPAddress->getNetwork().str())
			// * Find out which gate is the right one
			//   use the forwarding table 'forwardingtable' initialized above
			// * Send the datagram to the appropriate gate.

			EV << "[Router] Processing datagram.\n";

			// Determine destination network
			inet::IPv4Address destIP = datagram->getDestIP();
			std::string destNetwork = destIP.getNetwork().str();

			// Log destination
			EV << "[Router] Destination IP: " << destIP.str() << ", Network: " << destNetwork << "\n";

			// Check TTL
			if (datagram->getTtl() <= 0) {
				EV << "[Router] TTL expired. Dropping datagram.\n";
				delete datagram;
				return;
			}

			datagram->setTtl(datagram->getTtl() - 1);
			EV << "[Router] TTL decremented to " << datagram->getTtl() << "\n";

			// Lookup forwarding table
			if (forwardingtable.find(destNetwork) != forwardingtable.end()) {
				int outGate = forwardingtable[destNetwork];
				EV << "[Router] Forwarding to gate: " << outGate << "\n";
				send(datagram, "outLowerLayer", outGate);
			} else {
				EV << "[Router] No route found for network: " << destNetwork << "\n";
				delete datagram;
			}

		} else {
			// we are a host and not a router, so we have to hand it over to the next higher level.

			// TODO
			// * Create ControlInfo for upper layer ... application layer needs the data.
			// * Decapsulate message
			// * send to upper layer

			EV << "[Host] Processing datagram.\n";

			inet::IPv4Address destIP = datagram->getDestIP();
			inet::IPv4Address srcIP = datagram->getSrcIP();

			EV << "[Host] Datagram received from: " << srcIP.str() << " destined for: " << destIP.str() << "\n";

			// Create control info for the upper layer
			IPControlInfo *controlInfo = new IPControlInfo();
			controlInfo->setProtocol(datagram->getProtocol());
			controlInfo->setDestIP(destIP);
			controlInfo->setSrcIP(srcIP);

			// Decapsulate payload
			cPacket *payload = datagram->decapsulate();
			payload->setControlInfo(controlInfo);

			// Pass message to upper layer
			send(payload, "outUpperLayer", 0);
			EV << "[Host] Payload sent to upper layer.\n";

			delete datagram;
		}
	}

}
