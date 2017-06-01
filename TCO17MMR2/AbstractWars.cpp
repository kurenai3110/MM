// C++11
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

class AbstractWars {
public:
    int B;
    vector<int> baseX, baseY;
    // ----------------------------------------------
    int init(vector <int> baseLocations, int speed) {
        srand(123);
        B = baseLocations.size() / 2;
        for (int i = 0; i < B; ++i) {
            baseX.push_back(baseLocations[2*i]);
            baseY.push_back(baseLocations[2*i+1]);
        }
        return 0;
    }
    // ----------------------------------------------
    vector<int> others;
    // picks a random base to attack based on distance to the opponent bases: the closer the base, the higher the chances are
    int getRandomBase(int sourceInd) {
        vector<double> probs(others.size());
        double sp = 0;
        for (int i = 0; i < (int)others.size(); ++i) {
            int ind = others[i];
            probs[i] = 1 / (pow(baseX[sourceInd] - baseX[ind], 2) + pow(baseY[sourceInd] - baseY[ind], 2));
            sp += probs[i];
        }
        
        double r = rand() * 1.0/RAND_MAX * sp;
        double s = 0;
        for (int i = 0; i < (int)others.size(); ++i) {
            s += probs[i];
            if (s >= r)
                return others[i];
        }
        return others[others.size() - 1];
    }
    // ----------------------------------------------
    vector <int> sendTroops(vector <int> bases, vector <int> troops) {
        // compile the list of bases owned by other players
        others.resize(0);
        for (int i = 0; i < B; ++i)
            if (bases[2*i] != 0)
                others.push_back(i);
        if (others.size() == 0) {
            // noone to fight!
            return vector<int>(0);
        }

        vector<int> att;
        for (int i = 0; i < B; ++i) {
            if (bases[2*i] == 0 && bases[2*i+1] > 1000 * 2 / 3) {
                // send troops to a random base of different ownership
                att.push_back(i);
                att.push_back(getRandomBase(i));
            }
        }
        return att;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
    for (int i = 0; i < v.size(); ++i)
        cin >> v[i];
}

int main() {
    AbstractWars aw;
    int N;
    cin >> N;
    vector<int> baseLoc(N);
    getVector(baseLoc);
    int S;
    cin >> S;
    
    int retInit = aw.init(baseLoc, S);
    cout << retInit << endl;
    cout.flush();
    
    for (int st = 0; st < 2000; ++st) {
        int B;
        cin >> B;
        vector<int> bases(B);
        getVector(bases);
        int T;
        cin >> T;
        vector<int> troops(T);
        getVector(troops);
        
        vector<int> ret = aw.sendTroops(bases, troops);
        cout << ret.size() << endl;
        for (int i = 0; i < (int)ret.size(); ++i) {
            cout << ret[i] << endl;
        }
        cout.flush();
    }
}
