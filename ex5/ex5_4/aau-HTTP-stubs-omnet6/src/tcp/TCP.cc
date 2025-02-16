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

#include "TCP.h"
#include <stdio.h>
#include "../3rdParty/IPv4Address.h"
#include "../3rdParty/IPv6Address.h"
#include "../app/HTTPClientMsg_m.h"
#include "../app/HTTPServerMsg_m.h"
#include "../tcp/TCPControlInfo_m.h"
#include "../tcp/TCPSegment_m.h"

Define_Module(TCP);

/*
int seq;
int ack;
int status; // 1-holding connection, 2-no connection, 3-building connection, 4-server verify connection,
            // 5-FIN_Wait, 6-Server Close-Wait, 7-Client Time-Wait, 8-Server Last_ACK
TCPSegment *lastMsg;
cPacket *payload; // what I want to send when connection is build
*/

void TCP::initialize(){
    seq = 100; // init randomly
    status = 2;
    payload = nullptr;
    lastMsg = nullptr;
    expectAck = false;
    timeoutEvent = new cMessage("timeout resend lastMsg");
    timeoutTime = 1;
    // TODO Initialise seqn and ackn.
}


void TCP::handleMessage(cMessage *msg){
    // TODO Handle timeouts.
    // TODO Simulate packet loss.
    if (msg->arrivedOn("fromUpperLayer")) {
        // Comes from appliction.
        //EV << "TCP got message from Upper Layer:" << msg << endl;
        this->handleAppMessage((cPacket*)msg);
    }

    else if (msg->arrivedOn("fromLowerLayer")) {
        int k = intuniform(0, 10);
        if(k<0){// set rate of packet loss accordingly. Too high and a connection may not get created (5 is max in my testing)
            EV << "Simulate package loss: " << k << endl;
            return;
        }
        // Comes from lower layer.
        //EV << "TCP got message from Lower Layer:" << msg << endl;
        this->handleTCPSegment((cPacket*) msg);
    }
    else if (msg->isSelfMessage()){
        if(strcmp(msg->getName(), "Close Connection") == 0){// closing connection timeout
            ack = -1;
            initialize();
            EV << "Client Closed!" << endl;
        }else{
        EV << "Timeout event triggered: " << endl;
        this->handleTimeout();
        }
    }
    else{
        EV << "TCP reccieved message is bugged!" << msg << endl;
    }
}

void TCP::handleAppMessage(cPacket *msg) {

    if (auto *httpClientMsg = dynamic_cast<HTTPClientMsg *>(msg)) {
    TCPControlInfo *controlInfo = check_and_cast<TCPControlInfo *>(httpClientMsg->getControlInfo());
    if(controlInfo->getTcpCommand()==1){
        if(status==2){
            // open connection
            TCPSegment *segment = new TCPSegment("TCPSegment");
            segment->setSrcPort(controlInfo->getSrcPort());
            segment->setDestPort(controlInfo->getDestPort());
            segment->setSyn(true);
            segment->setSeqNr(seq);

            send(segment, "toLowerLayer");
            EV << "1. Client opens connection. SEQ: " << seq << endl;

            seq=seq+1;
            status=3;
            lastMsg = segment;
            // save HTTP Message which needs to be transfered
            httpClientMsg->removeControlInfo();
            payload = httpClientMsg;

        }else if(status==1){
            TCPSegment *segment = getSegment(controlInfo->getSrcPort(), controlInfo->getDestPort(), seq, ack);
            httpClientMsg->removeControlInfo();
            segment->encapsulate(httpClientMsg);
            send(segment, "toLowerLayer");
            lastMsg = segment;
            EV << "No need for requested open_Connection. TCP is already connected. Send payload. SEQ: " << seq << " ACK: " << ack << endl;
            seq = seq + getLength(httpClientMsg);
            expectAck = true;

        }else{
            EV << "ERROR: Client wants to Build up Connection, but TCP is not in correct State" << endl;
            return;
        }

    }else if(controlInfo->getTcpCommand()==0){
        if(status!=1){
            EV << "ERROR: Client wants to send Data, but no connection is ready and asked to create!" << endl;
            return;
        }
        TCPSegment *segment = new TCPSegment("TCPSegment");
                    segment->setSrcPort(controlInfo->getSrcPort());
                    segment->setDestPort(controlInfo->getDestPort());
                    segment->setSeqNr(seq);
                    segment->setAck(true);
                    segment->setAckNr(ack);
                    httpClientMsg->removeControlInfo();
                    segment->encapsulate(httpClientMsg);
                    send(segment, "toLowerLayer");
                    lastMsg = segment;
                    EV << "Send Data over connection. SEQ: " << seq << " ACK: " << ack << endl;
                    seq = seq + getLength(httpClientMsg);
                    expectAck = true;

    }else if(controlInfo->getTcpCommand()==2){        // close connection
        if(status!=1){
            EV << "ERROR: Client wants to close Connection, but is not in right state! State: " << status << endl;
        }
        TCPSegment *segment = new TCPSegment("TCPSegment");
                            segment->setSrcPort(controlInfo->getSrcPort());
                            segment->setDestPort(controlInfo->getDestPort());
                            segment->setSeqNr(seq);
                            segment->setAck(true);
                            segment->setAckNr(ack);
                            segment->setFin(true);
                            httpClientMsg->removeControlInfo();
                            segment->encapsulate(httpClientMsg);
                            send(segment, "toLowerLayer");
                            lastMsg = segment;
                            EV << "7. Closing Connection: Sending last request with Finbit. SEQ: " << seq << " ACK: " << ack << endl;
                            seq = seq + getLength(httpClientMsg);
                            expectAck = true;
                            status=5;
    }
    renewTimeout();

    }else if(auto *httpServerMsg = dynamic_cast<HTTPServerMsg *>(msg)){
        TCPControlInfo *controlInfo = check_and_cast<TCPControlInfo *>(httpServerMsg->getControlInfo());
        if(controlInfo->getTcpCommand()==1){
                EV << "Server wants to build up a connection? (Not implemented)" << endl;
                return;
        }else if(controlInfo->getTcpCommand()==2){
                // close connection
                EV << "Server wants to init close connection? (Not implemented)" << endl;


         }else if(controlInfo->getTcpCommand()==0){
                // send answer without closing connection!
                TCPSegment *segment = new TCPSegment("TCPSegment");
                segment->setSrcPort(controlInfo->getDestPort());// set according to config
                segment->setDestPort(controlInfo->getSrcPort());// set according to config
                segment->setSeqNr(seq);
                segment->setAck(true);
                segment->setAckNr(ack);
                httpServerMsg->removeControlInfo();
                segment->encapsulate(httpServerMsg);
                send(segment, "toLowerLayer");
                lastMsg = segment;
                EV << "6.x. Server Sends answer. SEQ: " << seq << " ACK: " << ack << " MSG:\n" << httpServerMsg->getResponse() << endl;
                seq = seq + getLength(httpServerMsg);
                expectAck = true;

        }

        renewTimeout();

    }else{
        EV << "TCP Unknown message type" << endl;
    }
}


void TCP::handleTCPSegment(cPacket *msg) {
    TCPSegment *input = check_and_cast<TCPSegment *>(msg);

    if(status==2){ // create connection (server)
        ack=input->getSeqNr() +1;
            TCPSegment *segment = new TCPSegment("TCPSegment");
                    segment->setSrcPort(input->getDestPort());// set according to config
                    segment->setDestPort(input->getSrcPort());// return to sender
                    segment->setSyn(true);
                    segment->setSeqNr(seq);
                    segment->setAck(true);
                    segment->setAckNr(ack);

                    send(segment, "toLowerLayer");
                    renewTimeout();

                    EV << "2. Server got connection-init of Client and verifies opening a connection. SEQ: " << seq << " ACK: " << ack << endl;
                    lastMsg = segment;
                    seq=seq+1;
                    status=4;
                    expectAck = true;
                    return;
    }else if(status == 7){
        if(!input->getFin()){
            EV << "ERROR! TIME-WAIT received msg has no FIN-Bit! What now?" << endl;
            return;
        }
        send(lastMsg, "toLowerLayer");
        scheduleAt(simTime() + 2*timeoutTime, timeoutEvent); // 2 * max segment lifetime
        return;
    }

    // ensuring package safety:
    if(input->getAckNr()<seq){
        EV << "Caught: Package Loss! Ack is smaller then supposed. ACK: " << input->getAckNr() << endl;
        handleTimeout();
        return;
    }else if(input->getAckNr()>seq){
        EV << "ERROR! Received ACK is too big! ACK: " << input->getAckNr() << endl;
        return;
    }
    if (timeoutEvent->isScheduled()) { // Cancel timeout
        cancelEvent(timeoutEvent);
    }
    //
    cPacket *newHTTPMsg = input->decapsulate();
    if(expectAck){
        if(newHTTPMsg == nullptr){// default status = 1
            EV << "Received outstanding Ack: " << input->getAckNr() << endl;
            if (timeoutEvent->isScheduled()) { // Cancel timeout
                cancelEvent(timeoutEvent);
            }
            expectAck = false;

            if(status == 4){
                status=1;
                EV << "4. Server verified connection" << endl;
            }else if(status == 6){
                TCPSegment *segment2 = getSegment(input->getDestPort(), input->getSrcPort(), seq, ack);
                                    segment2->setFin(true);
                                    send(segment2, "toLowerLayer");
                                    lastMsg = segment2;
                                    status = 8;
                                    expectAck = true;
                                    EV << "8. Got last response from server send. Send FIN-Bit" << endl;
                                    renewTimeout();
                                    seq = seq+1;
            }else if(status == 8){// connection closed. reset TCP
                ack = -1;
                initialize();
                EV << "Server closed!" << endl;
            }
            return;
        }else{
            // got payload but did not expect that:
            EV << "Caught missed ACK: Received ACK with data!" << endl;
        }
    }
    /*
    if(status==3||status==4||status==1){
        EV << "Current status: " << status << endl;
    }*/



    if(status==3){// verify connection + send payload if present

        // return verification
        ack=input->getSeqNr() +1;// empty payload => no seq occupied
        TCPSegment *segment = new TCPSegment("TCPSegment");
                            segment->setSrcPort(input->getDestPort());// set according to config
                            segment->setDestPort(input->getSrcPort());// return to sender
                            segment->setSeqNr(seq);
                            segment->setAck(true);
                            segment->setAckNr(ack);
                            send(segment, "toLowerLayer");
                            lastMsg = segment;
                            status=1;
                            EV << "3. Client got SEQ of Server and sends ACK. SEQ: " << seq << " ACK: " << ack << endl;

                            // send payload
                            if(payload != nullptr){
                            TCPSegment *segment = new TCPSegment("TCPSegment");
                            segment->setSrcPort(input->getDestPort());// set according to config
                            segment->setDestPort(input->getSrcPort());// return to sender
                            segment->setSeqNr(seq);
                            segment->setAck(true);
                            segment->setAckNr(ack);
                            segment->encapsulate(payload);
                            //payload = nullptr;
                            send(segment, "toLowerLayer");
                            renewTimeout();
                            lastMsg = segment;
                            EV << "5. Client send the actual HTTP payload. SEQ: " << seq << " ACK: " << ack << endl;
                            seq = seq + getLength(payload);
                            expectAck=true;
                            }
                            /*
    }else if(status==4){
        cPacket *newHTTPMsg = input->decapsulate();
        if(newHTTPMsg != nullptr){ // empty Ack
            EV << "ERROR (4.): Got Msg, but payload isn´t null: " << endl;
            return;
        }*/
    }else if(status==1){ // connection ready + payload got send


        // Error case no expected Ack
        if(newHTTPMsg == nullptr){
            if(!input->getFin()){
                EV << "ERROR: Got Msg, but payload is null and didn´t expect an empty Ack " << endl;
                return;
            }else{  //empty fin-bit -> send Ack + last msg
                //send Ack
                ack = ack + 1;
                TCPSegment *segment1 = getSegment(input->getSrcPort(), input->getDestPort(), ack, seq);
                send(segment1, "toLowerLayer");
                // send fin-bit back
                TCPSegment *segment = getSegment(input->getSrcPort(), input->getDestPort(), ack, seq);
                segment->setFin(true);
                send(segment, "toLowerLayer");
                lastMsg = segment;
                EV << "8. Got fin-Bit from Client so sent ACK and separate Fin-Bit. ACK: " << ack << endl;
                renewTimeout();
                status = 8;
                expectAck = true;
                return;
            }
        }
        // unpack payload:
        TCPControlInfo *controlInfo = new TCPControlInfo();
        controlInfo->setSrcPort(input->getSrcPort());
        controlInfo->setDestPort(input->getDestPort());
        controlInfo->setTcpStatus(1);
        controlInfo->setSrcIPv4(inet::IPv4Address("127.0.0.0"));
        controlInfo->setDestIPv4(inet::IPv4Address("127.0.0.1"));
        /*if(input->getFin()){
            controlInfo->setTcpStatus(2); // HTTP-Server gets controll-info connection closed (2) on close_wait?
        }*/
        newHTTPMsg->setControlInfo(controlInfo);
        send(newHTTPMsg, "toUpperLayer");
        // send ACK
        ack = ack + getLength(newHTTPMsg);
        TCPSegment *segment = getSegment(input->getDestPort(), input->getSrcPort(), seq, ack);
        send(segment, "toLowerLayer");
        lastMsg = segment;
        EV << "Got real payload and send up! Sending Ack: " << ack << endl;
        // seq stays same because only empty ACK

        if(input->getFin()){
            // server -> close wait (do all same, but send fin bit with next=last Server-msg)
            status = 6;
            EV << "Received Fin-bit from Client! Send ACK: " << ack << "And last request up." << endl;
        }
    }else if(status==5){// client
        if(!input->getFin()){
        if(newHTTPMsg == nullptr){
            EV << "ERROR: (9.x.) got payload from Server without payload! " << endl;
        }
        TCPControlInfo *controlInfo = new TCPControlInfo();
        controlInfo->setSrcPort(input->getSrcPort());
        controlInfo->setDestPort(input->getDestPort());
        controlInfo->setTcpStatus(2);
        controlInfo->setSrcIPv4(inet::IPv4Address("127.0.0.0"));
        controlInfo->setDestIPv4(inet::IPv4Address("127.0.0.1"));
        newHTTPMsg->setControlInfo(controlInfo);
        send(newHTTPMsg, "toUpperLayer");
        //send Ack
        ack = ack + getLength(newHTTPMsg);
        TCPSegment *segment = getSegment(input->getDestPort(), input->getSrcPort(), seq, ack);
        send(segment, "toLowerLayer");
        lastMsg = segment;
        EV << "(9.x.) Got payload from Server and send up! Sending Ack: " << ack << endl;
        }else{
            //FIN-Bit from Server
            EV << "9. Got FIN-Bit from Server and send Ack: " << ack << endl;
            ack = ack + 1;
            TCPSegment *segment = getSegment(input->getDestPort(), input->getSrcPort(), seq, ack);
            send(segment, "toLowerLayer");
            lastMsg = segment;
            status = 7;
            // timed close
            if(timeoutEvent->isScheduled()){
                cancelEvent(timeoutEvent);
            }
            delete timeoutEvent;
            timeoutEvent = new cMessage("Close Connection");
            scheduleAt(simTime() + 2*timeoutTime, timeoutEvent); // 2 * max segment lifetime
        }


    }


    // TODO handle input from lower layer
}

//helper functions:
void TCP::renewTimeout(){
    if(timeoutEvent->isScheduled()){
        cancelEvent(timeoutEvent);
    }
    scheduleAt(simTime() + timeoutTime, timeoutEvent);
}
void TCP::handleTimeout(){
    EV << "resend last message!" << endl;
    send(lastMsg->dup(), "toLowerLayer");
    //expectAck = true;
    if(timeoutEvent->isScheduled()){
        cancelEvent(timeoutEvent);
    }
    scheduleAt(simTime() + timeoutTime, timeoutEvent);
}
int TCP::getLength(cPacket *msg){
    if (auto *httpClientMsg = dynamic_cast<HTTPClientMsg *>(msg)) {
        const char *resource = httpClientMsg->getResource();
        int lengthInBytes = static_cast<int>(strlen(resource));
        return lengthInBytes;
    }
    else if (auto *httpServerMsg = dynamic_cast<HTTPServerMsg *>(msg)) {
        const char *resource = httpServerMsg->getResponse();
                int lengthInBytes = static_cast<int>(strlen(resource));
                return lengthInBytes;
    }else{
        EV << "ERROR!!!!! can´t get Length" << endl;
        return -1;
    }

}
TCPSegment* TCP::getSegment(int destPort, int srcPort, int seq, int ack){
    TCPSegment *segment = new TCPSegment("TCPSegment");
    segment->setSrcPort(destPort);// set according to config
    segment->setDestPort(srcPort);// return to sender
    segment->setSeqNr(seq);
    segment->setAck(true);
    segment->setAckNr(ack);
    return segment;
}
