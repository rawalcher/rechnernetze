import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

public class POP3Client {
    private final String server;
    private static final int port = 110;
    private final String username;
    private final String password;
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
        String response = reader.readLine();
        System.out.println("Server: " + response);
        if (!response.startsWith("+OK")) {
            throw new IOException("Failed to connect to the server.");
        }

        // Send USER and PASS commands
        writer.println("USER " + username);
        response = reader.readLine();
        System.out.println("Server: " + response);
        if (!response.startsWith("+OK")) {
            throw new IOException("Invalid username.");
        }

        writer.println("PASS " + password);
        response = reader.readLine();
        System.out.println("Server: " + response);
        if (!response.startsWith("+OK")) {
            throw new IOException("Invalid password.");
        }
    }

    public void listMessages() throws IOException {
        writer.println("LIST");
        String response = reader.readLine();
        System.out.println("Server: " + response);
        if (response.startsWith("+OK")) {
            while (!(response = reader.readLine()).equals(".")) {
                System.out.println(response);
            }
        } else {
            System.out.println("Error listing messages: " + response);
        }
    }

    public void retrieveMessage(int messageNumber) throws IOException {
        writer.println("RETR " + messageNumber);
        String response = reader.readLine();
        System.out.println("Server: " + response);
        if (response.startsWith("+OK")) {
            while (!(response = reader.readLine()).equals(".")) {
                System.out.println(response);
            }
        } else {
            System.out.println("Error retrieving message: " + response);
        }
    }

    public void deleteMessage(int messageNumber) throws IOException {
        writer.println("DELE " + messageNumber);
        String response = reader.readLine();
        System.out.println("Server: " + response);
        if (!response.startsWith("+OK")) {
            System.out.println("Error deleting message: " + response);
        }
    }

    public void disconnect() throws IOException {
        writer.println("QUIT");
        String response = reader.readLine();
        System.out.println("Server: " + response);
        socket.close();
    }

    public void run() {
        try (Scanner scanner = new Scanner(System.in)) {
            connect();
            boolean quit = false;
            while (!quit) {
                System.out.println("\nChoose an option:");
                System.out.println("1. List Messages");
                System.out.println("2. Retrieve Message");
                System.out.println("3. Delete Message");
                System.out.println("4. Quit");
                System.out.print("Enter choice (1-4): ");
                String choice = scanner.nextLine();

                switch (choice) {
                    case "1":
                        listMessages();
                        break;
                    case "2":
                        System.out.print("Enter message number to retrieve: ");
                        String msgNumStr = scanner.nextLine();
                        try {
                            int msgNum = Integer.parseInt(msgNumStr);
                            retrieveMessage(msgNum);
                        } catch (NumberFormatException e) {
                            System.out.println("Invalid message number.");
                        }
                        break;
                    case "3":
                        System.out.print("Enter message number to delete: ");
                        String delNumStr = scanner.nextLine();
                        try {
                            int delNum = Integer.parseInt(delNumStr);
                            deleteMessage(delNum);
                        } catch (NumberFormatException e) {
                            System.out.println("Invalid message number.");
                        }
                        break;
                    case "4":
                        disconnect();
                        quit = true;
                        break;
                    default:
                        System.out.println("Invalid choice. Please select an option between 1 and 4.");
                        break;
                }
            }
        } catch (IOException e) {
            System.err.println("Connection error: " + e.getMessage());
        } finally {
            // Ensure resources are closed
            try {
                if (socket != null && !socket.isClosed()) {
                    disconnect();
                }
            } catch (IOException e) {
                // Ignore
            }
        }
    }

    public static void main(String[] args) {
        String server = "localhost";
        String username = "username";
        String password = "password";

        POP3Client client = new POP3Client(server, username, password);
        client.run();
    }
}