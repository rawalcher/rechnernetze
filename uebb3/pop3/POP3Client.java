import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class POP3Client {
    private String server;
    private int port = 110; // Default POP3 port
    private String username;
    private String password;
    private Socket socket;
    private BufferedReader reader;
    private PrintWriter writer;

    public POP3Client(String server, String username, String password) {
        this.server = server;
        this.username = username;
        this.password = password;
    }

    public void connect() throws IOException {
        socket = new Socket(server, port);
        reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        writer = new PrintWriter(socket.getOutputStream(), true);

        // Read initial server greeting
        System.out.println("Server: " + reader.readLine());

        // Send USER and PASS commands
        writer.println("USER " + username);
        System.out.println("Response: " + reader.readLine());

        writer.println("PASS " + password);
        System.out.println("Response: " + reader.readLine());
    }

    public void listMessages() throws IOException {
        writer.println("LIST");
        String response;
        while (!(response = reader.readLine()).equals(".")) {
            System.out.println(response);
        }
    }

    public void retrieveMessage(int messageNumber) throws IOException {
        writer.println("RETR " + messageNumber);
        String response;
        while (!(response = reader.readLine()).equals(".")) {
            System.out.println(response);
        }
    }

    public void disconnect() throws IOException {
        writer.println("QUIT");
        System.out.println("Response: " + reader.readLine());
        socket.close();
    }

    public static void main(String[] args) {
        try {
            POP3Client client = new POP3Client("localhost", "username", "password");
            client.connect();
            client.listMessages();
            client.retrieveMessage(1); // Retrieve the first message as an example
            client.disconnect();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}