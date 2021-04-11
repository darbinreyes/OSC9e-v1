import java.net.*;
import java.io.*;
import java.util.Date;
import java.util.concurrent.TimeUnit;

public class QuoteServer
{
  public static boolean is_30seconds_passed(Date startdate, Date enddate) {
    System.out.println(startdate.toString());
    System.out.println(enddate.toString());
    TimeUnit tu = TimeUnit.SECONDS; // Time unit with which to calculate elapsed time between startdate and enddate.
    long diffms = enddate.getTime() - startdate.getTime(); // Time difference in milliseconds.
    long diff = tu.convert(diffms, TimeUnit.MILLISECONDS); // Convert diff in milliseconds into diff in seconds.

    System.out.println(diff);

    if (diff >= 30) {
      return true;
    }

    return false;
  }

  public static void main(String[] args) {
    try {
      String[] quotes = {"Hello", "Dijkstra", "Edsgar"};
      int index = 0;
      Date timestamp = new java.util.Date();
      Date ctimestamp;

      timestamp.setSeconds(0); // Zero out relevant field of the timestamp.

      System.out.println(timestamp.toString());

      ServerSocket sock = new ServerSocket(6017);

      /* now listen for connections */
      while (true) {

        Socket client = sock.accept();

        /*

          Instead of returning a new quote every day this returns a new quote
          after the 30 second of every minute. (For easier testing).

        */
        ctimestamp = new java.util.Date();
        if (is_30seconds_passed(timestamp, ctimestamp)) {
          // Time for a new quote.
          timestamp = ctimestamp;
          timestamp.setSeconds(0);
          timestamp.setTime(timestamp.getTime() + 1000 * 60); // Set new timestamp to the next minute in the future.
          index = (index + 1) % quotes.length;
        }

        System.out.println(quotes[index]);

        PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

          /* write the Date to the socket */
        pout.println(quotes[index]);

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