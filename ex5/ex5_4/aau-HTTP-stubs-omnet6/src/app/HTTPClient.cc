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
#include "../tcp/TCPControlInfo_m.h"

Define_Module(HTTPClient);


    std::string request1;
        std::string request2;
        std::string request3;
cMessage *timeoutEvent = nullptr;
int count;

void HTTPClient::initialize() {
    request1="/demo/\r\n";
    request2="/demo/logo.gif\r\n";
    request3="/demo/TechnikErleben.png\r\n";
    count = 1;

    // send first request
    sendMessage(request1, 1);
}

void HTTPClient::handleMessage(cMessage *msg) {
    if(msg==nullptr){
        if (timeoutEvent) {
            cancelEvent(timeoutEvent); // Abbrechen des Timeouts
        }
        EV << "Server Closed Connection without giving any return" << endl; //HTTP 0.9 => something went wrong = no return value
    }
    if (msg->isSelfMessage()) {
            if (msg == timeoutEvent) {
                EV << "Timeout erreicht, keine Antwort vom Server erhalten! = Anfrage war falsch" << endl;
            }
        } else {
            HTTPServerMsg *response = check_and_cast<HTTPServerMsg *>(msg);
            EV << "Antwort vom Server empfangen: " << response->getResponse() << endl;
            if (timeoutEvent) {
                cancelEvent(timeoutEvent); // Abbrechen des Timeouts
            }

        }

    // send next request:
    if(count == 2){
        sendMessage(request2, 0);
    }else if(count == 3){
        sendMessage(request3, 2);
    }
    // else stop
}

void HTTPClient::sendMessage(const std::string &input, int c){
    HTTPClientMsg *msg = new HTTPClientMsg("HTTPClientMsg");
                msg->setMethod("GET");
                msg->setResource(input.c_str());


                TCPControlInfo *controlInfo = new TCPControlInfo();
                    controlInfo->setSrcIPv4(inet::IPv4Address("192.168.1.1"));
                    controlInfo->setSrcPort(srcPort);
                    controlInfo->setDestIPv4(inet::IPv4Address("192.168.1.100"));
                    controlInfo->setDestPort(destPort);
                    controlInfo->setTcpCommand(c);

                    msg->setControlInfo(controlInfo);


                send(msg, "toLowerLayer");
                EV << "Client send message:" << msg << endl;
                count = count+1;

                if (timeoutEvent) {
                                cancelEvent(timeoutEvent); // Abbrechen des Timeouts
                            }
                timeoutEvent = new cMessage("timeout");
                        scheduleAt(simTime() + 10, timeoutEvent);
}
