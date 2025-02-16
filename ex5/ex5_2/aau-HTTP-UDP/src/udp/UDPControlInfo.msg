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
//			Benjamin Rainer, Alpen-Adria-Universität Klagenfurt / Austrian Institute of Technology GmbH
//			benjamin.rainer@aau.at / benjamin.rainer@ait.ac.at
//
// 621.800 (24W) Computer Networks and Network Programming

cplusplus {{
#include "../3rdParty/IPv4Address.h"
#include "../3rdParty/IPv6Address.h"
}}

class inet::IPv4Address {
    @existingClass;
}

class inet::IPv6Address {
    @existingClass;
}

message UDPControlInfo
{
    inet::IPv4Address srcIPv4;	// source IPv4 address
    inet::IPv6Address srcIPv6;	// source IPv6 address
    int srcPort;					// source port

    inet::IPv4Address destIPv4;	// destination IPv4 address
    inet::IPv6Address destIPv6;	// destination IPv6 address
    int destPort;				// destination port
}
