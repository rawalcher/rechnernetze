
package aau_http.simulations;

import simulations.Client;
import simulations.Server;

network TCPNetwork
{
    parameters:
        // TODO add global parameters (if needed)
        
    submodules:
        client: Client;
        server: Server;
        // TODO add submodules comprising a HTTPClient and HTTPServer
        
    connections:
        // TODO connect HTTPClient and HTTPServer
        client.outNet --> {delay = 100ms;} --> server.inNet;
        server.outNet --> {delay = 100ms;} --> client.inNet;

}
