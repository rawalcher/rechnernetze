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

#include "HTTPServer.h"
#include "HTTPClientMsg_m.h"
#include "HTTPServerMsg_m.h"

Define_Module(HTTPServer);



void HTTPServer::initialize()
{
    index_html = "<html>\n\t<head><title>Demo Web Site 2019</title></head>\n\t<body>\n\t\t<img src=\"logo.gif\"/>\n\t\t<h1>Welcome</h1>\n\t\t<img src=\"TechnikErleben.png\"/>\n\t</body>\n</html>\n";
    logo_gif = "logo.gif";
    TechnikErleben_png = "TechnikErleben.png";// TODO implement initialize
}

void HTTPServer::handleMessage(cMessage *msg)
{
    HTTPClientMsg *httpMsg = check_and_cast<HTTPClientMsg *>(msg);
    EV << "Server received message: methode = " << httpMsg->getMethod() << ", resource = " << httpMsg->getResource() << endl;

    HTTPServerMsg *response = new HTTPServerMsg("HTTPServerMsg");

    if(std::string(httpMsg->getMethod())!="GET"){
        EV << "Method is GET" << endl;
        return;
    }else{
        std::string resource = httpMsg->getResource();
        if(resource.size()<4){
            EV << "path size is too small!" << endl;
            delete httpMsg;
                        return; // /_\r\n = 4 char min.
                    }
        if(resource[resource.size()-1]!='\n'||resource[resource.size()-2]!='\r'){
            EV << "no carrige return linefeat at end" << endl;
            delete httpMsg;
            return; // no carrige return linefeat at end
                }else{
                    resource = resource.substr(0, resource.size()-2);
                }
        if(resource[0]=='/'){
            resource = resource.substr(1, resource.size());
            if(resource.size()==0){
                EV << "removing first / went wrong" << endl;
                delete httpMsg;
                return;
            }
        }else{
            EV << "no / in path" << endl;
            delete httpMsg;
                            return;
        }
        // get all path parts:
        size_t firstSlashPos = resource.find('/');
        if (firstSlashPos == std::string::npos) {
            EV << "here idk" << endl;
            delete httpMsg;
            return;
        }
        std::string part1 = resource.substr(0, firstSlashPos);
        std::string part2 = resource.substr(firstSlashPos+1);
        if(part1 != "demo"){
            EV << "first isn´t demo" << endl;
            delete httpMsg;
            return;
        }
        if(part2.size()==0){
            response->setResponse(index_html.c_str());
        }else if(part2=="logo.gif"){
            response->setResponse(logo_gif.c_str());
        }else if(part2 == "TechnikErleben.png"){
            response->setResponse(TechnikErleben_png.c_str());
        }else{
            EV << "searched element isn´t here" << endl;
            return;
        }
        send(response, "toLowerLayer");

    }
    delete httpMsg;
    // TODO implement handleMessage
}
