enum ResponseCode {
    OK = 0;
    ERROR = 1;
}

// Define the Request message
message Request {
    string hostname; // The hostname being requested
}

// Define the Response message
message Response {
    short address[4];         // IPv4 address (4 bytes)
    ResponseCode statusCode; // Status code: OK or ERROR
}
