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

#include "AlohaCable.h"

Define_Module(AlohaCable);

void AlohaCable::initialize() {
    this->numHosts = 4;
    this->LINK_DELAY = 0.005;
    this->colMsg = new cMessage("COL");
    this->bufMsg = NULL;
    this->collision = 0;
}

void AlohaCable::handleMessage(cMessage *msg) {
    if (!msg->isSelfMessage()) {
        EV << "-- Got new message. Collision state " << collision << "\n";
        if (bufMsg == NULL) {
            // no collision, buffer message and send self message to simulate link delay
            this->bufMsg = msg;
            delayMsg = new cMessage("DELAY");
            scheduleAt(simTime() + this->LINK_DELAY, delayMsg);
            EV << "-- This was not a collision. LINK_DELAY scheduled \n";
        } else {
            // collision
            EV << "-- collision detected. Previous collisions " << collision
                      << "\n";
            bubble("COL on cable!");
            collision++;
            if (collision == 1) {
                EV << "-- sending broadcast COL message.\n";
                broadcastMsg(this->colMsg);
            }
        }
    } else {
        if (collision == 0) {
            // broadcast message to all connected hosts
            bubble("NO COL on cable! Message broadcasted!");
            broadcastMsg(this->bufMsg);
            bufMsg = NULL;
        } else {
            // collision has happened, reset flag:
            bubble("Collision timeout: Cable free again!");
            collision = 0;
            bufMsg = NULL;
        }
    }

}

void AlohaCable::broadcastMsg(cMessage *msg) {
    for (int i = 0; i < this->numHosts; i++) {
        EV << "-- sending " << msg->getName() << " message\n";
        cMessage *copy = (cMessage *) msg->dup();
        send(copy, "outLink", i);
    }
    EV << "-- all " << msg->getName() << " messages sent\n";
}

