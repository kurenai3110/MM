import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.util.*;
import java.security.*;
import javax.swing.*;
import javax.imageio.*;

// --------------------------------------------------------
public class KnightsAttacksVis {
    static int maxS = 500, minS = 50;
    static int[] dr = {1,  1, -1, -1, 2,  2, -2, -2};
    static int[] dc = {2, -2,  2, -2, 1, -1,  1, -1};
    int S;                          // board size
    int[][] board;                  // input board
    String[] boardArg;              // input board as passed to the solution
    boolean[][] placedK;            // knights placement returned by the solution
    boolean[][] originalK;          // knights placement which generated input (for visualization only)
    int[][] attacks;                // attacks by placed knights
    // ---------------------------------------------------
    String generate(String seedStr) {
      try {
        SecureRandom rnd = SecureRandom.getInstance("SHA1PRNG");
        long seed = Long.parseLong(seedStr);
        rnd.setSeed(seed);

        S = rnd.nextInt(maxS - minS + 1) + minS;
        if (seed == 1)
            S = 6;
        else if (seed == 2)
            S = minS;
        else if (seed == 3)
            S = maxS;

        SZX = S * SZ;
        SZY = S * SZ;

        // generate the input board
        double pK = rnd.nextDouble() * 0.8 + 0.1;   // probability of a knight being placed in a cell
        double pC = rnd.nextDouble() * 0.4 + 0.1;   // probability of changing the number in a cell
        int maxC = rnd.nextInt(3) + 1;              // max diff between original number and changed one
        int nRand = rnd.nextInt(S);                 // number of cells changed completely randomly

        // generate a random placement of the knights and calculate the number of knights attacking each cell
        board = new int[S][S];
        originalK = new boolean[S][S];
        for (int i = 0; i < S; ++i)
        for (int j = 0; j < S; ++j)
            if (rnd.nextDouble() < pK) {
                originalK[i][j] = true;
                for (int k = 0; k < 8; ++k) {
                    int r = i + dr[k];
                    int c = j + dc[k];
                    if (r >= 0 && c >= 0 && r < S && c < S)
                        board[r][c]++;
                }
            }
        
        if (debug) {
            System.out.println("Original knights placement:");
            for (int i = 0; i < S; ++i) {
                for (int j = 0; j < S; ++j)
                    System.out.print(originalK[i][j] ? 'K' : '.');
                System.out.println();
            }
            System.out.println("Original numbers of attacks on each cell:");
            for (int i = 0; i < S; ++i) {
                for (int j = 0; j < S; ++j)
                    System.out.print(board[i][j]);
                System.out.println();
            }
        }

        // change the numbers on the board: increase or decrease randomly up to maxC units
        for (int i = 0; i < S; ++i)
        for (int j = 0; j < S; ++j)
            if (rnd.nextDouble() < pC) {
                int old = board[i][j];
                int low = Math.max(0, old - maxC);
                int high = Math.min(8, old + maxC);
                do {
                    board[i][j] = rnd.nextInt(high - low + 1) + low;
                } while (board[i][j] == old);
            }

        // completely random changes
        for (int k = 0; k < nRand; ++k)
            board[rnd.nextInt(S)][rnd.nextInt(S)] = rnd.nextInt(9);

        // convert to string argument
        boardArg = new String[S];
        for (int i = 0; i < S; ++i) {
            char[] row = new char[S];
            for (int j = 0; j < S; ++j)
                row[j] = (char)('0' + board[i][j]);
            boardArg[i] = new String(row);
        }
        
        StringBuffer sb = new StringBuffer();
        sb.append("S = ").append(S).append('\n');
        sb.append("Probability of knight in a cell = ").append(pK).append('\n');
        sb.append("Probability of a cell changing = ").append(pC).append('\n');
        sb.append("Max change value = ").append(maxC).append('\n');
        sb.append("Number of cells changing completely randomly = ").append(nRand).append('\n');
        if (seed < 3)
            for (int i = 0; i < S; ++i)
                sb.append(boardArg[i]).append('\n');
        return sb.toString();
      }
      catch (Exception e) {
        addFatalError("An exception occurred while generating test case.");
        e.printStackTrace();
        return "";
      }
    }
    // ---------------------------------------------------
    public double runTest(String seed) {
      try {
        String test = generate(seed);
        if (debug)
            System.out.println(test);

        // call user's solution and get return
        placedK = new boolean[S][S];
        if (proc != null) {
            String[] placement;
            try {
                placement = placeKnights(boardArg);
            } catch (Exception e) {
                addFatalError("Failed to get result from placeKnights.");
                return -1;
            }

            if (placement.length != S) {
                addFatalError("Your return must contain " + S + " strings, but it contained " + placement.length + ".");
                return -1;
            }

            for (int i = 0; i < S; ++i) {
                if (placement[i] == null || placement[i].length() != S) {
                    addFatalError("Each element of your return must be a string of length " + S + ", but element " + i + " was \"" + placement[i] + "\".");
                    return -1;
                }
                for (int j = 0; j < S; ++j) {
                    char c = placement[i].charAt(j);
                    if (c != '.' && c != 'K') {
                        addFatalError("Each character in your return must be '.' or 'K', but element " + i + " contained character '" + c + "'.");
                        return -1;
                    }
                    placedK[i][j] = (c == 'K');
                }
            }
        }
        else {
            // use the knights placement which generated the test (for visualization/scoring demo)
            for (int i = 0; i < S; ++i)
            for (int j = 0; j < S; ++j)
                placedK[i][j] = originalK[i][j];
        }

        // calculate the number of knights attacking each cell
        attacks = new int[S][S];
        for (int i = 0; i < S; ++i)
        for (int j = 0; j < S; ++j)
            if (placedK[i][j])
                for (int k = 0; k < 8; ++k) {
                    int r = i + dr[k];
                    int c = j + dc[k];
                    if (r >= 0 && c >= 0 && r < S && c < S)
                        attacks[r][c]++;
                }

        // calculate the score
        int diff = 0;
        for (int i = 0; i < S; ++i)
        for (int j = 0; j < S; ++j) {
            diff += Math.abs(board[i][j] - attacks[i][j]);
        }

        if (vis) {
            // draw the image
            jf.setSize(SZX+10,SZY+30);
            jf.setVisible(true);
            draw();
        }

        return diff;
      }
      catch (Exception e) {
        addFatalError("An exception occurred while trying to process your program's results.");
        e.printStackTrace();
        return -1;
      }
    }
// ------------- visualization part ----------------------
    static String exec, fileName;
    static boolean vis, debug, save;
    static Process proc;
    JFrame jf;
    Vis v;
    InputStream is;
    OutputStream os;
    BufferedReader br;
    // problem-specific drawing params
    int SZX, SZY;
    static int SZ;
    // ---------------------------------------------------
    String[] placeKnights(String[] board) throws IOException {
        // pass the params to the solution and get the return
        int i;
        StringBuffer sb = new StringBuffer();
        sb.append(board.length).append('\n');
        for (i = 0; i < board.length; ++i)
            sb.append(board[i]).append('\n');
        os.write(sb.toString().getBytes());
        os.flush();

        // get the return - an array of strings
        int nret = Integer.parseInt(br.readLine());
        String[] ret = new String[nret];
        for (i = 0; i < nret; ++i)
            ret[i] = br.readLine();
        return ret;
    }
    // ---------------------------------------------------
    void draw() {
        if (!vis) return;
        v.repaint();
    }
    // ---------------------------------------------------
    int getColor(int expected, int real) {
        // white if the number matches, blue if attacked by fewer, red if attacked by more
        if (expected == real)
            return 0xFFFFFF;
        if (expected > real)
            return (0xF - (expected - real)) * 0x11;
        return (0xF + (expected - real)) * 0x110000;
    }
    // -----------------------------------------
    BufferedImage loadImage() {
        try {
            return ImageIO.read(new File("knight.png"));
        }
        catch (Exception e)
        {   return null;  }
    }
    // ---------------------------------------------------
    public class Vis extends JPanel implements WindowListener {
        public void paint(Graphics g) {
          try {
            // do painting here
            BufferedImage kPic = loadImage();
            boolean drawPic = (kPic != null && SZ >= 30);
            char[] ch;
            BufferedImage bi = new BufferedImage(SZX+5,SZY+5,BufferedImage.TYPE_INT_RGB);
            Graphics2D g2 = (Graphics2D)bi.getGraphics();
            // background
            g2.setColor(new Color(0xD3D3D3));
            g2.fillRect(0,0,SZX+10,SZY+10);
            g2.setFont(new Font("Arial",Font.BOLD, SZ < 16 ? 10 : 12));
            FontMetrics fm = g2.getFontMetrics();

            for (int i = 0; i < S; ++i)
            for (int j = 0; j < S; ++j) {
                // highlight each cell
                g2.setColor(new Color(getColor(board[i][j], attacks[i][j])));
                g2.fillRect(j * SZ, i * SZ, SZ - 1, SZ - 1);
                // draw placed knights (under the numbers)
                if (placedK[i][j])
                    if (drawPic)
                        g2.drawImage(kPic, j * SZ + (SZ - 30) / 2 - 1, i * SZ + (SZ - 30) / 2 - 1, 30, 30, null);
                    else {
                        g2.setColor(Color.BLACK);
                        g2.fillOval(j * SZ + SZ / 8, i * SZ + SZ / 8, SZ * 3 / 4, SZ * 3 / 4);
                    }
                // draw required numbers
                ch = ("" + board[i][j]).toCharArray();
                int h = i * SZ + SZ/2 + fm.getHeight()/2 - 2;
                if (placedK[i][j]) {
                    if (!drawPic) {
                        g2.setColor(Color.WHITE);
                        g2.drawChars(ch,0,ch.length, j * SZ + SZ/2 - fm.charWidth(ch[0])/2, h);
                    } else {
                        g2.setColor(Color.BLACK);
                        g2.drawChars(ch,0,ch.length, j * SZ + SZ/2 + 1, h);
                    }
                } else {
                    g2.setColor(Color.BLACK);
                    g2.drawChars(ch,0,ch.length, j * SZ + SZ/2 - fm.charWidth(ch[0])/2, h);
                }
            }

            g.drawImage(bi,0,0,SZX+5,SZY+5,null);
            if (save) {
                ImageIO.write(bi,"png", new File(fileName +".png"));
            }
      }
      catch (Exception e) { e.printStackTrace(); }
        }
        public Vis() {
            jf.addWindowListener(this);
        }
        // WindowListener
        public void windowClosing(WindowEvent e) {
            if(proc != null)
                try { proc.destroy(); }
                catch (Exception ex) { ex.printStackTrace(); }
            System.exit(0);
        }
        public void windowActivated(WindowEvent e) { }
        public void windowDeactivated(WindowEvent e) { }
        public void windowOpened(WindowEvent e) { }
        public void windowClosed(WindowEvent e) { }
        public void windowIconified(WindowEvent e) { }
        public void windowDeiconified(WindowEvent e) { }
    }
    // ---------------------------------------------------
    public KnightsAttacksVis(String seed) {
        //interface for runTest
        if (vis)
        {   jf = new JFrame();
            v = new Vis();
            jf.getContentPane().add(v);
        }
        if (exec != null) {
            try {
                Runtime rt = Runtime.getRuntime();
                proc = rt.exec(exec);
                os = proc.getOutputStream();
                is = proc.getInputStream();
                br = new BufferedReader(new InputStreamReader(is));
                new ErrorReader(proc.getErrorStream()).start();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        System.out.println("Score = "+runTest(seed));
        if (proc != null)
            try { proc.destroy(); }
            catch (Exception e) { e.printStackTrace(); }
    }
    // ---------------------------------------------------
    public static void main(String[] args) {
        String seed = "1";
        SZ = 12;
        for (int i = 0; i<args.length; i++)
        {   if (args[i].equals("-seed"))
                seed = args[++i];
            if (args[i].equals("-exec"))
                exec = args[++i];
            if (args[i].equals("-vis"))
                vis = true;
            if (args[i].equals("-debug"))
                debug = true;
            if (args[i].equals("-size"))
                SZ = Integer.parseInt(args[++i]);
            if (args[i].equals("-save"))
                save = true;
        }
        if (seed.equals("1") && SZ < 30)
            SZ = 30;
        if (save) {
            fileName = seed;
            vis = true;
        }
        KnightsAttacksVis f = new KnightsAttacksVis(seed);
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
