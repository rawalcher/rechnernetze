public class IntegerList {
    protected int[] data;
    protected int size;
    private static final int DEFAULT_SIZE = 10;
    protected int index;
    public IntegerList() {
        data = new int[DEFAULT_SIZE];
        index = 0;
    }
    public IntegerList(IntegerList toCopy) {
        IntegerList saveCopy = toCopy.getClone();
        data = saveCopy.toArray();
        size = data.length;
        index = saveCopy.getIndex();
    }
    public synchronized int get(int index) throws IllegalArgumentException{
        if(index>size){
            throw new IllegalArgumentException();
        }
        return data[index];
    }
    public synchronized int add(int value) {
        if(index==data.length){
            int newlength = data.length*2;
            int[] newData = new int[newlength];
            arrayCopy(data, newData);
            newData[data.length] = value;
            data = newData;
            size = newData.length;
        }else{
            data[index] = value;
        }
        return index++;
    }
    public synchronized void clear(){
        data = new int[DEFAULT_SIZE];
        size = DEFAULT_SIZE;
        index = 0;
    }
    public synchronized void setCapacity(int n){
        int[] newData = new int[n];
        for(int i = 0; i<(Math.min(n, index-1)); i++){
            newData[i] = data[i];
        }
        data = newData;
        size = newData.length;
        if(n<index){
            index = n;
        }
    }
    public synchronized int[] toArray(){
        int[] newData = new int[size];
        arrayCopy(data, newData);
        return newData;
    }
    public synchronized IntegerList getClone(){
        IntegerList newlist = new IntegerList();
        for(int i=0;i<size;i++){
            newlist.add(data[i]);
        }
        newlist.setCapacity(size);
        return newlist;
        // index already set correctly by add
    }
    public synchronized int getIndex(){
        return index;
    }
    public void arrayCopy(int[] src, int[] dest){
        if(src == null || dest == null){
            throw new IllegalArgumentException("Array is null");
        }
        for(int i=0;i<Math.min(src.length, dest.length);i++){
            dest[i]=src[i];
        }
    }
}