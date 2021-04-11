import java.net.*;
import java.io.*;

public class EchoClient
{
  public static void main(String[] args) {
    try {
      Socket sock = new Socket("127.0.0.1", 6013);

      /* Send something to the server */
      String send_str = "Hello Dijkstra.";
      int[] primes = {2, 3, 5, 7, 11, 13};
      OutputStream out = sock.getOutputStream();
      InputStream in = sock.getInputStream();
      int send_byte;
      int rec_byte;

      for (int i = 0; i < primes.length/*send_str.length()*/; i++){
        send_byte = primes[i]; // send_str.charAt(i);
        out.write(send_byte); // Send a byte
        rec_byte = in.read(); // Receive a byte
        System.out.println("Sent/Received byte: " + new Integer(send_byte) + "/" + new Integer(rec_byte)); // Print received byte.
      }

      sock.close();
    }
    catch (IOException ioe) {
      System.err.println(ioe);
    }
  }
}