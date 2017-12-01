// C++11
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
#include <string>

using namespace std;

class CirclesMix {
public:
    vector<int> ret;
    void add(vector<int> t) {
        ret.insert(ret.end(), t.begin(), t.end());
    }
    vector<int> drawImage(int H, vector<int> pixels, int N) {
        add({144, 144, 90, 0xfea700});
        add({144, 144, 90, 0xfea700});
        add({144, 144, 90, 0xfea700});

        add({275, 285, 90, 0xfea700});
        add({275, 285, 90, 0xfea700});
        add({275, 285, 90, 0xfea700});

        add({360, 374, 80, 0});
        add({360, 374, 80, 0});
        add({360, 374, 80, 0});
        add({360, 374, 80, 0});
        add({360, 374, 80, 0});
        add({360, 374, 80, 0});

        add({382, 400, 90, 0xfea700});
        add({382, 400, 90, 0xfea700});

        add({427, 444, 80, 0});
        add({427, 444, 80, 0});
        add({427, 444, 80, 0});
        add({427, 444, 80, 0});
        add({427, 444, 80, 0});
        add({427, 444, 80, 0});

        return ret;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
    for (int i = 0; i < v.size(); ++i)
        cin >> v[i];
}

int main() {
    CirclesMix cm;
    int H;
    cin >> H;
    int S;
    cin >> S;
    vector<int> pixels(S);
    getVector(pixels);
    
    int N;
    cin >> N;
    
    vector<int> ret = cm.drawImage(H, pixels, N);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}
