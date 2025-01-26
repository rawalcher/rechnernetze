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

#include "App.h"
#include "AppMessage_m.h"

Define_Module(App);

void App::initialize() {
    this->clientIP = new inet::IPv4Address(
            getParentModule()->par("clientIP").stringValue());
    this->serverIP = new inet::IPv4Address(
            getParentModule()->par("serverIP").stringValue());

    if (opp_strcmp(par("appType"), "client") == 0) {
        // send first message ...
        event = new cMessage("event");
        scheduleAt(simTime() + 1, event);
    }
}

void App::handleMessage(cMessage *msg) {
    if (msg == event) {
        //  send first message ...
        AppMessage *firstMessage = new AppMessage("first message");
        firstMessage->setNumber(42);
        firstMessage->setCommand(1);
        // adding control info:
        UDPControlInfo *udpInfo = new UDPControlInfo();
        udpInfo->setSrcIP(*this->clientIP);
        udpInfo->setDestIP(*this->serverIP);
        udpInfo->setSrcPort(srcPort);
        udpInfo->setDestPort(destPort);
        firstMessage->setControlInfo(udpInfo);
        bubble("Sending first message ...");
        send(firstMessage, "outLowerLayer");
    } else {
        AppMessage *message = check_and_cast<AppMessage *>(msg);

        if (opp_strcmp(par("appType"), "server") == 0) {
            AppMessage *returnMsg = new AppMessage("returning message");
            returnMsg->setNumber(message->getNumber() + message->getCommand());
            returnMsg->setCommand(0);
            // adding control info:
            UDPControlInfo *inCtrlInfo = check_and_cast<UDPControlInfo *>(
                    msg->removeControlInfo());
            UDPControlInfo *udpInfo = new UDPControlInfo();
            udpInfo->setSrcIP(*this->clientIP);
            udpInfo->setDestIP(inCtrlInfo->getSrcIP());
            udpInfo->setSrcPort(inCtrlInfo->getSrcPort());
            returnMsg->setControlInfo(udpInfo);
            EV << "-- sending answer " << returnMsg->getNumber() << "\n";
            send(returnMsg, "outLowerLayer");
        } else {
            EV << "-- Result: " << message->getNumber() << "\n";
        }
    }
}
