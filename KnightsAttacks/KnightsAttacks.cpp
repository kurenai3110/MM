#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <cstring>
using namespace std;

#define LIMIT 9.8

#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2800000000ULL
class Timer {
	double start, end;
	double limit;

	double getCycle() {
#ifdef NDEBUG
		unsigned long long time;
		time = __rdtsc();
		return (double)time / LOCAL_SCALE;
#else
		unsigned long long low, high;
		__asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
		return (double)((low) | (high << 32)) / TC_SCALE;
#endif
	}

public:
	Timer() {
		start = getCycle();
	}

	inline double getTime() {
		end = getCycle();
		return end - start;
	}

	bool Over() {
		if (getTime() > limit) {
			return true;
		}
		return false;
	}

	void setLimit(double l) {
		limit = l;
	}
	void setStart() { start = getCycle(); }
	inline double getLimit() { return limit; }
};

class Xor128 {
	unsigned static int x, y, z, w;
public:
	Xor128() {
		x = 31103110, y = 123456789, z = 521288629, w = 88675123;
	}

	unsigned int rand()
	{
		unsigned int t;
		t = (x ^ (x << 11)); x = y; y = z; z = w;
		return(w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
	}
};
unsigned int Xor128::x, Xor128::y, Xor128::z, Xor128::w;

struct POINT {
	int x, y;

	POINT() {}
	POINT(int x_, int y_) { x = x_, y = y_; }

	POINT operator+(const POINT& r)const {
		return POINT(x + r.x, y + r.y);
	}
};

int dir1[] = { 2, 2, 1, 1, -1, -1, -2, -2 };
int dir2[] = { 1, -1, 2, -2, 2, -2, 1, -1 };

bool state[500][500];
bool best_state[500][500];
char Board[500][500];
char targetBoard[500][500];
int changes[500][500];

class KnightsAttacks {
	Timer tmr;
	Xor128 xor128;

	int S;
	vector<string>ret;

	int score,best_score;
public:
	void init(vector<string>& board) {
		tmr.setLimit(LIMIT);

		best_score = 1e8;
		score = 0;

		S = board.size();
		ret.resize(S, string(S, '.'));

		for (int i = 0; i < S; i++)for (int j = 0; j < S; j++) {
			targetBoard[i][j] = board[i][j] - '0';
			score += targetBoard[i][j];
		}

		for (int i = 0; i < S; i++)for (int j = 0; j < S; j++) {
			changes[i][j] = set_change(POINT(i, j), false);
		}

		for (int i = 0; i<S; i++)for (int j = 0; j<S; j++) {
			auto flag = &state[i][j];

			int change = changes[i][j];

			if (change < 0) {
				score += change;
				action(POINT(i,j), *flag);
				*flag ^= true;
			}
		}
	}

	vector<string> placeKnights(vector<string>& board) {
		init(board);

		SA();
		
		for (int i = 0; i < S; ++i) {
			for (int j = 0; j < S; ++j) {
				if (best_state[i][j])ret[i][j] = 'K';
			}
		}
		
		return ret;
	}

	void SA() {
		double startTemp = 0.58;
		double endTemp = 0.22;
		double diffTemp = (endTemp - startTemp) / LIMIT;

		double save_time = 9. + S*S / 500000.;

		//int loop = 0;
		while (!tmr.Over()) {
			for (int i = 0; i < S; ++i) {
				for (int j = 0; j < S; ++j) {
					double current_time = tmr.getTime();
					if (current_time > LIMIT) {
						i = S;
						break;
					}

					int change = changes[i][j];
					if (change > 2)continue;

					auto flag = &state[i][j];

					bool force = false;
					if (change > 0) {
						double temp, prob;
						temp = startTemp + diffTemp * current_time;
						//prob = exp(-change / temp);
						force = change < (22.1807 - log(xor128.rand()))*temp;
						//force = UINT32_MAX * prob > xor128.rand();

						//if(force)cerr <<tmr.getTime() << " " << prob << endl;
					}
					else force = true;

					if (force) {
						score += change;
						action(POINT(i, j), *flag);
						*flag ^= true;

						if (current_time > save_time && score < best_score) {
							best_score = score;
							//for (int i = 0; i < S; i++)for (int j = 0; j < S; j++)best_state[i][j] = state[i][j];
							memcpy(best_state, state, sizeof(state));
						}

						//if(change!=0)cerr << p.x << " " << p.y << endl;
					}

					//loop++;
				}
			}
		}

		//cerr << loop << endl;
		//cerr << best_score << endl;
	}

	inline int set_change(POINT p, bool mode) {
		int change = 0;

		for (int i = 0; i < 8; ++i) {
			POINT np = p + POINT(dir1[i], dir2[i]);
			if (is_outOfRange(np))continue;

			if (mode) {
				if (targetBoard[np.x][np.y] < Board[np.x][np.y])--change;
				else ++change;
			}
			else {
				if (targetBoard[np.x][np.y] > Board[np.x][np.y])--change;
				else ++change;
			}
		}

		return change;
	}

	inline void set_change2(POINT p, bool mode) {
		for (int i = 0; i < 8; ++i) {
			POINT np = p + POINT(dir1[i], dir2[i]);
			if (is_outOfRange(np))continue;

			if (state[np.x][np.y]) {
				if (mode) {
					if (targetBoard[p.x][p.y] == Board[p.x][p.y])changes[np.x][np.y] += 2;
				}
				else {
					if (targetBoard[p.x][p.y] + 1 == Board[p.x][p.y])changes[np.x][np.y] -= 2;
				}
			}
			else {
				if (mode) {
					if (targetBoard[p.x][p.y] - 1 == Board[p.x][p.y])changes[np.x][np.y] -= 2;
				}
				else {
					if (targetBoard[p.x][p.y] == Board[p.x][p.y])changes[np.x][np.y] += 2;
				}
			}
		}
	}

	inline void action(POINT p, bool mode) {
		int tmp = -changes[p.x][p.y];

		for (int i = 0; i < 8; ++i) {
			POINT np = p + POINT(dir1[i], dir2[i]);
			if (is_outOfRange(np))continue;

			if (mode) {
				--Board[np.x][np.y];
				set_change2(np, mode);
			}
			else {
				++Board[np.x][np.y];
				set_change2(np, mode);
			}
		}

		changes[p.x][p.y] = tmp;
	}

	/*int scoring() {
		int score = 0;

		for (int i = 0; i < S; i++) {
			for (int j = 0; j < S; j++) {
				score += abs(targetBoard[i][j] - Board[i][j]);
			}
		}

		return score;
	}*/

	inline bool is_outOfRange(POINT p) {
		if (p.x < 0 || p.x >= S || p.y < 0 || p.y >= S)return true;
		return false;
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
