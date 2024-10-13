import java.nio.ByteBuffer;

public class Ex1_3 {
    static byte[] createMsg (boolean isData,
                             boolean isUrgent,
                             int sequenceNumber,
                             byte [] payload) throws IllegalArgumentException{

        // check invalid input
        if(sequenceNumber<0||sequenceNumber>=0xffff){
            throw new IllegalArgumentException("sequenceNumber too small or too large");
        }
        if(payload==null||payload.length==0){
            throw new IllegalArgumentException("payload is null or empty");
        }

        ByteBuffer buffer = ByteBuffer.allocate(payload.length+8);//4 + 4 + payload

        // version + 3 reserved bits
        buffer.put((byte) 0x10);

        // 6 reserved bits + D + U
        byte secondByte = (byte)0x00;
        if(isData){
            secondByte+=2;
        }
        if(isUrgent){
            secondByte+=1;
        }
        buffer.put(secondByte);

        // change to Network Byte Order (Big-Endian)
        // buffer.order(ByteOrder.BIG_ENDIAN); // ByteBuffer by default already configured correctly

        // sequenceNumber 16 bits (short-length)
        buffer.putShort((short)sequenceNumber);

        // payload length 32 bits (integer-length)
        buffer.putInt(payload.length);

        buffer.put(payload);

        return buffer.array();
    }
}
