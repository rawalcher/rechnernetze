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
import networks.Host;

network AlohaNetwork
{
    submodules:
        client1: Host {
                @display("i=device/pc2;p=275,78");
                application.appType = "client";
                macAddress = "00-08-74-4C-7F-1B";
                clientIP = "192.168.1.1";
                clientPort = 23456;
                serverIP = "192.168.3.3";
                serverPort = 23456;
        }
        client2: Host {
                @display("i=device/pc;p=135,80");
                application.appType = "client";
                macAddress = "00-08-74-4C-7F-2B";
                clientIP = "192.168.2.2";
                clientPort = 23456;
                serverIP = "192.168.3.3";
                serverPort = 23456;
        }
        server1: Host {
                @display("i=device/server;p=135,277");
                application.appType = "server";
                macAddress = "00-08-74-4C-7F-3B";
                clientIP = "192.168.3.3";
                clientPort = 23456;
                serverIP = "192.168.3.3";
                serverPort = 23456;
        }
        server2: Host {
                @display("i=device/server;p=275,276");
                application.appType = "server";
                macAddress = "00-08-74-4C-7F-4B";
                clientIP = "192.168.3.4";
                clientPort = 23456;
                serverIP = "192.168.3.4";
                serverPort = 23456;
        }
        link: AlohaCable {
            @display("p=213,182;b=296,6;o=#6060b0");
            gates:
                inLink[4];
                outLink[4];
        }
    connections:
        client1.outNet --> interHostChannel --> link.inLink[0];
        client1.inNet <-- interHostChannel <-- link.outLink[0];

        client2.outNet --> interHostChannel --> link.inLink[1];
        client2.inNet <-- interHostChannel <-- link.outLink[1];

        server1.outNet --> interHostChannel --> link.inLink[2];
        server1.inNet <-- interHostChannel <-- link.outLink[2];

        server2.outNet --> interHostChannel --> link.inLink[3];
        server2.inNet <-- interHostChannel <-- link.outLink[3];

}


channel interHostChannel extends ned.DelayChannel
{
    delay = 100ms;
}
