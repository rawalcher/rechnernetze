import java.io.*;

public class TheRealThing extends Thread {
    private static float result = 0;
    private String filename;
    private int start;
    private int end;
    private static final int NUM_ITERATIONS = 1000;
    /**
     * Creates a new TheRealThing thread which operates
     * on the indexes start to end.
     */
    public TheRealThing(String filename, int start, int end) {
        this.filename = filename;
        this.start = start;
        this.end = end;
    }
    /**
     * Performs "eine komplizierte Berechnung" on array and
     * returns the result
     */
    public float eine_komplizierte_Berechnung(float[] array) {
       float[] tempArray = array.clone();
        float totalChange =0;

        for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
            for (int i = 1; i < array.length - 1; i++) {
                float average = (tempArray[i - 1] + tempArray[i + 1]) / 2;

                float change = (average - tempArray[i]) * 0.01f;
                tempArray[i] += change;
                totalChange += Math.abs(change);
            }
        }
        return totalChange;
    }
    public void run() {
        // TODO ...
        try(DataInputStream file = new DataInputStream(new FileInputStream(filename));
        ) {
            file.readUTF();
            file.readInt();
            if(file.readInt()<end){
                throw new IOException("Not enough float values in the file!");
            }

            // skip until index start
            for(int i=0;i<start;i++){
                file.readFloat();
            }

            // read in all floats
            float[] array = new float[(end-start)+1];
            for(int i=0;i<=end-start;i++){
                array[i]=file.readFloat();
            }

            // do calculation and increase the shared result
            incrementResult(eine_komplizierte_Berechnung(array));

        }catch (IOException e){
            e.printStackTrace();
        }
    }

    private synchronized void incrementResult(float value){
        result = result + value;
    }

    public static void main(String[] args) {
        String pathToFile = ".\\src\\myArrayData.dat";
        int numThreads = 12;
        int arraySize = 200;
        // TODO ...
        int threadArraySize = arraySize/numThreads; // array size for every thread (without remaining elements)
        int numThreadsWithExtraElement = arraySize%numThreads; // amount of remaining elements
        TheRealThing[] threads = new TheRealThing[numThreads];

        // create all threads with correct indexes
        for (int i = 0; i < numThreadsWithExtraElement; i++) {
            int start = i*(threadArraySize+1);
           threads[i] = new TheRealThing(pathToFile,start, start +threadArraySize);
        }
        int startSecondLoop = numThreadsWithExtraElement*(threadArraySize+1);
        for(int i=0;i<numThreads-numThreadsWithExtraElement;i++){
            int start = startSecondLoop+(i*threadArraySize);
            threads[numThreadsWithExtraElement+i] = new TheRealThing(pathToFile, start, start + (threadArraySize-1));
        }

        // start all threads
        for(TheRealThing thread : threads){
            thread.start();
        }

        // wait for all threads to be done
        for(TheRealThing thread : threads){
            try{
                thread.join();
            }catch (InterruptedException e){
                e.printStackTrace();
            }
        }

        System.out.println("All threads are finished.\nThe final result is: "+result);
    }
    public static float getResult(){
        main(new String[]{});
        return result;
    }
}
