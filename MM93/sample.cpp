// C++11
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class CrossStitch {
public:
    vector<string> embroider(vector<string> pattern) {
        vector<string> ret;
        int S = pattern.size();
        // for each color, for each cell (r, c) do two stitches (r+1, c)-(r, c+1)-(r+1, c+1)-(r, c)
        for (char col = 'a'; col <= 'z'; ++col) {
            bool first = true;
            for (int r = 0; r < S; ++r)
            for (int c = 0; c < S; ++c)
                if (pattern[r][c] == col) {
                    if (first) {
                        ret.push_back(string(1, col));
                        first = false;
                    }
                    ret.push_back(to_string(r+1) + " " + to_string(c));
                    ret.push_back(to_string(r) + " " + to_string(c+1));
                    ret.push_back(to_string(r+1) + " " + to_string(c+1));
                    ret.push_back(to_string(r) + " " + to_string(c));
                }
        }
        return ret;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
    for (int i = 0; i < v.size(); ++i)
        cin >> v[i];
}

int main() {
    int S;
    cin >> S;
    vector<string> pattern(S);
    getVector(pattern);
    
    CrossStitch cs;
    vector<string> ret = cs.embroider(pattern);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}

