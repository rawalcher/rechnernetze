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

            logger.info("POP3 Server started on port " + PORT);

            while (true) {
                Socket clientSocket = serverSocket.accept();
                threadPool.submit(new ClientHandler(clientSocket, database));
            }
        } catch (IOException e) {
            logger.log(Level.SEVERE, "Server error: " + e.getMessage(), e);
        }
    }
}

class ClientHandler implements Runnable {
    private static final Logger logger = Logger.getLogger(ClientHandler.class.getName());
    private static final String ERR_NO_SUCH_MESSAGE = "No such message";
    private static final String ERR_INVALID_MESSAGE = "Invalid message number";
    private static final String ERR_NOT_AUTHENTICATED = "Not authenticated";
    private static final String ERR_INVALID_COMMAND = "Invalid command";
    private static final String ERR_MESSAGE_DELETED = "Message marked as deleted";
    private static final String INFO_MESSAGE_DELETED = "Message already marked as deleted";

    private final Socket clientSocket;
    private BufferedReader reader;
    private BufferedWriter writer;
    private boolean authenticated = false;
    private boolean quit = false;
    private final SampleDataBase database;
    private final Set<Integer> deletedMessages = new HashSet<>();

    private final Map<String, CommandHandler> commandHandlers = new HashMap<>();

    public ClientHandler(Socket clientSocket, SampleDataBase database) {
        this.clientSocket = clientSocket;
        this.database = database;
        initializeCommandHandlers();
    }

    @FunctionalInterface
    interface CommandHandler {
        void handle(String[] args) throws IOException;
    }

    private void initializeCommandHandlers() {
        commandHandlers.put("USER", this::handleUSER);
        commandHandlers.put("PASS", this::handlePASS);
        commandHandlers.put("STAT", this::handleSTAT);
        commandHandlers.put("LIST", this::handleLIST);
        commandHandlers.put("RETR", this::handleRETR);
        commandHandlers.put("TOP", this::handleTOP);
        commandHandlers.put("DELE", this::handleDELE);
        commandHandlers.put("QUIT", this::handleQUIT);
        commandHandlers.put("CAPA", this::handleCAPA);
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

                CommandHandler handler = commandHandlers.get(command);
                if (handler != null) {
                    try {
                        handler.handle(commandParts);
                    } catch (Exception e) {
                        sendError(e.getMessage());
                        logger.log(Level.WARNING, "Error handling command [" + command + "]: " + e.getMessage(), e);
                    }
                } else {
                    handleUnknownCommand(command);
                }
            }
        } catch (IOException e) {
            logger.log(Level.SEVERE, "Connection error: " + e.getMessage(), e);
        } finally {
            try {
                clientSocket.close();
                logger.info("Client disconnected from " + clientSocket.getRemoteSocketAddress());
            } catch (IOException e) {
                logger.log(Level.WARNING, "Error closing client socket: " + e.getMessage(), e);
            }
        }
    }

    private void handleUSER(String[] args) throws IOException {
        if (args.length != 2) {
            sendError("Syntax: USER username");
            return;
        }
        authenticated = false;
        sendLine("+OK User accepted");
        logger.info("USER command accepted for user: " + args[1]);
    }

    private void handlePASS(String[] args) throws IOException {
        if (args.length != 2) {
            sendError("Syntax: PASS password");
            return;
        }
        authenticated = true;
        sendLine("+OK User authenticated");
        logger.info("User authenticated.");
    }

    private void handleSTAT(String[] args) throws IOException {
        if (!checkAuthenticated()) return;

        int messageCount;
        int totalSize = 0;

        synchronized (database) {
            messageCount = database.messages.size() - deletedMessages.size();
            for (int i = 0; i < database.messages.size(); i++) {
                if (!deletedMessages.contains(i)) {
                    totalSize += database.messages.get(i).length();
                }
            }
        }

        sendLine("+OK " + messageCount + " " + totalSize);
        logger.info("STAT command - " + messageCount + " messages, total size: " + totalSize);
    }

    private void handleLIST(String[] args) throws IOException {
        if (!checkAuthenticated()) return;

        if (args.length == 1) {
            listAllMessages();
        } else if (args.length == 2) {
            listSingleMessage(args[1]);
        } else {
            sendError("Invalid arguments");
            logger.warning("LIST command - Invalid arguments.");
        }
    }

    private void handleRETR(String[] args) throws IOException {
        if (!checkAuthenticated()) return;

        int msgIndex = parseMessageIndex(args);
        if (msgIndex == -1) return;

        synchronized (database) {
            if (!isValidMessageIndex(msgIndex)) {
                sendError(ERR_NO_SUCH_MESSAGE);
                logger.warning("RETR command - No such message");
                return;
            }
            if (isMessageDeleted(msgIndex)) {
                sendError(ERR_MESSAGE_DELETED);
                logger.warning("RETR command - " + ERR_MESSAGE_DELETED);
                return;
            }
            String message = database.messages.get(msgIndex);
            sendLine("+OK " + message.length() + " octets");
            logger.info("RETR command - Retrieving message " + (msgIndex + 1));
            sendMessageContent(message);
        }
    }

    private void handleDELE(String[] args) throws IOException {
        if (!checkAuthenticated()) return;

        int msgIndex = parseMessageIndex(args);
        if (msgIndex == -1) return;

        synchronized (database) {
            if (!isValidMessageIndex(msgIndex)) {
                sendError(ERR_NO_SUCH_MESSAGE);
                logger.warning("DELE command - No such message");
                return;
            }
            if (deletedMessages.contains(msgIndex)) {
                sendError(INFO_MESSAGE_DELETED);
                logger.warning("DELE command - " + INFO_MESSAGE_DELETED);
                return;
            }
            deletedMessages.add(msgIndex);
            sendLine("+OK Message " + (msgIndex + 1) + " marked for deletion");
            logger.info("DELE command - Marked message " + (msgIndex + 1) + " for deletion");
        }
    }

    private void handleQUIT(String[] args) throws IOException {
        sendLine("+OK Goodbye");
        logger.info("QUIT command - Client disconnected.");

        synchronized (database) {
            List<Integer> toDelete = new ArrayList<>(deletedMessages);
            Collections.sort(toDelete, Collections.reverseOrder());
            for (int index : toDelete) {
                database.messages.remove(index);
            }
        }
        quit = true;
    }

    private void handleCAPA(String[] args) throws IOException {
        sendLine("+OK Capability list follows");
        for (String command : commandHandlers.keySet()) {
            sendLine(command);
        }
        sendLine(".");
        logger.info("CAPA command - Capability list sent.");
    }

    private void handleTOP(String[] args) throws IOException {
        if (!checkAuthenticated()) return;

        if (args.length != 3) {
            sendError("Syntax: TOP message_number lines");
            return;
        }

        int msgIndex;
        int lines;
        try {
            msgIndex = Integer.parseInt(args[1]) - 1;
            lines = Integer.parseInt(args[2]);
        } catch (NumberFormatException e) {
            sendError("Invalid message number or line count");
            return;
        }

        synchronized (database) {
            if (!isValidMessageIndex(msgIndex)) {
                sendError(ERR_NO_SUCH_MESSAGE);
                return;
            }
            if (isMessageDeleted(msgIndex)) {
                sendError(ERR_MESSAGE_DELETED);
                return;
            }
            sendHeadersAndLines(msgIndex, lines);
        }
    }

    private void handleUnknownCommand(String command) throws IOException {
        sendError("Unknown command");
        logger.warning("Unknown command: " + command);
    }

    private void listAllMessages() throws IOException {
        sendLine("+OK scan listing follows");
        logger.info("LIST command - Listing all messages.");

        synchronized (database) {
            for (int i = 0; i < database.messages.size(); i++) {
                if (!deletedMessages.contains(i)) {
                    sendLine(formatMessageEntry(i));
                }
            }
        }
        sendLine(".");
    }

    private void listSingleMessage(String messageNumber) throws IOException {
        int msgIndex;
        try {
            msgIndex = Integer.parseInt(messageNumber) - 1;
        } catch (NumberFormatException e) {
            sendError(ERR_INVALID_MESSAGE);
            return;
        }

        synchronized (database) {
            if (!isValidMessageIndex(msgIndex)) {
                sendError(ERR_NO_SUCH_MESSAGE);
                return;
            }
            if (isMessageDeleted(msgIndex)) {
                sendError(ERR_MESSAGE_DELETED);
                return;
            }
            sendSingleMessageEntry(msgIndex);
        }
    }

    private void sendSingleMessageEntry(int msgIndex) throws IOException {
        String response = String.format("+OK %d %d", msgIndex + 1, database.messages.get(msgIndex).length());
        sendLine(response);
        logger.info("LIST command - Listing message " + (msgIndex + 1));
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
                sendLine("");
                continue;
            }
            sendLine(applyDotStuffing(msgLine));
            if (bodyStarted && ++lineCount >= lines) {
                break;
            }
        }
        sendLine(".");
    }

    private boolean checkAuthenticated() throws IOException {
        if (!authenticated) {
            sendError(ERR_NOT_AUTHENTICATED);
            return false;
        }
        return true;
    }

    private int parseMessageIndex(String[] args) throws IOException {
        if (args.length < 2) {
            sendError("No message number specified");
            return -1;
        }
        try {
            return Integer.parseInt(args[1]) - 1;
        } catch (NumberFormatException e) {
            sendError(ERR_INVALID_MESSAGE);
            return -1;
        }
    }

    private boolean isValidMessageIndex(int index) {
        return index >= 0 && index < database.messages.size();
    }

    private boolean isMessageDeleted(int index) {
        return deletedMessages.contains(index);
    }

    private String formatMessageEntry(int index) {
        return String.format("%d %d", index + 1, database.messages.get(index).length());
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

    private void sendError(String errorMessage) throws IOException {
        sendLine("-ERR " + errorMessage);
    }
}