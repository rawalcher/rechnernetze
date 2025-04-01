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

#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>

#include "HTTPServer.h"
#include "HTTPClientMsg_m.h"
#include "HTTPServerMsg_m.h"
#include "../udp/UDPControlInfo_m.h"

Define_Module(HTTPServer);

void HTTPServer::initialize() {
    documentRoot = par("documentRoot").stdstringValue();
    srcPort = par("srcPort").intValue();
    destPort = par("destPort").intValue();
    EV << "HTTPServer initialized at port " << srcPort << "\n";
}

void HTTPServer::handleMessage(cMessage *msg) {
    HTTPClientMsg *clientMsg = check_and_cast<HTTPClientMsg *>(msg);
    if (clientMsg) {
        UDPControlInfo *controlInfo = check_and_cast<UDPControlInfo *>(clientMsg->getControlInfo());
        std::string method = clientMsg->getMethod();
        std::string resource = clientMsg->getResource();
        EV << "Received request: Method=" << method << ", Resource=" << resource << "\n";
        std::string response = processRequest(method, resource);

        UDPControlInfo *responseControlInfo = new UDPControlInfo();
        responseControlInfo->setSrcIPv4(controlInfo->getDestIPv4());
        responseControlInfo->setDestIPv4(controlInfo->getSrcIPv4());
        responseControlInfo->setSrcPort(controlInfo->getDestPort());
        responseControlInfo->setDestPort(controlInfo->getSrcPort());

        HTTPServerMsg *serverMsg = new HTTPServerMsg();
        serverMsg->setResponse(response.c_str());
        serverMsg->setControlInfo(responseControlInfo);
        send(serverMsg, "toLowerLayer");
        delete clientMsg;
    } else {
        EV << "Received unknown message type. Discarding.\n";
        delete msg;
    }
}

std::string HTTPServer::processRequest(const std::string &method, const std::string &resource) {
    if (method != "GET") {
        return "HTTP/0.9 400 Bad Request\nOnly GET method is supported.";
    }
    std::string data;
    if (resource == "/images/logo.gif") {
        data = "logo.gif";
    } else if (resource == "/images/TechnikErleben.png") {
        data = "TechnikErleben.png";
    }else {
        std::string filePath = documentRoot + resource;

        if (!std::filesystem::exists(filePath)) {
            EV << "File not found: " << filePath << "\n";
            return "HTTP/0.9 404 Not Found\nFile not found.";
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            EV << "Failed to open file: " << filePath << "\n";
            return "HTTP/0.9 500 Internal Server Error\nCould not open file.";
        }

        std::ostringstream fileContent;
        fileContent << file.rdbuf();
        file.close();
        EV << "Successfully read file: " << filePath << "\n";
        data = fileContent.str();
    }
    return data;
}
