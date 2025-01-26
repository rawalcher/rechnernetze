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

package aau_http.simulations;

import aau_http.app.HTTPClient;
import aau_http.tcp.TCP;

module Client
{
    gates:
        input inNet;
        output outNet;

    submodules:
       	clientHTTP: HTTPClient;
        clientTCP: TCP;

    connections:
       clientHTTP.toLowerLayer --> {delay = 10ms;} --> clientTCP.fromUpperLayer;
        clientTCP.toLowerLayer --> {delay = 100ms;} --> outNet;
        
        inNet --> {delay = 100ms;} --> clientTCP.fromLowerLayer;
        clientTCP.toUpperLayer --> {delay = 10ms;} --> clientHTTP.fromLowerLayer;
}
