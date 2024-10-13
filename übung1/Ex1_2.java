import java.io.*;
import java.nio.charset.Charset;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

public class Ex1_2 {
    public static void main(String[] args) {
        // Überprüfen der Kommandozeilenparameter
        if (args.length != 2) {
            System.out.println("Usage: java FileConverterSimple <input file> <output file>");
            return;
        }

        String inputFile = args[0];
        String outputFile = args[1];

        try (
                // BufferedReader für die UTF-8 Eingabedatei
                BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(inputFile), Charset.forName("UTF-8")));

                // BufferedWriter für die ISO 8859-1 Ausgabedatei
                ZipOutputStream zipwriter = new ZipOutputStream(new BufferedOutputStream(new FileOutputStream(outputFile)));
                OutputStreamWriter writer = new OutputStreamWriter(zipwriter, Charset.forName("ISO-8859-1"));

        ) {
            // Erstellen eines Eintrags im Zip-ordner
            zipwriter.putNextEntry(new ZipEntry("data.txt"));
            String line;
            while ((line = reader.readLine()) != null) {
                // Zeilenumbruch von LF auf CR+LF ändern und in die Ausgabedatei schreiben
                String convertedLine = line + "\r\n"; // CR+LF
                writer.write(convertedLine); // In Ausgabedatei schreiben
            }
            writer.flush();
            zipwriter.closeEntry();
            System.out.println("Datei erfolgreich konvertiert.");
        } catch (FileNotFoundException e) {
            System.out.println("Error: File not found - " + e.getMessage());
        } catch (IOException e) {
            System.out.println("Error during file processing - " + e.getMessage());
        }
    }
}