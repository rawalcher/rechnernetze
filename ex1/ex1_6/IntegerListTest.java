import java.util.Random;

public class IntegerListTest extends Thread{

    public static void main(String[] args){
        IntegerList list = new IntegerList();
        list.add(1);
        IntegerListTest[] threads = new IntegerListTest[1000];
        for(int i=0;i<threads.length;i++){
            threads[i]=new IntegerListTest(list);
        }
        for(IntegerListTest thread : threads){
            thread.start();
        }
        for(IntegerListTest thread : threads){
            try{
                thread.join();
            }catch (InterruptedException e){
                e.printStackTrace();
            }
        }
    }

    private final IntegerList integerList;
    private static final int THREAD_REPEAT_TIME = 1000;
    public IntegerListTest(IntegerList list){
        integerList = list;
    }
    @Override
    public void run() {
        Random random = new Random();
        int[] numbers = new int[THREAD_REPEAT_TIME];
        int[] indexes = new int[THREAD_REPEAT_TIME];
        for(int i=0;i<THREAD_REPEAT_TIME;i++){
            numbers[i]=random.nextInt();
        }
        for(int i=0;i<THREAD_REPEAT_TIME;i++){
            indexes[i] = integerList.add(numbers[i]);
        }
        for(int i=0;i<THREAD_REPEAT_TIME;i++){
            if(numbers[i]!=integerList.get(indexes[i])){
                System.out.println("Test failed! expected: "+numbers[i]+" actual: "+integerList.get(indexes[i])+" at the "+i+"th loop.");
            }
        }
    }
}
