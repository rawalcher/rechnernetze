import java.io.*;
import java.nio.charset.StandardCharsets;

public class Ex1_2_with_filter {
    public static void main(String[] args) {


        try(InputStreamReader inputReader = new InputStreamReader(new CustomFilterInputStream(new FileInputStream(args[0])), StandardCharsets.UTF_8)){
            OutputStreamWriter outputWriter = new OutputStreamWriter(new FileOutputStream(args[1]), StandardCharsets.ISO_8859_1);

            int count=0;
            int i=inputReader.read();
            while(i!=-1){
                outputWriter.write((char)i);
                //System.out.print(i+" ");
                if(i==13) {
                    count++;
                    //System.out.println("\nnow");
                }
                i=inputReader.read();
            }
            outputWriter.flush();
            inputReader.close();
            outputWriter.close();
            System.out.println("Changed "+count+" many LF to CR+LF.");
        }catch(IOException e) {
            e.printStackTrace();
        }
    }
}
class CustomFilterInputStream extends FilterInputStream{
    private int nextInt = -1;

    protected CustomFilterInputStream(InputStream in) {
        super(in);
    }

    @Override
    public int read() throws IOException {
        if(nextInt>=0){
            int temp = nextInt;
            nextInt=-1;
            return temp;
        }
        int c = super.read();
        if(c==10){
            nextInt=c;
            return 13;
        }
        return c;
    }
    @Override
    public int read(byte[] b, int off, int len) throws IOException {
        if(b.length == 0 ||b.length<=off){
            throw new IOException();
        }
        int count=0;
        int i;
        if(nextInt<0){
            i=super.read();
        }else{
            i = nextInt;
        }

        while(i!=-1&&count+off<b.length&&count!=len){
            if(nextInt>=0){
                b[off+count]= (byte) nextInt;
                nextInt=-1;
                i=super.read();
                count++;
                continue;
            }
            if(i==10){
                nextInt=i;
                b[off+count]= 13;
                count++;
                continue;
            }
            b[off+count]= (byte) i;
            i=super.read();
            count++;
        }
        if(count==0){
            return -1;
        }
        return count;


    }
}

