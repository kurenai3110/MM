//C++11
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class WaterfallFishing {
public:
    vector<int> placeTraps(vector<string> data) {
        int W = data[0].size();
        // put traps in all locations on the river
        vector<int> traps;
        for (int j = 0; j < W; ++j)
            traps.push_back(j);
        return traps;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
    for (int i = 0; i < v.size(); ++i)
        cin >> v[i];
}

int main() {
    WaterfallFishing wf;
    int S;
    cin >> S;
    vector<string> data(S);
    getVector(data);
    
    vector<int> ret = wf.placeTraps(data);
    cout << ret.size() << endl;
    for (int i : ret)
        cout << i << endl;
    cout.flush();
}
