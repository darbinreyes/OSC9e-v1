import java.net.*;
import java.io.*;
import java.lang.Integer;

public class EchoServer
{
  public static void main(String[] args) {
    try {
      ServerSocket sock = new ServerSocket(6013);

      /* now listen for connections */
      while (true) {
        Socket client = sock.accept();
        System.out.println("Client connected.");
        /* Read what the client sent us */
        InputStream in = client.getInputStream();
        int next_byte;
        OutputStream out = client.getOutputStream();
        // Since the loop below terminates when the client closes the connection
        // we must send data to the client inside this loop. We can't send data
        // to the client after he has closed the connection. The problem states
        // that we should store the received data in a buffer; I did, but it is
        // a buffer of size 1 byte. There is no point in using, say, the
        // BufferedOutputReader class since we would have to flush the buffer
        // inside the loop, effectively creating a size 1 byte buffer.
        // Furthermore, flushing the supposed buffer outside the loop would fail
        // because the client has already closed the connection.
        while ( (next_byte = in.read()) != -1) { // Receive a byte.
          System.out.println("Received/Echoing byte: "+ new Integer(next_byte)); // Print received byte.
          out.write(next_byte); // Send the byte back to the client.
        }

        System.out.println("Client closed connection.");

        /* close the socket and resume */
        /* listening for connections */
        client.close();
      }
    }
    catch (IOException ioe) {
      System.err.println(ioe);
    }
  }
}