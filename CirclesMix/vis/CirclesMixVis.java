import java.io.*;
import java.util.*;
import java.security.*;
import javax.imageio.*;
import java.awt.image.*;

// --------------------------------------------------------
public class CirclesMixVis {
    static int maxN = 1000, minN = 20;
    static int border = 1000;
    int N;                          // number of circles allowed
    int H, W;
    int[][] givenImage;
    int[] imgArg;
    
    int Nc;                         // number of circles used
    int[][] drawnImage;
    // ---------------------------------------------------
    String generate(String seedStr, int N_) {
      try {
        SecureRandom rnd = SecureRandom.getInstance("SHA1PRNG");
        //long seed = Long.parseLong(seedStr);
        //rnd.setSeed(seed);
        //N = rnd.nextInt(maxN - minN + 1) + minN;
        N=N_;
        /*if (seed == 1)
            N = minN;
        else if (seed == 2)
            N = maxN;
        */

        // read image from file seedStr + ".jpg"
        BufferedImage img = ImageIO.read(new File("inputs//" + seedStr + ".jpg"));
        H = img.getHeight();
        W = img.getWidth();
        givenImage = new int[H][W];
        for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c)
            givenImage[r][c] = img.getRGB(c, r) & 0xFFFFFF;

        // convert image to int[] argument
        imgArg = new int[H * W];
        for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c)
            imgArg[r * W + c] = givenImage[r][c];

        // generate test representation
        StringBuffer sb = new StringBuffer();
        sb.append("H = " + H + '\n');
        sb.append("W = " + W + '\n');
        sb.append("N = " + N + '\n');
        return sb.toString();
      }
      catch (Exception e) {
        addFatalError("An exception occurred while generating test case.");
        e.printStackTrace();
        return "";
      }
    }
    // ---------------------------------------------------
    int comp(int color, int index) {
        return (color >> (8 * index)) & 0xFF;
    }
    // ---------------------------------------------------
    int getDiff(int c1, int c2) {
        int d = 0;
        for (int i = 0; i < 3; ++i)
            d += Math.abs(comp(c1, i) - comp(c2, i));
        return d;
    }
    // ---------------------------------------------------
    int drawOver(int c1, int c2) {
        int c = 0;
        for (int i = 0; i < 3; ++i)
            c += ((comp(c1, i) + comp(c2, i)) / 2) << (8 * i);
        return c;
    }
    // ---------------------------------------------------
    int p2(int t) {
        return t*t;
    }
    // ---------------------------------------------------
    public double runTest(String seed, int N_) {
      try {
        String test = generate(seed, N_);
        if (debug)
            System.out.println(test);

        // call user's solution and get return
        int[] centerRow, centerCol, radius, color;
        if (proc != null) {
            int[] raw_circles;
            try {
                raw_circles = drawImage(H, imgArg, N);
            } catch (Exception e) {
                addFatalError("Failed to get result from drawImage.");
                return -1;
            }

            if (raw_circles.length % 4 != 0) {
                addFatalError("The number of elements in your return must be divisible by 4, and your return contained " + raw_circles.length + ".");
                return -1;
            }
            if (raw_circles.length > 4 * N) {
                addFatalError("Your return can have at most " + (4 * N) + " elements, and it contained " + raw_circles.length + ".");
                return -1;
            }

            // check that this is a valid set of circles
            Nc = raw_circles.length / 4;
            centerRow = new int[Nc];
            centerCol = new int[Nc];
            radius = new int[Nc];
            color = new int[Nc];
            for (int i = 0; i < Nc; ++i) {
                centerRow[i] = raw_circles[4*i];
                centerCol[i] = raw_circles[4*i + 1];
                radius[i]    = raw_circles[4*i + 2];
                color[i]     = raw_circles[4*i + 3];
                if (centerRow[i] < -border || centerRow[i] > H-1 + border) {
                    addFatalError("Center of circle " + i + " must have center at row between " + (-border) + " and " + (H-1 + border) + ", inclusive.");
                    return -1;
                }
                if (centerCol[i] < -border || centerCol[i] > W-1 + border) {
                    addFatalError("Center of circle " + i + " must have center at column between " + (-border) + " and " + (W-1 + border) + ", inclusive.");
                    return -1;
                }
                if (radius[i] < 0 || radius[i] > border) {
                    addFatalError("Radius of circle " + i + " must be between 0 and " + border + ".");
                    return -1;
                }
                if (color[i] < 0 || color[i] > 0xFFFFFF) {
                    addFatalError("Circle " + i + " can only have color between 0 and 0xFFFFFF.");
                    return -1;
                }
            }
        }
        else {
            // no circles
            Nc = 0;
            centerRow = new int[Nc];
            centerCol = new int[Nc];
            radius = new int[Nc];
            color = new int[Nc];
        }

        // draw circles on the canvas, starting with black
        drawnImage = new int[H][W];
        for (int r = 0; r < H; ++r)
            Arrays.fill(drawnImage[r], 0);
        for (int i = 0; i < Nc; ++i) {
            // transparency: take 50% of old color and 50% of new color on each component
            for (int r = Math.max(centerRow[i] - radius[i], 0); r <= Math.min(centerRow[i] + radius[i], H-1); r++) {
                int dc = (int)Math.sqrt(p2(radius[i]) - p2(centerRow[i] - r));
                for (int c = Math.max(centerCol[i] - dc, 0); c <= Math.min(centerCol[i] + dc, W-1); c++)
                    if (p2(centerRow[i] - r) + p2(centerCol[i] - c) <= p2(radius[i]))
                        drawnImage[r][c] = drawOver(drawnImage[r][c], color[i]);
            }

            if (vis) {
                BufferedImage bi = new BufferedImage(W, H, BufferedImage.TYPE_INT_RGB);
                for (int r = 0; r < H; ++r)
                for (int c = 0; c < W; ++c)
                    bi.setRGB(c, r, drawnImage[r][c]);
                File outputFile = new File("outputs//"+ seed+"_"+N+"_seq//" + seed + "_" + (i+1) + ".png");
                outputFile.getParentFile().mkdirs();
                ImageIO.write(bi,"png", outputFile);
            }
        }

        if (vis) {
            BufferedImage bi = new BufferedImage(W, H, BufferedImage.TYPE_INT_RGB);
            for (int r = 0; r < H; ++r)
            for (int c = 0; c < W; ++c)
                bi.setRGB(c, r, drawnImage[r][c]);
            ImageIO.write(bi,"png", new File("outputs//" + seed + "_" + N + ".png"));
        }

        // calculate score
        double diff = 0;
        for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c)
            diff += getDiff(givenImage[r][c], drawnImage[r][c]);

        return diff;
      }
      catch (Exception e) {
        addFatalError("An exception occurred while trying to process your program's results.");
        e.printStackTrace();
        return 0.0;
      }
    }
// ------------- visualization part ----------------------
    static String exec;
    static boolean vis, debug;
    static Process proc;
    InputStream is;
    OutputStream os;
    BufferedReader br;
    // ---------------------------------------------------
    int[] drawImage(int H, int[] pixels, int N) throws IOException {
        // pass the params to the solution and get the return
        int i;
        StringBuffer sb = new StringBuffer();
        sb.append(H).append('\n');
        sb.append(pixels.length).append('\n');
        for (i = 0; i < pixels.length; ++i)
            sb.append(pixels[i]).append('\n');
        sb.append(N).append('\n');
        os.write(sb.toString().getBytes());
        os.flush();

        // get the return - an array of integers
        int nRet = Integer.parseInt(br.readLine());
        int[] ret = new int[nRet];
        for (i = 0; i < nRet; ++i)
            ret[i] = Integer.parseInt(br.readLine());
        return ret;
    }
    // ---------------------------------------------------
    public CirclesMixVis(String seed, int N_) {
        // interface for runTest
        Thread thread = null;
        if (exec != null) {
            try {
                Runtime rt = Runtime.getRuntime();
                proc = rt.exec(exec);
                os = proc.getOutputStream();
                is = proc.getInputStream();
                br = new BufferedReader(new InputStreamReader(is));
                thread = new ErrorReader(proc.getErrorStream());
                thread.start();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        double s = runTest(seed, N_);

        if (thread != null)
            try { thread.join(1000); } 
            catch (Exception e) { e.printStackTrace(); }

        System.out.println("Seed = " + seed);
        System.out.println("N = " + N);
        System.out.println("Score = " + s);

        if (proc != null)
            try { proc.destroy(); }
            catch (Exception e) { e.printStackTrace(); }
    }
    // ---------------------------------------------------
    public static void main(String[] args) {
        String seed = "1";
        int N = 1000;
        vis = true;
        for (int i = 0; i < args.length; i++) {
            if (args[i].equals("-seed"))
                seed = args[++i];
            if (args[i].equals("-N"))
                N = Integer.parseInt(args[++i]);
            if (args[i].equals("-exec"))
                exec = args[++i];
            if (args[i].equals("-novis"))
                vis = false;
            if (args[i].equals("-debug"))
                debug = true;
        }
        CirclesMixVis f = new CirclesMixVis(seed , N);
    }
    // ---------------------------------------------------
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
