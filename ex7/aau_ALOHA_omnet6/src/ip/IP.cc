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
// (c) 2018 Christian Timmerer, Alpen-Adria-Universität Klagenfurt / Bitmovin Inc.
//          christian.timmerer@itec.aau.at / christian.timmerer@bitmovin.com
//
// 621.800 (18W) Computer Networks and Network Programming

#include "IP.h"

Define_Module(IP);

void IP::initialize() {
    // Check if this IP impl is a router or not:
    isRouter = 0;
}

void IP::handleMessage(cMessage *msg) {

    if (msg->arrivedOn("inUpperLayer")) { //comes from above

        IPDatagram *pkt = new IPDatagram("IPpkt");

        IPControlInfo *ipInfo = check_and_cast<IPControlInfo *>(
                msg->removeControlInfo());

        pkt->setDestIP(ipInfo->getDestIP());
        pkt->setSrcIP(ipInfo->getSrcIP());
        pkt->setProtocol(ipInfo->getProtocol());

        AlohaControlInfo *aInfo = new AlohaControlInfo();
        aInfo->setSrc(
                *new inet::MACAddress(
                        getParentModule()->par("macAddress").stringValue()));
        aInfo->setDest(
                *new inet::MACAddress(
                        getParentModule()->par("receiverMac").stringValue()));
        pkt->setControlInfo(aInfo);

        // create link layer control info and attach to IP packet
        pkt->encapsulate((cPacket*) msg);
        // send to lower layer through appropriate gate
        int gate = 0;
        send(pkt, "outLowerLayer", gate);
        delete ipInfo;
    }

    else if (msg->arrivedOn("inLowerLayer")) {
        //message comes from the network:
        IPDatagram *ipDat = check_and_cast<IPDatagram *>(msg);
        // create IP control info
        IPControlInfo *ipInfo = new IPControlInfo();
        ipInfo->setDestIP(ipDat->getDestIP());
        ipInfo->setSrcIP(ipDat->getSrcIP());
        ipInfo->setProtocol(ipDat->getProtocol());

        // decapsulate payload and attach IP control info to segment
        cMessage *unpacked = ipDat->decapsulate();
        unpacked->setControlInfo(ipInfo);
        int gate = 0;
        send(unpacked, "outUpperLayer", gate);
        delete ipDat;       //not needed anymore

    }
}
