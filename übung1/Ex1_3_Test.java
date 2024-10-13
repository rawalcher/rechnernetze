import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class Ex1_3_Test {
    @Test
    public void testSequenceNumberTooSmall(){
        assertThrows(IllegalArgumentException.class, () -> {
            Ex1_3.createMsg(true, true, -1, new byte[]{0x01, 0x02, 0x03, 0x04});
        });
    }
    @Test
    public void testSequenceNumberTooBig(){
        assertThrows(IllegalArgumentException.class, () -> {
            Ex1_3.createMsg(true, true, Integer.MAX_VALUE, new byte[]{0x01, 0x02, 0x03, 0x04});
        });
    }
    @Test
    public void testPayloadIsNull(){
        assertThrows(IllegalArgumentException.class, () -> {
            Ex1_3.createMsg(true, true, Integer.MAX_VALUE, null);
        });
    }
    @Test
    public void testPayloadIsEmpty(){
        assertThrows(IllegalArgumentException.class, () -> {
            Ex1_3.createMsg(true, true, Integer.MAX_VALUE, new byte[0]);
        });
    }
    @Test
    public void testPayloadIsEmpty_2(){
        assertThrows(IllegalArgumentException.class, () -> {
            Ex1_3.createMsg(true, true, Integer.MAX_VALUE, new byte[4]);
        });
    }
    @Test
    public void testFunctionality(){
        byte[] payload = new byte[]{0x01, 0x02, 0x03, 0x04};
        byte[] result = Ex1_3.createMsg(true, true, 38052, payload);
        assertEquals((byte)0x10, result[0]);
        assertEquals((byte)0x03, result[1]);
        assertEquals((byte)0x94, result[2]);
        assertEquals((byte)0xA4, result[3]);
        assertEquals((byte)0x00, result[4]);
        assertEquals((byte)0x00, result[5]);
        assertEquals((byte)0x00, result[6]);
        assertEquals((byte)0x04, result[7]);
        assertEquals((byte)0x01, result[8]);
        assertEquals((byte)0x02, result[9]);
        assertEquals((byte)0x03, result[10]);
        assertEquals((byte)0x04, result[11]);
    }
    @Test
    public void testFunctionalityBig(){
        byte[] payload = new byte[Integer.MAX_VALUE/8];
        for(int i=0;i<Integer.MAX_VALUE/8;i++){
            payload[i] = (byte)i;
        }
        byte[] result = Ex1_3.createMsg(true, false, 38052, payload);
        assertEquals((byte)0x10, result[0]);
        assertEquals((byte)0x02, result[1]);
        assertEquals((byte)0x94, result[2]);
        assertEquals((byte)0xA4, result[3]);
        assertEquals((byte)0x0F, result[4]);
        assertEquals((byte)0xFF, result[5]);
        assertEquals((byte)0xFF, result[6]);
        assertEquals((byte)0xFF, result[7]);
        assertEquals((byte)0x00, result[8]);
        assertEquals((byte)0x01, result[9]);
        assertEquals((byte)0x00, result[8+256]);
        assertEquals((byte)0xFF, result[8+256-1]);
        assertEquals((byte)0x01, result[8+256+1]);
        assertEquals((byte)0x00, result[8+(256*5)]);
        assertEquals((byte)0x00, result[8+(256*200)]);
    }

}
