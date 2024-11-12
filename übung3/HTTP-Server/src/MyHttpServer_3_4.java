import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class MyHttpServer_3_4 {
    private final int tcpPort;
    private final File documentRoot;

    public MyHttpServer_3_4(int tcpPort, String documentRoot){
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
        try(ServerSocket server = new ServerSocket(tcpPort)){
            while(true){
                try(Socket client = server.accept();BufferedReader inFromClient = new BufferedReader(new InputStreamReader(client.getInputStream()))){
                    String request = inFromClient.readLine();
                    /* request: GET <path> <ignore> */
                    if(request == null||!request.startsWith("GET ")||request.length()<5){continue;}
                    String path = request.split(" ")[1];
                    File returnFile = handleRequest(path);
                    if(returnFile!=null){
                        sendFileToClient(returnFile, client);
                    }
                }catch (IOException e){
                    System.err.println(e.getMessage());
                }
            }
        }catch (IOException e){
            System.err.println(e.getMessage());
        }
    }

    /**
     * checks the given path for faulty input (trying to exit Files of MyHttpServer) and returns a valid File-Object or null
     * @param path relative path to a File or directory in the Files of the MyHttpServer
     * @return the File-Object (File) with correct path (index.html if a directory is pointed at)
     *         null if something went wrong
     */
    private File handleRequest(String path){
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

    /**
     * Sends the given file to the client by using Streams.
     * (No StreamReader/Writer, because more than just texts should be sent correctly)
     * @param file the File meant to be sent. Can be pictures or other file-types not only text
     * @param client the Socket of the client to which the file should be sent to
     */
    private void sendFileToClient(File file, Socket client) {
        try (BufferedOutputStream outToClient = new BufferedOutputStream(client.getOutputStream()); BufferedInputStream fileReader = new BufferedInputStream(new FileInputStream(file))) {
            byte[] buffer = new byte[10240]; // max 10KB per write
            int bytesRead;
            while ((bytesRead = fileReader.read(buffer)) != -1) {
                outToClient.write(buffer, 0, bytesRead);
            }
            outToClient.flush();
        } catch (IOException e) {
            System.err.println(e.getMessage());
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
        MyHttpServer_3_4 server = new MyHttpServer_3_4(port, args[1]);
        server.run();
            System.out.println("Finished!");
    }
}