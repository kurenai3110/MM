// C++11
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class KnightsAttacks {
public:
    vector<string> placeKnights(vector<string> board) {
        int S = board.size();
        vector<string> ret(S, string(S, '.'));
        srand(123);
        for (int i = 0; i < S; ++i)
        for (int j = 0; j < S; ++j)
            if (rand() % 2)
                ret[i][j] = 'K';
        return ret;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
    for (int i = 0; i < v.size(); ++i)
        cin >> v[i];
}

int main() {
    KnightsAttacks ka;
    int S;
    cin >> S;
    vector<string> board(S);
    getVector(board);
    
    vector<string> ret = ka.placeKnights(board);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}
