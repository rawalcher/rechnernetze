network simpleNet {
    parameters:
        @display("i=cloud");
    submodules:
        client: Client {
            @display("p=100,100");
        }
        server: Server {
            @display("p=300,100");
        }
    connections:
        client.gate <--> Channel <--> server.gate;
}
