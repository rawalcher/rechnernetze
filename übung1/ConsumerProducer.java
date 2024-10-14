import java.util.Random;
import java.util.Stack;
public class ConsumerProducer {
    public static final int AMOUNT_CONSUMER = 10;
    public static final int AMOUNT_PRODUCER = 2;
    public static void main(String[] args)
    {
        Stack<Integer> sharedStack = new Stack<>();
        Consumer[] consumers = new Consumer[AMOUNT_CONSUMER];
        Producer[] producers = new Producer[AMOUNT_PRODUCER];

        for(int i=0;i<AMOUNT_CONSUMER;i++){
            consumers[i]=new Consumer(sharedStack);
        }
        for(int i=0;i<AMOUNT_PRODUCER;i++){
            producers[i]=new Producer(sharedStack);
        }
        for(int i=0;i<AMOUNT_CONSUMER;i++){
            consumers[i].start();
        }
        for(int i=0;i<AMOUNT_PRODUCER;i++){
            producers[i].start();
        }
        for(int i=0;i<AMOUNT_CONSUMER;i++){
            try{
                consumers[i].join();
            }catch (InterruptedException e){
                e.printStackTrace();
            }
        }
        for(int i=0;i<AMOUNT_PRODUCER;i++){
            try{
                producers[i].join();
            }catch (InterruptedException e){
                e.printStackTrace();
            }
        }
    }
}
class Consumer extends Thread {
    private final Stack<Integer> stack;
    public Consumer(Stack<Integer> stack) {
        this.stack = stack;
    }

    public void run() {
        Random random = new Random();
        int count = 0;
        while (true) {
            synchronized (stack) {
                while (stack.isEmpty()) {
                    try {
                        stack.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                int randomAmount = random.nextInt(stack.size())+1;
                for (int i = 0; i < randomAmount; i++) {
                    count += stack.pop();
                }
                stack.notifyAll();
                System.out.println(count);
            }
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }
}
class Producer extends Thread {
    public static final int PRODUCER_MAX_VALUE = 10;
    public static final int STACK_MAX_SIZE = 100;
    private final Stack<Integer> stack;
    public Producer(Stack<Integer> stack)
    {
        this.stack = stack;
    }
    public void run()
    {
        Random random = new Random();
        synchronized (stack){
            while(true){
                while(stack.size()==STACK_MAX_SIZE){
                    try {
                        stack.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                int num = random.nextInt(PRODUCER_MAX_VALUE);
                stack.add(num);
                //System.out.println("Produced: "+num);
                stack.notifyAll();
            }
        }
    }
}
