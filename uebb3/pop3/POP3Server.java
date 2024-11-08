import java.io.*;
import java.net.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ExecutorService;

public class SimplePOP3Server {
    private static final int PORT = 110; // standard POP3 port
    private static SampleDataBase database = new SampleDataBase();

    public static void main(String[] args) {
        ExecutorService threadPool = Executors.newCachedThreadPool();

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("POP3 Server @ " + PORT);

            while (true) {
                Socket clientSocket = serverSocket.accept();
                threadPool.submit(new ClientHandler(clientSocket));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

class ClientHandler implements Runnable {
    private Socket clientSocket;
    private BufferedReader reader;
    private PrintWriter writer;

    public ClientHandler(Socket clientSocket) {
        this.clientSocket = clientSocket;
    }

    @Override
    public void run() {
        try {
            reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            writer = new PrintWriter(clientSocket.getOutputStream(), true);

            writer.println("+OK Simple POP3 Server ready");

            boolean authenticated = false;
            String line;
            while ((line = reader.readLine()) != null) {
                String[] commandParts = line.split(" ");
                String command = commandParts[0].toUpperCase();

                switch (command) {
                    case "USER":
                        // accept every user
                        writer.println("+OK user accepted");
                        break;
                    case "PASS":
                        // accept every password
                        writer.println("+OK pass accepted");
                        authenticated = true;
                        break;
                    case "STAT":
                        // should list a short summary of the messages
                        if (authenticated) {
                            int messageCount = SampleDataBase.messages.size(); // SampleDataBase is a ArrayList of Strings
                            int totalSize = SampleDataBase.messages.stream().mapToInt(String::length).sum(); // getting every message adding up to the total size
                            writer.println("+OK " + messageCount + " " + totalSize);
                        } else {
                            writer.println("-ERR not authenticated"); // will never happen
                        }
                        break;

                    case "LIST":
                        // list all email
                        if (authenticated) {
                            writer.println("+OK listing messages");
                            for (int i = 0; i < SampleDataBase.messages.size(); i++) {
                                writer.println((i + 1) + " " + SampleDataBase.messages.get(i).length());
                            }
                            writer.println(".");
                        } else {
                            writer.println("-ERR not authenticated"); // will never happen
                        }
                        break;
                    case "RETR":
                        // Retrieve the full message content for the specified message number
                        if (authenticated && commandParts.length > 1) {
                            int msgIndex = Integer.parseInt(commandParts[1]) - 1;
                            if (msgIndex >= 0 && msgIndex < SampleDataBase.messages.size()) {
                                String message = SampleDataBase.messages.get(msgIndex);
                                writer.println("+OK " + message.length() + " octets");

                                // Send the message line by line, ensuring \r\n at the end of each line
                                String[] lines = message.split("\n", -1);
                                for (String msgLine : lines) {
                                    writer.println(msgLine.replace("\n", "\r\n"));
                                }
                                writer.println(".");
                            } else {
                                writer.println("-ERR no such message");
                            }
                        } else {
                            writer.println("-ERR invalid command or not authenticated");
                        }
                        break;
                    case "TOP":
                        // returns header + num of specified lines of message
                        if (authenticated && commandParts.length > 2) {
                            int msgIndex = Integer.parseInt(commandParts[1]) - 1;
                            int numLines = Integer.parseInt(commandParts[2]);
                            if (msgIndex >= 0 && msgIndex < SampleDataBase.messages.size()) {
                                String message = SampleDataBase.messages.get(msgIndex);
                                String[] lines = message.split("\n", -1); // -1 means it will split until done
                                writer.println("+OK");

                                // Send header
                                boolean headerEndFound = false;
                                for (String msgLine : lines) {
                                    writer.println(msgLine.replace("\n", "\r\n")); // POP3 requires \r\n as EOL
                                    if (msgLine.isEmpty()) {
                                        headerEndFound = true;
                                        break;
                                    }
                                }

                                // Send body lines
                                if (headerEndFound) {
                                    int count = 0;
                                    for (int i = lines.length - numLines; i < lines.length && count < numLines; i++) {
                                        writer.println(lines[i].replace("\n", "\r\n"));
                                        count++;
                                    }
                                }
                                writer.println(".");
                            } else {
                                writer.println("-ERR no such message");
                            }
                        } else {
                            writer.println("-ERR invalid command or not authenticated");
                        }
                        break;

                    case "QUIT":
                        writer.println("+OK goodbye");
                        clientSocket.close();
                        return;
                    case "CAPA":
                        // returns the possible commands of server
                        writer.println("+OK Capability list follows");
                        writer.println("USER");
                        writer.println("PASS");
                        writer.println("STAT");
                        writer.println("LIST");
                        writer.println("RETR");
                        writer.println("TOP");
                        writer.println("QUIT");
                        writer.println("CAPA");
                        writer.println(".");
                        break;

                    default:
                        writer.println("-ERR unknown command");
                        break;
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
