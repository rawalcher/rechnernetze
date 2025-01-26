import org.junit.jupiter.api.Test;

import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import static org.junit.jupiter.api.Assertions.*;

public class TheRealThingTest {
    @Test
    public void testFunctionality(){
        String pathToFile = "./src/myArrayData.dat"; // Pfad zur Datei
        String description = "Beispielbezeichnung"; // Die beliebige Bezeichnung
        int id = 12345; // Die ID
        int numberOfFloats = 200; // Anzahl der Float-Werte
        float[] floatValues = {
                5.5792115f, 3.4571139f, 3.8153661f, 6.0671958f, 3.6181405f, 7.469633f, 4.1432772f, 6.0813416f,
                8.259749f, 6.4664819f, 7.2632384f, 2.7038105f, 3.255358f, 7.638553f, 1.5820925f, 5.5394945f,
                0.8044713f, 7.172166f, 6.296624f, 4.9139566f, 0.796955f, 5.389734f, 9.519175f, 8.790988f,
                8.778443f, 6.051792f, 9.361343f, 4.9377413f, 6.713365f, 3.2236729f, 0.3508487f, 3.142678f,
                7.243782f, 6.486067f, 0.93033123f, 0.09020583f, 7.745484f, 7.303584f, 0.8901358f, 4.4020936f,
                3.6115627f, 0.43073067f, 6.3807206f, 7.063124f, 9.382608f, 9.183543f, 8.067164f, 2.6090534f,
                7.992739f, 8.408094f, 9.309336f, 5.0801783f, 3.0336077f, 2.173259f, 0.9739728f, 4.9985013f,
                1.2256258f, 0.97693133f, 3.482062f, 2.0317247f, 0.17834105f, 8.342082f, 3.6284032f, 5.9031634f,
                7.9902353f, 9.065767f, 8.241701f, 4.2127247f, 3.7534277f, 5.9026527f, 0.11576221f, 8.994944f,
                6.6377907f, 1.4296989f, 0.526359f, 2.5365732f, 5.037031f, 2.7176135f, 5.581921f, 0.11674117f,
                9.171068f, 7.872752f, 2.4949908f, 2.6670616f, 0.22387926f, 1.5902579f, 9.153918f, 0.6328651f,
                6.493625f, 7.086496f, 3.0665512f, 1.334120f, 6.8304243f, 4.5465217f, 7.6909413f, 8.549993f,
                4.1093054f, 3.361908f, 0.6831289f, 9.071291f, 8.247227f, 2.9723313f, 1.3158629f, 8.427575f,
                5.440195f, 6.340078f, 0.86910385f, 4.576661f, 0.6473118f, 1.753409f, 6.039048f, 6.599983f,
                2.2715087f, 0.025684817f, 6.169467f, 4.522497f, 8.084172f, 7.1036487f, 3.8570879f, 0.48212552f,
                5.244206f, 2.9582381f, 3.9413497f, 8.31825f, 4.2022386f, 4.805835f, 8.899446f, 1.5719923f,
                3.7291422f, 0.7569177f, 8.43069f, 9.257576f, 3.6278067f, 6.3939304f, 4.612599f, 7.384674f,
                7.937644f, 3.6253033f, 7.517373f, 7.068262f, 4.8900075f, 3.2521083f, 1.3301059f, 5.588187f,
                4.181114f, 1.5228859f, 3.1435065f, 8.888475f, 4.201391f, 3.9857328f, 5.295185f, 4.105644f,
                5.400043f, 7.2504535f, 0.6879951f, 5.6542435f, 4.716141f, 2.6181567f, 9.231001f, 7.618569f,
                1.7569904f, 3.7241712f, 6.779496f, 5.0190935f, 5.350586f, 3.008752f, 8.49914f, 4.6833959f,
                4.9812336f, 4.7322097f, 8.824134f, 7.4299164f, 7.734831f, 8.485082f, 7.58584f, 8.48772f,
                3.9890559f, 5.0554943f, 4.914702f, 3.9051247f, 8.919671f, 9.411959f, 4.1680202f, 7.9369144f,
                0.43533f, 7.277195f, 8.037661f, 7.587139f, 3.6074734f, 9.141857f, 0.4583782f, 7.24427f,
                2.2818167f, 4.0579925f, 0.27275464f, 3.5679815f, 0.97832096f, 6.6799936f, 8.378948f, 6.7856984f
        };
        try (DataOutputStream dos = new DataOutputStream(new FileOutputStream(pathToFile))) {
            dos.writeUTF(description);
            dos.writeInt(id);
            dos.writeInt(numberOfFloats);
            for (float value : floatValues) {
                dos.writeFloat(value);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        float res = 0;
        for(int i=0;i<floatValues.length;i++){
            res += floatValues[i];
        }
        System.out.println("Real result: " + res);
        res=Math.round(res);
        assertEquals(res, Math.round(TheRealThing.getResult()));
    }

}
