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

#include "Aloha.h"

Define_Module(Aloha);

void Aloha::initialize() {
    frameBuf = nullptr;
    maxWaitTime = par("maxWaitTime").doubleValue();
}

void Aloha::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        if (frameBuf != nullptr) {
            sendCopyOf(frameBuf, 0);
        }
        delete msg;
        return;
    }

    if (msg->arrivedOn("inUpperLayer")) {
        if (frameBuf == nullptr) {
            frameBuf = msg;
            sendCopyOf(frameBuf, 0);
        } else {
            EV << "Already transmitting a frame; dropping new packet.\n";
            delete msg;
        }
        return;
    }


    if (strcmp(msg->getName(), "COL") == 0) {
        delete msg;
        if (frameBuf != nullptr) {
            scheduleAt(simTime() + uniform(0.2, maxWaitTime), new cMessage("backoff"));
        }
    }
    else {
        std::string src  = msg->par("src").stdstringValue();
        std::string dest = msg->par("dest").stdstringValue();
        std::string myName = getName();

        if (src == myName) {
            EV << "Frame successfully transmitted.\n";
            if (frameBuf != nullptr) {
                delete frameBuf;
                frameBuf = nullptr;
            }
            delete msg;
        }
        else if (dest == myName) {
            EV << "Frame received for this host; delivering to upper layer.\n";
            send(msg, "outUpperLayer");
        }
        else {
            delete msg;
        }
    }
}

void Aloha::sendCopyOf(cMessage *msg, double delay) {
    cMessage *copy = msg->dup();
    sendDelayed(copy, simTime() + delay, "outLowerLayer");
}