import java.io.*;
import java.net.*;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import static java.lang.Thread.sleep;

public class MyHttpServer_3_5 {
    private final int tcpPort;
    private final File documentRoot;
    private final List<LogEntry> log = Collections.synchronizedList(new ArrayList<>());

    public class LogEntry{
        private LocalDateTime timestamp;
        private String request;
        private InetAddress clientIP;
        private int clientPort;
        public LogEntry(LocalDateTime timestamp, String request, InetAddress clientIP, int clientPort){
            this.timestamp = timestamp;
            this.request = request;
            this.clientIP = clientIP;
            this.clientPort = clientPort;
        }
        public String getStringRepresentation(){
            return timestamp.format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:s"))+" "+request+" "+clientIP.getHostAddress()+" "+clientPort;
        }
    }

    public MyHttpServer_3_5(int tcpPort, String documentRoot){
        this.tcpPort = tcpPort;
        this.documentRoot = new File(documentRoot);
    }

    public void run(){
        /* check Valid Input */
        if(!this.documentRoot.exists()){
            System.err.println("Document root directory does not exist.");
            return;
        }else if(!this.documentRoot.isDirectory()){
            System.err.println("Document root directory is not a directory.");
            return;
        }

        /* run Server */
        Thread logger = new Thread(new Logger(this.log));
        logger.start();
        try(ServerSocket server = new ServerSocket(tcpPort)){
            while(true){
                Socket client = server.accept();
                Thread clientThread = new Thread(new HandleClientThread(client));
                clientThread.start();
            }
        }catch (IOException e){
            System.err.println(e.toString());
        }
    }

    public class HandleClientThread implements Runnable {
        private final Socket clientSocket;
        private boolean keepAlive = false;
        public HandleClientThread(Socket clientSocket) {
            this.clientSocket = clientSocket;
        }
        private int versionMinOne(String input){
            if(input==null||!input.contains(".")){return -1;}
            String[] partsInput = input.split("\\.");
            if(partsInput.length!=2||Integer.parseInt(partsInput[1])<0){return -1;}
            return Integer.parseInt(partsInput[0].split("\\/")[1])>=1 ? 1 : 0;
        }
        private File getRequestedFile(String path){
            File requestFile = new File(documentRoot, path);
            if(path.contains("..")||!requestFile.getAbsolutePath().startsWith(documentRoot.getAbsolutePath())){
                return null; // client trying to reach documents outside Servers allowance(document_root)
            }
            if(requestFile.exists()){
                if(requestFile.isDirectory()){
                    requestFile = new File(requestFile, "index.html");
                    return requestFile.exists() ? requestFile : null;
                } else if (requestFile.isFile()) {
                    return requestFile;
                }
            }
            return null;
        }
        private void sendFileSimpleToClient(File file) {
            try (BufferedOutputStream outToClient = new BufferedOutputStream(clientSocket.getOutputStream()); BufferedInputStream fileReader = new BufferedInputStream(new FileInputStream(file))) {
                byte[] buffer = new byte[10240]; // max 10KB per write
                int bytesRead;
                while ((bytesRead = fileReader.read(buffer)) != -1) {
                    outToClient.write(buffer, 0, bytesRead);
                }
                outToClient.flush();
            } catch (IOException e) {
                System.err.println(e.toString());
            }
        }
        private List<String> readHeader(BufferedReader input) throws IOException {
            List<String> result = new ArrayList<>();
            String newLine = input.readLine();
            while(!newLine.isEmpty()){
                result.add(newLine);
                newLine = input.readLine();
            }
            return result;
        }
        private boolean isKeepAlive(List<String> headers){
            for(String header : headers){
                if(header.startsWith("Connection")&&header.equals("Connection: Keep-Alive")){
                    return true;
                }
            }
            return false;
        }
        private int getContentLength(List<String> headers){
            for(String header : headers){
                if(header.startsWith("Content-Length: ")){
                    return Integer.parseInt(header.substring(16));
                }
            }
            return -1;
        }
        private void ignoreBody(Socket input, int lengthInBytes) throws IOException {
            byte[] entityBody = new byte[lengthInBytes];
            int bytesRead = input.getInputStream().read(entityBody, 0, lengthInBytes);
            if (bytesRead != lengthInBytes) {
                System.err.println("Fehler: Entity-Body konnte nicht vollständig gelesen werden");
            }
        }
        private void sendError(Socket clientSocket, int errorCode){
            try(BufferedWriter outToClient = new BufferedWriter(new OutputStreamWriter(new BufferedOutputStream(clientSocket.getOutputStream())))){
                String errorMessage;
                switch (errorCode){
                    case 501:
                        errorMessage="Not Implemented";
                        break;
                    case 404:
                        errorMessage = "Not Found";
                        break;
                    default:
                        System.err.println("Error is not 501 or 404!");
                        return;
                }
                String statusLine = "HTTP/1.0 "+errorCode+" "+errorMessage+"\r\n";
                outToClient.write(statusLine);
                if(keepAlive){
                    outToClient.write("Connection: Keep-Alive\r\n");
                }else{
                    outToClient.write("Connection: close\r\n");
                }
                outToClient.flush();
            } catch (IOException e) {
                System.err.println(e.toString());
            }
        }
        private void sendFileFull(Socket clientSocket, File file) throws IOException {
            String statusLine = "HTTP/1.0 200 OK\r\n";
            List<String> headers = new ArrayList<>();
            headers.add("Content-Length: "+file.length()+"\r\n");
            headers.add("Content-Type: "+URLConnection.guessContentTypeFromName(file.getName())+"\r\n"); //library to get type of file in right format
            if(keepAlive){
                headers.add("Connection: Keep-Alive\r\n");
            }else{
                headers.add("Connection: close\r\n");
            }
            try (BufferedInputStream fileReader = new BufferedInputStream(new FileInputStream(file))) {
                BufferedOutputStream outToClient = new BufferedOutputStream(clientSocket.getOutputStream());
                outToClient.write(statusLine.getBytes());
                for(String header : headers){
                    outToClient.write(header.getBytes());
                }
                outToClient.write("\r\n".getBytes());
                outToClient.flush();
                /* send File */
                byte[] buffer = new byte[10240]; // max 10KB per write
                int bytesRead;
                while ((bytesRead = fileReader.read(buffer)) != -1) {
                    outToClient.write(buffer, 0, bytesRead);
                }
                outToClient.flush();
            } catch (IOException e) {
                System.err.println(e.toString());
            }
        }
        private void log(Socket clientSocket, String request){
            synchronized (log) {
                log.add(new LogEntry(LocalDateTime.now(), request, clientSocket.getInetAddress(), clientSocket.getPort()));
            }
        }
        @Override
        public void run() {
            try(BufferedReader inFromClient = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()))){
                do{
                    String firstLine = inFromClient.readLine();
                    if(keepAlive&&firstLine==null){
                        clientSocket.close();
                        return;
                    }
                        if (firstLine == null) {
                            System.err.println("requestLine is null");
                            return;
                        }
                    String[] requestLine = firstLine.split(" ");
                    log(clientSocket, firstLine);
                    if (requestLine.length < 2) {
                        System.err.println("Too few items in requestLine" + firstLine);
                        return;
                    }
                    if (requestLine.length == 2 || versionMinOne(requestLine[2]) == 0) { //HTTP 0.9
                        clientSocket.close();
                        if (requestLine[0].equals("GET")) {
                            File returnFile = getRequestedFile(requestLine[1]);
                            if (returnFile != null) {
                                sendFileSimpleToClient(returnFile);
                            }
                        }
                        keepAlive=false;
                    } else { //HTTP 1.0
                        List<String> header = readHeader(inFromClient);
                        this.keepAlive = isKeepAlive(header);
                        if (getContentLength(header) > 0) { //client sends Entity (with body)
                            ignoreBody(clientSocket, getContentLength(header));
                        }
                        if (!requestLine[0].equals("GET")) { // return Err 501 not Implemented
                            sendError(clientSocket, 501);
                        } else {
                            File returnFile = getRequestedFile(requestLine[1]);
                            if (returnFile == null) { //return Err 404
                                sendError(clientSocket, 404);
                            } else {
                                //return normal entity-response
                                sendFileFull(clientSocket, returnFile);
                            }
                        }
                        /* keepalive */
                        if (keepAlive) {
                            clientSocket.setSoTimeout(5000);//set max wait time to 5 sec (30 000 ms)
                        }else{
                            clientSocket.close();
                        }
                    }
                }while (keepAlive);
            }catch (Exception e){
                System.err.println(e.getMessage());
                if(e instanceof SocketTimeoutException){
                    try {
                        clientSocket.close();
                    } catch (IOException ex) {
                        throw new RuntimeException(ex);
                    }
                }
            }
        }
    }
    public class Logger implements Runnable{
        private List<LogEntry> logBook;
        public Logger(List<LogEntry> logBook){
            this.logBook = logBook;
        }
        @Override
        public void run() {
            while(true){
                System.out.println("Log:");
                synchronized (logBook) {
                    for (LogEntry entry : logBook) {
                        System.out.println(entry.getStringRepresentation());
                    }
                }
                try {
                    sleep(5000);
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }

    public static void main(String[] args){
        if(args.length!=2){
            System.err.println("Wrong number of arguments");
            return;
        }
        int port;
        try{
            port = Integer.parseInt(args[0]);
        }catch (Exception e){
            System.err.println("Invalid port input");
            return;
        }
        MyHttpServer_3_5 server = new MyHttpServer_3_5(port, args[1]);
        server.run();
        System.out.println("Finished!");
    }
}