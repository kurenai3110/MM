import java.awt.*;
import java.awt.image.*;
import java.io.*;
import java.util.Arrays;
import java.util.Random;
import java.security.*;
import javax.imageio.*;

public class WaterfallFishingVis {
 
SecureRandom r;
int width;
int length;
int rockCount;
boolean[][] rocks;
int[] fishCount;
String[] data;
int[] truth;

private boolean canPlaceRock(int y, int x) {
  if (rocks[y][x]) return false;
  if (y > 0 && rocks[y - 1][x]) return false;
  if (y < length - 1 && rocks[y + 1][x]) return false;
  if (x > 0 && rocks[y][x - 1]) return false;
  if (x < width - 1 && rocks[y][x + 1]) return false;
  return true;
}

private int[] getDailyFish(String mode, int ind) {
  int numFish = 5 + r.nextInt(96);
  int mean = r.nextInt(width);
  int stDev = 2 + r.nextInt(width / 4 - 1);
  int[] ret = new int[width];
  
  // ---------------------
  int SZ = 10;
  BufferedImage bi = new BufferedImage(SZ * width + 1, SZ * (length + 2) + 1, BufferedImage.TYPE_INT_RGB);
  Graphics2D g2 = (Graphics2D)bi.getGraphics();
  Random rv = new Random(ind);
  if (vis) {
    g2.setColor(Color.WHITE);
    g2.fillRect(0, 0, SZ * width + 1, SZ * (length + 2) + 1);
    // grid
    g2.setColor(Color.BLACK);
    for (int i = 0; i <= width; ++i)
        g2.drawLine(SZ*i,SZ,SZ*i,SZ*(length+1));
    for (int i = 1; i < length + 2; ++i)
        g2.drawLine(0,SZ*i,SZ*width,SZ*i);
    // rocks
    for (int y = 0; y < length; ++y)
    for (int x = 0; x < width; ++x)
        if (rocks[y][x])
            g2.fillRect(x*SZ, (y+1)*SZ, SZ, SZ);
    g2.setStroke(new BasicStroke(2.0f));
  }
  // ---------------------
  
  while (numFish > 0) {
    int x = (int)Math.round(r.nextGaussian() * stDev + mean);
    if (x < 0 || x > width - 1) continue;
    
    // ---------------------
    int oldX = x;
    if (vis) {
        g2.setColor(new Color(rv.nextInt(0xDDDDDD)+0x111111));
    }
    // ---------------------
    
    for (int y = 0; y < length; y++) {
      // ---------------------
      if (vis && y > 0) {
        g2.drawLine(oldX * SZ + SZ/2, (y-1) * SZ + SZ/2, x * SZ + SZ/2, y * SZ + SZ/2);
        oldX = x;
      }
      // ---------------------
      if (!rocks[y][x]) continue;
      if (x == 0)
        x++;
      else if (x == width - 1)
        x--;
      else
        x += r.nextInt(2) * 2 - 1;
    }
    // ---------------------
    if (vis) {
      // last jump
      g2.drawLine(oldX * SZ + SZ/2, (length-1) * SZ + SZ/2, x * SZ + SZ/2, length * SZ + SZ/2);
      // move over the waterfall into trap
      g2.drawLine(x * SZ + SZ/2, length * SZ + SZ/2, x * SZ + SZ/2, (length+1) * SZ + SZ/2);
    }
    // ---------------------
    ret[x]++;
    if (ret[x] > 35) ret[x] = 35;
    numFish--;
  }
  
  if (vis) {
    try {
        ImageIO.write(bi,"png", new File(mode + "_" + ind +".png"));
    } catch (IOException e) {
        System.err.println("Failed to write " + mode + "_" + ind + " visualization to file.");
        e.printStackTrace();
    }
  }
  
  return ret;
}

private char convertCountToChar(int x) {
  if (x >= 35) return 'Z';
  if (x < 10) return (char)('0' + x);
  return (char)('A' + x - 10);
}

private void generateTestCase(String s) throws NoSuchAlgorithmException {
  long seed = Long.parseLong(s);
  r = SecureRandom.getInstance("SHA1PRNG");
  r.setSeed(seed);
  width = 20 + r.nextInt(181);
  length = 5 + r.nextInt(96);
  System.out.println("width = "+width);
  System.out.println("length = "+length);
  if (seed == 1) {
    width = 20;
    length = 10;
  }
  if (seed == 2) {
    width = 40;
    length = 15;
  }
  if (seed == 3) {
    width = 80;
    length = 25;
  }
  if (seed == 4) {
    width = 100;
    length = 30;
  }
  rockCount = 5 + r.nextInt(width * length / 10 - 4);
  System.out.println("rockCount = "+rockCount);
  rocks = new boolean[length][width];
  while (rockCount > 0) {
    int x = r.nextInt(width);
    int y = r.nextInt(length);
    if (!canPlaceRock(y, x)) continue;
    rocks[y][x] = true;
    rockCount--;
  }
  data = new String[4 + r.nextInt(17)];
  for (int i = 0; i < data.length; i++) {
    data[i] = "";
    int[] fish = getDailyFish("data", i);
    for (int j = 0; j < width; j++)
      data[i] += convertCountToChar(fish[j]);
  }
  truth = new int[width];
  for (int i = 0; i < 3; i++) {
    int[] day = getDailyFish("test", i);
    for (int j = 0; j < width; j++)
      truth[j] = Math.min(35, truth[j] + day[j]);
  }
  System.out.println("datasize = "+data.length);
  
  for(int j=0;j<width;j++)
    System.out.print(convertCountToChar(truth[j]));
  System.out.println('\n');
/*
  int sorted[] = Arrays.copyOf(truth, truth.length);
  Arrays.sort(sorted);
  int cnt=1;
  double score = sorted[width-1];
  for(int j=width-2;j>=0;j--){
    System.out.println(score/cnt+" "+(score+sorted[j])/(cnt+1));
    if(score/cnt <= (score+sorted[j])/(cnt+1)){
      score += sorted[j];
      cnt++;
    }
    else break;
  } 
  score /= cnt;
  System.out.println("maxScore = " + score);
  */
}

public double runTest(String seed) {
try {
  generateTestCase(seed);

  int[] res = placeTraps(data);
  if (res == null) {
    addFatalError("failed to get result from placeTraps()");
    return -1.0;
  }
  if (res.length < 1 || res.length > width) {
    addFatalError("the length of the return must be between 1 and W");
    return -1.0;
  }
  
  Arrays.sort(res);
  for (int i = 0; i < res.length; i++) {
    if (res[i] < 0 || res[i] >= width) {
      addFatalError("value " + res[i] + " is outside the bounds of the river.");
      return -1.0;
    }
    if (i > 0 && res[i - 1] == res[i]) {
      addFatalError("value " + res[i] + " is duplicated in the result.");
      return -1.0;
    }
  }
  
  int fish = 0;
  for (int i = 0; i < res.length; i++) {
    fish += truth[res[i]];
  }
  return 1.0 * fish / res.length;
} catch (Exception e) {
  System.err.println("An exception occurred while trying to get your program's results.");
  e.printStackTrace();
  return -1.0;
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
    int[] placeTraps(String[] data) throws IOException {
        if (proc == null) {
            return new int[0];
        }
        StringBuffer sb = new StringBuffer();
        sb.append(data.length).append("\n");
        for (String st : data) {
            sb.append(st).append("\n");
        }
        os.write(sb.toString().getBytes());
        os.flush();
        
        int retN = Integer.parseInt(br.readLine());
        int[] ret = new int[retN];
        for (int i = 0; i < retN; ++i)
            ret[i] = Integer.parseInt(br.readLine());
        return ret;
    }
    // -----------------------------------------
    public WaterfallFishingVis(String seed) {
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
            if (args[i].equals("-vis"))
                vis = true;
        }
        WaterfallFishingVis f = new WaterfallFishingVis(seed);
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

