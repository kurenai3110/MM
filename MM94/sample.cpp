// C++11
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

class ConnectedComponent {
public:
    vector<int> permute(vector<int> matrix) {
        int S = (int)sqrt(matrix.size());
        vector<int> ret(S);
        for (int i = 0; i < S; ++i) {
            ret[i] = S - 1 - i;
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
    ConnectedComponent cc;
    int M;
    cin >> M;
    vector<int> matrix(M);
    getVector(matrix);
    
    vector<int> ret = cc.permute(matrix);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}
