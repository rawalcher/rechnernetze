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
    maxWaitTime = 0.8;  // Get parameter from NED
}

void Aloha::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        // Backoff timer expired → retransmit
        sendCopyOf(frameBuf, 0);
        delete msg;
    }
    else if (msg->arrivedOn("inUpperLayer")) {
        // New packet from app layer
        if (frameBuf == nullptr) {  // Not busy? Send immediately
            frameBuf = msg;
            sendCopyOf(frameBuf, 0);
        }
        else {  // Busy → drop new packet (no queuing)
            delete msg;
            EV << "Dropping new packet (already transmitting)\n";
        }
    }
    else {
        // Collision detected (message from lower layer)
        delete msg;  // Discard collision notification
        if (frameBuf != nullptr) {
            // Schedule retransmission after random backoff
            scheduleAt(simTime() + uniform(0, maxWaitTime), new cMessage("backoff"));
        }
    }
}

void Aloha::sendCopyOf(cMessage *msg, double delay) {
    cMessage *copy = msg->dup();
    sendDelayed(copy, delay, "outLowerLayer");
}