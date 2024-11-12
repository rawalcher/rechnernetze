import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

public class POP3Client {
    private final String server; // localhost
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

    public void handleCommand(String command) throws IOException {
        writer.println(command);
        String response = reader.readLine();
        System.out.println("Server: " + response);

        String upperCommand = command.toUpperCase();
        if (response.startsWith("+OK")) {
            boolean isMultiLineResponse = false;

            if (upperCommand.startsWith("LIST")) {
                if (upperCommand.trim().equals("LIST")) {
                    isMultiLineResponse = true;
                }
            } else if (upperCommand.startsWith("RETR") || upperCommand.startsWith("TOP") || upperCommand.equals("CAPA")) {
                isMultiLineResponse = true;
            }

            if (isMultiLineResponse) {
                // Read lines until the end marker '.'
                while ((response = reader.readLine()) != null && !response.equals(".")) {
                    System.out.println("Server: " + response);
                }
            }
        } else {
            // Handle error or unexpected response
            System.out.println("Server Error or Unexpected Response: " + response);
        }
    }

    public void run() {
        try (Scanner scanner = new Scanner(System.in)) {
            connect();
            boolean quit = false;
            while (!quit) {
                System.out.println("\nChoose an option:");
                System.out.println("1. CAPA - List all supported commands");
                System.out.println("2. STAT - Show the number and total size of all messages");
                System.out.println("3. LIST [optional: message number] - List all messages with their sizes or specify a message number to see details of one message");
                System.out.println("4. RETR <message number> - Retrieve the full content of a specified message");
                System.out.println("5. TOP <message number> <line count> - Retrieve headers and a specified number of lines from a message");
                System.out.println("6. DELE <message number> - Mark a message for deletion by its ID");
                System.out.println("7. QUIT - Quit");
                System.out.print("Enter choice (1-7): ");
                String choice = scanner.nextLine();

                switch (choice) {
                    case "1":  // CAPA
                        handleCommand("CAPA");
                        break;
                    case "2":  // STAT
                        handleCommand("STAT");
                        break;
                    case "3":  // LIST with optional message number
                        System.out.print("Enter message number (or 0 for all messages): ");
                        String listNumStr = scanner.nextLine();
                        try {
                            int listNum = Integer.parseInt(listNumStr);
                            if (listNum == 0) {
                                handleCommand("LIST");
                            } else {
                                handleCommand("LIST " + listNum);
                            }
                        } catch (NumberFormatException e) {
                            System.out.println("Invalid message number.");
                        }
                        break;
                    case "4":  // RETR with message number
                        System.out.print("Enter message number to retrieve: ");
                        String msgNumStr = scanner.nextLine();
                        try {
                            int msgNum = Integer.parseInt(msgNumStr);
                            handleCommand("RETR " + msgNum);
                        } catch (NumberFormatException e) {
                            System.out.println("Invalid message number.");
                        }
                        break;
                    case "5": // TOP with message number and number of lines
                        System.out.print("Enter message number to retrieve: ");
                        msgNumStr = scanner.nextLine();
                        System.out.print("Enter number of lines to retrieve: ");
                        String numLineStr = scanner.nextLine();
                        try {
                            int msgNum = Integer.parseInt(msgNumStr);
                            int numLines = Integer.parseInt(numLineStr);
                            handleCommand("TOP " + msgNum + " " + numLines);
                        } catch (NumberFormatException e) {
                            System.out.println("Invalid input. Both message number and line count must be integers.");
                        }
                        break;
                    case "6":  // DELE with message number
                        System.out.print("Enter message number to delete: ");
                        String delNumStr = scanner.nextLine();
                        try {
                            int delNum = Integer.parseInt(delNumStr);
                            handleCommand("DELE " + delNum);
                        } catch (NumberFormatException e) {
                            System.out.println("Invalid message number.");
                        }
                        break;
                    case "7":  // QUIT
                        handleCommand("QUIT");
                        quit = true;
                        break;
                    default:
                        System.out.println("Invalid choice. Please select an option between 1 and 7.");
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