simple Client {
    parameters:
        int n;                // Number of requests
        double intReq;        // Mean interval between requests
        @display("i=device/pc");
    gates:
        inout gate;
}
