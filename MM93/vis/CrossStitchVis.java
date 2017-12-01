import java.awt.*;
import java.awt.image.*;
import java.io.*;
import java.lang.Math.*;
import java.security.*;
import java.util.*;
import javax.imageio.*;

class P {
    public int r, c;
    public P() {};
    public P(int r1, int c1) {
        r = r1;
        c = c1;
    }
    public String toString() {
        return "(" + r + "," + c + ")";
    }
    public static int dot(P A, P B, P C) {
        // dot product AB * BC
        return (B.r - A.r)*(C.r - B.r) + (B.c - A.c)*(C.c - B.c);
    }
    public static int cross(P A, P B, P C) {
        // cross product AB x AC
        return (B.r - A.r)*(C.c - A.c) - (B.c - A.c)*(C.r - A.r);
    }
    public static double dist(P A, P B) {
        // distance from A to B
        return Math.sqrt(Math.pow(B.r - A.r, 2) + Math.pow(B.c - A.c, 2));
    }
    public static double segDist(P A, P B, P C) {
        // distance from C to segment AB
        if (dot(A, B, C) > 0)
            return dist(B, C);
        if (dot(B, A, C) > 0)
            return dist(A, C);
        return Math.abs(cross(A, B, C) / dist(A, B));
    }
}

public class CrossStitchVis {
    static int maxSize = 100, minSize = 10;
    static int maxColor = 20, minColor = 2;

    int S;                      // size of the embroidery
    int C;                      // number of colors in the pattern
    char[][] pattern;           // pattern: . for empty space, letters a..z for colors
    int nFill;                  // number of filled spaces in the pattern
    int[] nColFill;             // number of filled cells of each color

    String errmes;
    SecureRandom r1;
    // -----------------------------------------
    void genPattern() {
        // generate the pattern
        pattern = new char[S][S];
        nColFill = new int[C];
        nFill = 0;
        double pFlip = r1.nextDouble() * 0.2 + 0.2;

        // some random overlapping lines
        int nL = r1.nextInt(S+C) + Math.min(S, C);
        P[] lbeg = new P[nL];
        P[] lend = new P[nL];
        int[] lw = new int[nL];
        int[] lc = new int[nL];
        for (int i = 0; i < nL; ++i) {
            lbeg[i] = new P(r1.nextInt(S+1), r1.nextInt(S+1));
            lend[i] = new P(r1.nextInt(S+1), r1.nextInt(S+1));
            lw[i] = r1.nextInt(2) + 1;
            lc[i] = r1.nextInt(C);
        }
        
        for (int i = 0; i < S; ++i)
        for (int j = 0; j < S; ++j) {
            // find the topmost line which passes through this point
            pattern[i][j] = '.';
            for (int k = nL - 1; k >= 0; k--) {
                P p = new P(i, j);
                if (P.segDist(lbeg[k], lend[k], p) < lw[k]) {
                    pattern[i][j] = (char)(lc[k] + 'a');
                    break;
                }
            }
            // randomly flip the result to possibly different color
            if (r1.nextDouble() < pFlip) {
                pattern[i][j] = (char)(r1.nextInt(C)+'a');
            }
            
            if (pattern[i][j] != '.') {
                nFill++;
                nColFill[pattern[i][j] - 'a']++;
            }
        }
    }
    // -----------------------------------------
    void generate(String seedStr) {
    try {
        // generate test case
        r1 = SecureRandom.getInstance("SHA1PRNG");
        long seed = Long.parseLong(seedStr);
        r1.setSeed(seed);

        S = r1.nextInt(maxSize - minSize + 1) + minSize;
        C = r1.nextInt(maxColor - minColor + 1) + minColor;
        if (seed == 1) {
            S = 10;
            C = 2;
        } else if (seed == 2) {
            S = maxSize;
            C = maxColor;
        }

        // make sure all colors promised are represented in the pattern, otherwise return length is invalid
        boolean allCol;
        do {
            genPattern();
            allCol = true;
            for (int i = 0; i < C; ++i)
                if (nColFill[i] == 0)
                    allCol = false;
        } while (!allCol);

        if (debug) {
            System.out.println("Size of the pattern S = " + S);
            System.out.println("Number of colors C = " + C);
            System.out.println("Pattern: ");
            for (int i = 0; i < S; ++i) {
                System.out.println(new String(pattern[i]));
            }
        }

        if (vis) {
            colors = new int[C];
            for (int i = 0; i < C; ++i)
                colors[i] = r1.nextInt(0xAAAAAA) + 0x555555;
        }
    }
    catch (Exception e) {
        System.err.println("An exception occurred while generating test case.");
        e.printStackTrace();
    }
    }
    // -----------------------------------------
    public double runTest(String seed) {
    try {
        generate(seed);

        String[] patternArg = new String[S];
        for (int i = 0; i < S; ++i)
            patternArg[i] = new String(pattern[i]);

        String[] ret = embroider(patternArg);

        // parse return value and convert it to a list of needle points
        if (ret == null) {
            addFatalError("Failed to get result from embroider.");
            return 0.0;
        }
        
        // number of points in return must be: 4 * number of filled cells + number of colors
        int L = ret.length;
        if (L != 4 * nFill + C) {
            addFatalError("Your return must contain exactly " + (4 * nFill + C) + " elements, and it contained " + L + ".");
            return 0.0;
        }
        
        BufferedImage bi = new BufferedImage(S * SZ + 1, S * SZ + 1, BufferedImage.TYPE_INT_RGB);
        Graphics2D g2 = (Graphics2D)bi.getGraphics();
        g2.setColor(Color.WHITE);
        g2.fillRect(0, 0, S * SZ + 1, S * SZ + 1);
        
        double score = 0.0;
        int ind = 0;        // index of current element in ret
        for (int c = 0; c < C; ++c) {
            // return consists of block for each color c
            char curCol = (char)('a' + c);
            // first element must be corresponding lowercase letter
            if (ret[ind].length() != 1 || ret[ind].charAt(0) != curCol) {
                addFatalError("For each color section of return which corresponds to it must start with the color; for color " + curCol + " it started with " + ret[ind] + ".");
                return 0.0;
            }
            ind++;
            ArrayList<P> points = new ArrayList<>();
            // followed by some needle point coordinates
            while (ind < L && ret[ind].length() > 1) {
                String[] sp = ret[ind].split(" ");
                if (sp.length != 2) {
                    addFatalError("Each non-color element of return must be formatted as 'ROW COL', and your return contained element '" + ret[ind] + "'.");
                    return 0.0;
                }
                
                int row, col;
                try {
                    row = Integer.parseInt(sp[0]);
                    col = Integer.parseInt(sp[1]);
                } catch (Exception e) {
                    addFatalError("Each non-color element of return must be formatted as 'ROW COL', and your return contained element '" + ret[ind] + "'.");
                    return 0.0;
                }
                
                if (row < 0 || row > S || col < 0 || row > S) {
                    addFatalError("ROW and COL coordinates of each point must be between 0 and " + S + ", inclusive, and your return contained element '" + ret[ind] + "'.");
                    return 0.0;
                }
                
                // up to this point validation is only done on individual points; 
                // per-stitch and overall validation is done on the whole array of points for the color
                // including validation that each point is in the corner of one of the cells of correct color
                points.add(new P(row, col));
                ind++;
            }
            
            // # of points for each color must be 4 * number of cells in that color
            if (points.size() != nColFill[c] * 4 ) {
                addFatalError("Number of points for color " + curCol + " must be " + nColFill[c] * 4 + ", while it was " + points.size() + ".");
                return 0.0;
            }
            
            // validate that stitches fill the pattern for that color properly
            // for each cell in the pattern, mark whether it is embroided correctly: diag[0] for (r,c)-(r+1,c+1) stitches, diag[1] for (r+1,c)-(r,c+1)
            boolean[][][] diag = new boolean[2][S][S];
            // each pair points[2*i]-points[2*i+1] corresponds to face of stitch
            // so must be either (r,c)-(r+1,c+1) or (r+1,c)-(r,c+1) (not necessarily in that order)
            for (int i = 0; i < nColFill[c] * 2; ++i) {
                P a = points.get(2*i);
                P b = points.get(2*i + 1);
                String st = a.toString() + "-" + b.toString();
                if (a.equals(b)) {
                    addFatalError("Stitch " + st + " on the front side must have different starting and ending points.");
                    return 0.0;
                }
                if (Math.abs(a.r - b.r) != 1 || Math.abs(a.c - b.c) != 1) {
                    addFatalError("Stitch " + st + " on the front side must have ROW and COL of starting and ending points differ by 1.");
                    return 0.0;
                }
                int dir = (a.r-b.r == a.c-b.c ? 0 : 1);
                int row = Math.min(a.r, b.r);
                int col = Math.min(a.c, b.c);
                if (diag[dir][row][col]) {
                    addFatalError("Stitch " + st + " on the front side has already been done.");
                    return 0.0;
                }
                if (pattern[row][col] != curCol) {
                    addFatalError("Stitch " + st + " on the front side is done in wrong color: expected " + pattern[row][col] + ", done " + curCol + ".");
                    return 0.0;
                }
                diag[dir][row][col] = true;
            }
            
            // each other pair is the backside, so can be any stitch with different endpoints
            for (int i = 0; i < nColFill[c] * 2 - 1; ++i)
                if (points.get(2*i+1).equals(points.get(2*i+2))) {
                    addFatalError("Every stitch on the back side must have different starting and ending points.");
                    return 0.0;
                }
            
            // after all individual stitches are done, double-check that each cell of this color got two correct stitches
            for (int row = 0; row < S; ++row)
            for (int col = 0; col < S; ++col)
                if (pattern[row][col] == curCol && (!diag[0][row][col] || !diag[1][row][col])) {
                    addFatalError("Every cell of color " + curCol + " must have two diagonal stitches on the front side.");
                    return 0.0;
                }
            
            // finally, after all validation add each back side stitch's length to the answer (front side length is fixed to nColFill[c] * 2 * sqrt(2))
            for (int i = 0; i < nColFill[c] * 2 - 1; ++i) {
                P a = points.get(2*i+1);
                P b = points.get(2*i+2);
                score += Math.sqrt(Math.pow(a.r - b.r, 2) + Math.pow(a.c - b.c, 2));
            }

            if (vis) {
                // stitches
                g2.setColor(new Color(colors[c]));
                for (int i = 0; i < points.size() - 1; ++i) {
                    P a = points.get(i);
                    P b = points.get(i+1);
                    if (!frontOnly && !backOnly || frontOnly && i % 2 == 0 || backOnly && i % 2 == 1)
                        g2.drawLine(a.c * SZ, a.r * SZ, b.c * SZ, b.r * SZ);
                }
            }
        }

        if (vis) {
            // draw the resulting stitching to a png file
            ImageIO.write(bi,"png", new File(seed+".png"));
        }

        double frontSideLength = nFill * 2 * Math.sqrt(2);
        return Math.max(0, Math.pow((5 - score / frontSideLength) / 5, 3));
    }
    catch (Exception e) {
        System.err.println("An exception occurred while trying to get your program's results.");
        e.printStackTrace();
        return 0;
    }
    }
// ------------- visualization part ------------
    static String exec;
    static boolean vis, debug, frontOnly, backOnly;
    static Process proc;
    static int SZ;
    int[] colors;
    InputStream is;
    OutputStream os;
    BufferedReader br;
    // -----------------------------------------
    String[] embroider(String[] pattern) throws IOException {
        if (proc == null) {
            return new String[0];
        }
        StringBuffer sb = new StringBuffer();
        sb.append(pattern.length).append("\n");
        for (String st : pattern) {
            sb.append(st).append("\n");
        }
        os.write(sb.toString().getBytes());
        os.flush();

        int retN = Integer.parseInt(br.readLine());
        String[] ret = new String[retN];
        for (int i = 0; i < retN; ++i)
            ret[i] = br.readLine();
        return ret;
    }
    // -----------------------------------------
    public CrossStitchVis(String seed) {
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
        vis = true;
        frontOnly = false;
        backOnly = false;
        SZ = 10;
        for (int i = 0; i<args.length; i++)
        {   if (args[i].equals("-seed"))
                seed = args[++i];
            if (args[i].equals("-exec"))
                exec = args[++i];
            if (args[i].equals("-novis"))
                vis = false;
            if (args[i].equals("-debug"))
                debug = true;
            if (args[i].equals("-size"))
                SZ = Integer.parseInt(args[++i]);
            if (args[i].equals("-frontOnly")) {
                frontOnly = true;
                backOnly = false;
            }
            if (args[i].equals("-backOnly")) {
                frontOnly = false;
                backOnly = true;
            }
        }
        CrossStitchVis f = new CrossStitchVis(seed);
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
