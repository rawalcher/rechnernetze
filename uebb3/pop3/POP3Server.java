import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ExecutorService;
import java.util.logging.Level;
import java.util.logging.Logger;

public class POP3Server {
    private static final int PORT = 110; // standard POP3 port
    private static final SampleDataBase database = new SampleDataBase();
    private static final Logger logger = Logger.getLogger(POP3Server.class.getName());

    public static void main(String[] args) {
        ExecutorService threadPool = Executors.newCachedThreadPool();

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {

            logger.info("POP3 Server @ " + PORT);

            while (true) {
                Socket clientSocket = serverSocket.accept();
                threadPool.submit(new ClientHandler(clientSocket, database));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

class ClientHandler implements Runnable {
    private static final Logger logger = Logger.getLogger(ClientHandler.class.getName());
    public static final String errMessageMarkedDeleted = "Message marked as deleted";

    private final Socket clientSocket;
    private BufferedReader reader;
    private BufferedWriter writer;
    private boolean authenticated = false;
    private boolean quit = false;
    private final SampleDataBase database;
    private final Set<Integer> deletedMessages = new HashSet<>();

    private static final String errNoSuchMessage = "No such message";
    private static final String errInvalidMessage = "Invalid message number";
    private static final String errInvalidCommandOrNotAuthenticated = "Invalid command or not authenticated";
    private static final String infoMarkedDel = "marked as deleted";


    public ClientHandler(Socket clientSocket, SampleDataBase database) {
        this.clientSocket = clientSocket;
        this.database = database;
    }

    private void sendLine(String message) throws IOException {
        writer.write(message + "\r\n");
        writer.flush();
        logger.fine("Sent: [" + message + "]");
    }

    @Override
    public void run() {
        try {
            reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream(), StandardCharsets.US_ASCII));
            writer = new BufferedWriter(new OutputStreamWriter(clientSocket.getOutputStream(), StandardCharsets.US_ASCII));

            logger.info("Client connected from " + clientSocket.getRemoteSocketAddress());
            sendLine("+OK Simple POP3 Server ready");

            String line;

            while (!quit && (line = reader.readLine()) != null) {
                logger.fine("Received command: [" + line + "]");
                String[] commandParts = line.split(" ");
                String command = commandParts[0].toUpperCase();

                try {
                    switch (command) {
                        case "USER":
                            handleUSER(commandParts);
                            break;

                        case "PASS":
                            handlePASS(commandParts);
                            break;

                        case "STAT":
                            handleSTAT();
                            break;

                        case "LIST":
                            handleLIST(commandParts);
                            break;

                        case "RETR":
                            handleRETR(commandParts);
                            break;

                        case "TOP":
                            handleTOP(commandParts);
                            break;

                        case "DELE":
                            handleDELE(commandParts);
                            break;

                        case "QUIT":
                            handleQUIT();
                            break;

                        case "CAPA":
                            handleCAPA();
                            break;

                        default:
                            handleUnknownCommand(command);
                            break;
                    }
                } catch (Exception e) {
                    sendError(e.getMessage());
                    logger.log(Level.WARNING, "Error handling command [" + command + "]: " + e.getMessage(), e);
                }
            }
        } catch (IOException e) {
            logger.log(Level.SEVERE,
                    "Connection error: " + e.getMessage(),
                    e);
        } finally {
            try {
                clientSocket.close();
                logger.info("Client disconnected from " + clientSocket.getRemoteSocketAddress());
            } catch (IOException e) {
                logger.log(Level.WARNING, "Error closing client socket: " + e.getMessage(), e);
            }
        }
    }

    private void handleUSER(String[] commandParts) throws IOException {
        if (commandParts.length != 2) {
            sendError("Syntax: USER username");
            return;
        }
        // Accept any username for simplicity
        authenticated = false;
        sendLine("+OK User accepted");
        logger.info("USER command accepted for user: " + commandParts[1]);
    }

    private void handlePASS(String[] commandParts) throws IOException {
        if (commandParts.length != 2) {
            sendError("Syntax: PASS password");
            return;
        }
        // Accept any password for simplicity
        authenticated = true;
        sendLine("+OK User authenticated");
        logger.info("User authenticated.");
    }

    private void handleSTAT() throws IOException {
        if (authenticated) {
            int messageCount = database.messages.size() - deletedMessages.size();
            int totalSize = 0;
            for (int i = 0; i < database.messages.size(); i++) {
                if (!deletedMessages.contains(i)) {
                    totalSize += database.messages.get(i).length();
                }
            }
            sendLine("+OK " + messageCount + " " + totalSize);
            logger.info("STAT command - " + messageCount + " messages, total size: " + totalSize);
        } else {
            sendError("Not authenticated");
            logger.warning("STAT command - Not authenticated.");
        }
    }

    private void handleLIST(String[] commandParts) throws IOException {
        if (!isAuthenticated()) {
            sendError("Not authenticated");
            logger.warning("LIST command - Not authenticated.");
            return;
        }

        if (commandParts.length == 1) {
            listAllMessages();
        } else if (commandParts.length == 2) {
            listSingleMessage(commandParts[1]);
        } else {
            sendError("Invalid arguments");
            logger.warning("LIST command - Invalid arguments.");
        }
    }

    private boolean isAuthenticated() {
        return authenticated;
    }

    private void sendError(String errorMessage) throws IOException {
        sendLine("-ERR " + errorMessage);
    }

    private void listAllMessages() throws IOException {
        sendLine("+OK Listing messages");
        logger.info("LIST command - Listing all messages.");

        for (int i = 0; i < database.messages.size(); i++) {
            if (!isMessageDeleted(i)) {
                sendLine(formatMessageEntry(i));
            }
        }
        sendLine(".");
    }

    private String formatMessageEntry(int index) {
        return String.format("%d %d", index + 1, database.messages.get(index).length());
    }

    private void listSingleMessage(String messageNumber) throws IOException {
        int msgIndex;
        try {
            msgIndex = parseMessageIndex(messageNumber) - 1;
        } catch (NumberFormatException e) {
            sendError(errInvalidMessage);
            logger.warning("LIST command - Invalid message number.");
            return;
        }

        if (!isValidMessageIndex(msgIndex)) {
            sendError(errNoSuchMessage);
            logger.warning("LIST command - No such message.");
            return;
        }

        if (isMessageDeleted(msgIndex)) {
            sendError(errMessageMarkedDeleted);
            logger.warning("LIST command - Message " + (msgIndex + 1) + infoMarkedDel);
            return;
        }

        sendSingleMessageEntry(msgIndex);
    }

    private boolean isValidMessageIndex(int msgIndex) {
        return msgIndex >= 0 && msgIndex < database.messages.size();
    }


    private void sendSingleMessageEntry(int msgIndex) throws IOException {
        String response = String.format("+OK %d %d", msgIndex + 1, database.messages.get(msgIndex).length());
        sendLine(response);
        logger.info("LIST command - Listing message " + (msgIndex + 1));
    }

    private void handleRETR(String[] commandParts) throws IOException {
        if (authenticated && commandParts.length == 2) {
            try {
                int msgIndex = Integer.parseInt(commandParts[1]) - 1;
                if (msgIndex >= 0 && msgIndex < database.messages.size()) {
                    if (deletedMessages.contains(msgIndex)) {
                        sendError(errMessageMarkedDeleted);
                        logger.warning("RETR command - Message " + (msgIndex + 1) + infoMarkedDel);
                        return;
                    }
                    String message = database.messages.get(msgIndex);
                    sendLine("+OK " + message.length() + " octets");
                    logger.info("RETR command - Retrieving message " + (msgIndex + 1));
                    sendMessageContent(message);
                } else {
                    sendError(errNoSuchMessage);
                    logger.warning("RETR command - No such message");
                }
            } catch (NumberFormatException e) {
                sendError(errInvalidMessage);
                logger.warning("RETR command - Invalid message number");
            }
        } else {
            sendError(errInvalidCommandOrNotAuthenticated);
            logger.warning("RETR command - Invalid command or not authenticated.");
        }
    }

    private void handleDELE(String[] commandParts) throws IOException {
        if (authenticated && commandParts.length == 2) {
            try {
                int msgIndex = Integer.parseInt(commandParts[1]) - 1;
                if (msgIndex >= 0 && msgIndex < database.messages.size()) {
                    if (deletedMessages.contains(msgIndex)) {
                        sendError("Message already marked as deleted");
                        logger.warning("DELE command - Message " + (msgIndex + 1) + " already marked as deleted");
                        return;
                    }
                    deletedMessages.add(msgIndex);
                    sendLine("+OK Message " + (msgIndex + 1) + " marked for deletion");
                    logger.info("DELE command - Marked message " + (msgIndex + 1) + " for deletion");
                } else {
                    sendError(errNoSuchMessage);
                    logger.warning("DELE command - No such message");
                }
            } catch (NumberFormatException e) {
                sendError(errInvalidMessage);
                logger.warning("DELE command - Invalid message number");
            }
        } else {
            sendError(errInvalidCommandOrNotAuthenticated);
            logger.warning("DELE command - Invalid command or not authenticated.");
        }
    }

    private void handleQUIT() throws IOException {
        sendLine("+OK Goodbye");
        logger.info("QUIT command - Client disconnected.");

        // Physically remove deleted messages
        // Remove messages in reverse order to avoid index shifting
        List<Integer> toDelete = new ArrayList<>(deletedMessages);
        Collections.sort(toDelete, Collections.reverseOrder());
        for (int index : toDelete) {
            database.messages.remove(index);
        }
        quit = true;
    }

    private void handleCAPA() throws IOException {
        sendLine("+OK Capability list follows");
        sendLine("USER");
        sendLine("PASS");
        sendLine("STAT");
        sendLine("LIST");
        sendLine("RETR");
        sendLine("DELE");
        sendLine("QUIT");
        sendLine("CAPA");
        sendLine("TOP");
        sendLine(".");
        logger.info("CAPA command - Capability list sent.");
    }

    private void handleUnknownCommand(String command) throws IOException {
        sendError("Unknown command");
        logger.warning("Unknown command: " + command);
    }

    private void sendMessageContent(String message) throws IOException {
        String[] lines = message.split("\r?\n", -1);
        for (String msgLine : lines) {
            sendLine(applyDotStuffing(msgLine));
        }
        sendLine(".");
    }

    private String applyDotStuffing(String line) {
        if (line.startsWith(".")) {
            return "." + line;
        }
        return line;
    }

        private void handleTOP(String[] commandParts) throws IOException {
            if (!isValidCommand(commandParts)) {
                sendError(errInvalidCommandOrNotAuthenticated);
                logger.warning("TOP command - Invalid command or not authenticated.");
                return;
            }

            int msgIndex;
            int lines;
            try {
                msgIndex = parseMessageIndex(commandParts[1]) - 1;
                lines = parseLineCount(commandParts[2]);
            } catch (NumberFormatException e) {
                sendError("Invalid message number or line count");
                logger.warning("TOP command - Invalid message number or line count");
                return;
            }

            if (!isMessageIndexValid(msgIndex)) {
                sendError(errNoSuchMessage);
                logger.warning("TOP command - No such message");
                return;
            }

            if (isMessageDeleted(msgIndex)) {
                sendError(errMessageMarkedDeleted);
                logger.warning("TOP command - Message " + (msgIndex + 1) + infoMarkedDel);
                return;
            }

            sendHeadersAndLines(msgIndex, lines);
        }

        private boolean isValidCommand(String[] commandParts) {
            return authenticated && commandParts.length == 3;
        }

        private int parseMessageIndex(String indexStr) throws NumberFormatException {
            return Integer.parseInt(indexStr);
        }

        private int parseLineCount(String linesStr) throws NumberFormatException {
            return Integer.parseInt(linesStr);
        }

        private boolean isMessageIndexValid(int msgIndex) {
            return msgIndex >= 0 && msgIndex < database.messages.size();
        }

        private boolean isMessageDeleted(int msgIndex) {
            return deletedMessages.contains(msgIndex);
        }

        private void sendHeadersAndLines(int msgIndex, int lines) throws IOException {
            String message = database.messages.get(msgIndex);
            sendLine("+OK");
            logger.info("TOP command - Retrieving headers and first " + lines + " lines of message " + (msgIndex + 1));

            String[] messageLines = message.split("\r?\n", -1);
            boolean bodyStarted = false;
            int lineCount = 0;

            for (String msgLine : messageLines) {
                if (msgLine.isEmpty()) {
                    bodyStarted = true;
                }
                sendLine(applyDotStuffing(msgLine));
                if (bodyStarted && ++lineCount >= lines) {
                    break;
                }
            }
            sendLine(".");
        }
    }
