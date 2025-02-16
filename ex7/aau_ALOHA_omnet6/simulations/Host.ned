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

package networks;

import src.app.App;
import src.udp.UDP;
import src.ip.IP;
import src.aloha.Aloha;
import src.aloha.AlohaCable;

module Host
{
    parameters:
        string clientIP;
        int clientPort;
        string serverIP;
        string receiverMac;
        string macAddress;
        int serverPort;
    gates:
        input inNet;
        output outNet;
    submodules:
        application: App {
              
        }
        udp: UDP {
            @display("p=89,189;i=block/layer");
        }
        ip: IP {
            @display("p=89,284;i=block/routing");
            gates:
                outLowerLayer[1];
                inLowerLayer[1];
                inUpperLayer[1];
                outUpperLayer[1];
        }
        aloha: Aloha {
        		macAddress = parent.macAddress;
                @display("p=89,358;i=block/layer");
        }
    connections:
        // application layer connections
        application.outLowerLayer --> udp.inUpperLayer;
        application.inLowerLayer <-- udp.outUpperLayer;

        // transport layer connections
        udp.outLowerLayer --> ip.inUpperLayer[0];
        udp.inLowerLayer <-- ip.outUpperLayer[0];

        // network layer connections
        ip.outLowerLayer[0] --> aloha.inUpperLayer;
        ip.inLowerLayer[0] <-- aloha.outUpperLayer;

        // data link layer connections
        aloha.outLowerLayer --> outNet;
        aloha.inLowerLayer <-- inNet;
}
