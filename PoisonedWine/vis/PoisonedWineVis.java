import java.io.*;
import java.security.*;
import java.util.Arrays;

public class PoisonedWineVis {

private SecureRandom r;
private int numBottles;
private int testStrips;
private int testRounds;
private int numPoison;
private boolean[] bottles;

private boolean failure = false;

private void generateTestCase(long seed) {
  try {
    r = SecureRandom.getInstance("SHA1PRNG");
  } catch (Exception e) { }
  r.setSeed(seed);
  numBottles = r.nextInt(9951) + 50;
  bottles = new boolean[numBottles];
  testStrips = r.nextInt(16) + 5;
  testRounds = r.nextInt(10) + 1;
  numPoison = r.nextInt(numBottles / 50) + 1;
  System.out.println("seed " + seed);
  System.out.println(numBottles + " " + testStrips + " " + testRounds + " " + numPoison);
  int remain = numPoison;
  //r.setSeed(System.currentTimeMillis());
  while (remain > 0) {
    int x = r.nextInt(numBottles);
    if (bottles[x]) continue;
    bottles[x] = true;
    remain--;
  }
}

public int[] useTestStrips(String[] tests) {
  if (tests.length > testStrips) {
    addFatalError("testWine() called with " + tests.length + " tests when only " + testStrips + " strips remain");
    failure = true;
    return new int[0];
  }
  if (testRounds <= 0) {
    addFatalError("testWine() called too many times");
    failure = true;
    return new int[0];
  }
  int[] ret = new int[tests.length];
  for (int i = 0; i < tests.length; i++) {
    boolean poison = false;
    String[] s = tests[i].split(",");
    for (int j = 0; j < s.length; j++) {
      int x = -1;
      try {
        x = Integer.parseInt(s[j]);
      } catch (Exception e) {
        addFatalError("Invalid value " + s[j] + " found in a test request");
        failure = true;
        return new int[0];
      }
      if (x < 0 || x >= numBottles) {
        addFatalError("Invalid value " + x + " found in a test request");
        failure = true;
        return new int[0];
      }
      poison |= bottles[x];
    }
    if (poison) {
      ret[i] = 1;
      testStrips--;
    }
  }
  testRounds--;
  return ret;
}

public double runTest(String seed) {
try {
  generateTestCase(Long.parseLong(seed));
  int[] ret = testWine(numBottles, testStrips, testRounds, numPoison);
  if (failure) {
    return 0;
  }
  for (int i = 0; i < ret.length; i++) {
    if (ret[i] < 0 || ret[i] >= numBottles) {
      addFatalError("Invalid return value: " + ret[i]);
      return 0;
    }
    bottles[ret[i]] = false;
  }
  for (int i = 0; i < bottles.length; i++) {
    if (bottles[i]) {
      addFatalError("A poisoned bottle remained.");
      return 0;
    }
  }
  Arrays.sort(ret);
  for (int i = 1; i < ret.length; i++) {
    if (ret[i] == ret[i - 1]) {
      addFatalError("Bottle " + ret[i] + " was duplicated in the return");
      return 0;
    }
  }
  double pct = 1.0 * (numBottles - ret.length) / (numBottles - numPoison);
  return pct * pct;
} catch (Exception e) {
  System.err.println("An exception occurred while trying to get your program's results.");
  e.printStackTrace();
  return 0;
}
}

// ------------- visualization part ------------
    static String exec;
    static Process proc;
    static boolean vis;
    InputStream is;
    OutputStream os;
    BufferedReader br;
    // -----------------------------------------
    int[] testWine(int numBottles, int testStrips, int testRounds, int numPoison) throws IOException {
        int[] ret = new int[0];
        if (proc == null) {
            return ret;
        }
        // pass parameters to main method
        StringBuffer sb = new StringBuffer();
        sb.append(numBottles).append("\n");
        sb.append(testStrips).append("\n");
        sb.append(testRounds).append("\n");
        sb.append(numPoison).append("\n");
        os.write(sb.toString().getBytes());
        os.flush();
        
        // simulate function calls - they start with "?" in separate line followed by a line with # of tests followed by lines with tests themselves
        String s;
        while ((s = br.readLine()).equals("?")) {
            // get params of next call
            int nTests;
            try {
                nTests = Integer.parseInt(br.readLine());
            } catch (NumberFormatException e) {
                // failed to convert query arg to ints
                return ret;
            }
            String[] tests = new String[nTests];
            for (int i = 0; i < nTests; ++i)
                tests[i] = br.readLine();
            int[] testRes = useTestStrips(tests);

            sb = new StringBuffer();
            sb.append(testRes.length).append("\n");
            for (int i = 0; i < testRes.length; ++i)
                sb.append(testRes[i]).append("\n");
            os.write(sb.toString().getBytes());
            os.flush();
        }

        // get final result
        int retN = Integer.parseInt(s);
        ret = new int[retN];
        for (int i = 0; i < retN; ++i)
            ret[i] = Integer.parseInt(br.readLine());
        return ret;
    }
    // -----------------------------------------
    public PoisonedWineVis(String seed) {
      try {
        if (exec != null) {
            try {
                Runtime rt = Runtime.getRuntime();
                proc = rt.exec(exec);
                os = proc.getOutputStream();
                is = proc.getInputStream();
                br = new BufferedReader(new InputStreamReader(is));
                new ErrorReader(proc.getErrorStream()).start();
            } catch (Exception e) { e.printStackTrace(); }
        }
        System.out.println("Score = " + runTest(seed));
        if (proc != null)
            try { proc.destroy(); }
            catch (Exception e) { e.printStackTrace(); }
      }
      catch (Exception e) { e.printStackTrace(); }
    }
    // -----------------------------------------
    public static void main(String[] args) {
        String seed = "1";
        for (int i = 0; i<args.length; i++)
        {   if (args[i].equals("-seed"))
                seed = args[++i];
            if (args[i].equals("-exec"))
                exec = args[++i];
        }
        PoisonedWineVis f = new PoisonedWineVis(seed);
    }
    // -----------------------------------------
    void addFatalError(String message) {
        System.out.println(message);
    }
}

class ErrorReader extends Thread{
    InputStream error;
    public ErrorReader(InputStream is) {
        error = is;
    }
    public void run() {
        try {
            byte[] ch = new byte[50000];
            int read;
            while ((read = error.read(ch)) > 0)
            {   String s = new String(ch,0,read);
                System.out.print(s);
                System.out.flush();
            }
        } catch(Exception e) { }
    }
}
