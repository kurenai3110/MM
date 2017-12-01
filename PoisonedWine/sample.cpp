#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class PoisonTest {
public:
    static vector<int> useTestStrips(vector<string> tests) {
        cout << "?" << endl;
        cout << tests.size() << endl;
        for (int i = 0; i < (int)tests.size(); ++i)
            cout << tests[i] << endl;
        cout.flush();

        int n;
        cin >> n;
        vector<int> res(n);
        for (int i = 0; i < n; ++i)
            cin >> res[i];
        return res;
    }
};

// -------8<------- start of solution submitted to the website -----8<-------
class PoisonedWine {
public:
    vector<int> testWine(int numBottles, int testStrips, int testRounds, int numPoison) {
        // do one round, split all bottles in batches and run one test on each batch
        int n = numBottles / testStrips;
        if (numBottles % testStrips)
            n++;
        cerr << "Batch size " << n << "\n";
        vector<string> tests(testStrips);
        for (int i = 0; i < testStrips; ++i) {
            if (i * n >= numBottles) {
                tests[i] = "0";
                continue;
            }
            stringstream A;
            A << (i * n);
            for (int j = 1; j < n && i * n + j < numBottles; ++j)
                A << "," << (i * n + j);
            A >> tests[i];
        }
        vector<int> testRes = PoisonTest::useTestStrips(tests);
        vector<int> bad;
        for (int i = 0; i < (int)testRes.size(); ++i)
            if (testRes[i] == 1) {
                // poison detected - throw out all bottles in this batch
                for (int j = 0; j < n && i * n + j < numBottles; ++j)
                    bad.push_back(i * n + j);
            } else {
                cerr << "Keeping batch " << i << "\n";
            }
        return bad;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

int main() {
    PoisonedWine pw;
    int numBottles, testStrips, testRounds, numPoison;
    cin >> numBottles >> testStrips >> testRounds >> numPoison;
    
    vector<int> ret = pw.testWine(numBottles, testStrips, testRounds, numPoison);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}