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
//aau-HTTP-UDP.networks
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
// (c) 2019 Christian Timmerer, Alpen-Adria-Universität Klagenfurt / Bitmovin Inc.
//          christian.timmerer@itec.aau.at / christian.timmerer@bitmovin.com
//
// 621.800 (19W) Computer Networks and Network Programming

import src.app.HTTPServer;
import src.udp.UDP;

module ServerHost
{
    @display("bgb=478,286");
    gates:
        input inNet;
        output outNet;

    submodules:
        httpServer1: src.app.HTTPServer {
            parameters:
                srcPort = 80;
                @display("p=27,85");
        }
        httpServer2: src.app.HTTPServer {
            parameters:
                srcPort = 81;
                @display("p=222,85");
        }
        httpServer3: src.app.HTTPServer {
            parameters:
                srcPort = 82;
                @display("p=130,185");
        }
        udp: src.udp.UDP {
			gateSize = 3;
            @display("p=130,85");
        }
    connections:
        httpServer1.toLowerLayer --> udp.fromUpperLayer[0];
        httpServer2.toLowerLayer --> udp.fromUpperLayer[1];
        httpServer3.toLowerLayer --> udp.fromUpperLayer[2];
        udp.toUpperLayer[0] --> httpServer1.fromLowerLayer;
        udp.toUpperLayer[1] --> httpServer2.fromLowerLayer;
        udp.toUpperLayer[2] --> httpServer3.fromLowerLayer;

        udp.toLowerLayer --> outNet;
        udp.fromLowerLayer <-- inNet;
}
