#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <queue>
#include <iomanip>
#include <random>
#include <time.h>
#include <array>
#include <set>
using namespace std;

#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2500000000ULL
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
	Timer(double l) {
		start = getCycle();
		limit = l;
	}

	double getTime() {
		end = getCycle();
		return end - start;
	}

	bool Over() {
		if (getTime() > limit) {
			return true;
		}
		return false;
	}

	double getLimit() { return limit; }
	void setLimit(double l) {
		limit = l;
	}
	void setStart() { start = getCycle(); }
};

class Xor128 {
	unsigned static int x, y, z, w;
public:
	Xor128() {
		x = (unsigned)time(NULL), y = 123456789, z = 521288629, w = 88675123;
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

struct REGION {
	int id;
	POINT tl, br;

	REGION() { id=0; }
};
struct INFO {
	int cnt, sum;
	vector<POINT>links[4];

	INFO() { cnt = 0, sum = 0; }
};

vector<vector<REGION>>cell;
INFO Info[250000];

int v[] = { -1, 0, 1, 0 };
int h[] = { 0, 1, 0, -1 };

char GRID[500][500];
bool used[500][500];
bool used2[250000];

class ConnectedComponent {
	vector<int>matrix;
	int S,sqrtS;
	vector<int> ret;

	vector<POINT>koho,tmpkoho;
	bool mode;
	int l, r, R;

	vector<POINT>que;
	vector<int>que2;

	double max_score;
	int max_cnt;

	int id_n;

	POINT P0, PS;

	Xor128 xor128;
	Timer tmr;
public:
	void init() {
		S = (int)sqrt(matrix.size());
		//cerr << S << endl;
		ret.resize(S);
		for (int i = 0; i < S; i++)ret[i] = i;

		mode = false;

		id_n = 0;

		que.resize(500000);
		que2.resize(500000);

		P0 = POINT(0, 0);
		PS = POINT(S, S);

		cell.resize(S, vector<REGION>(S));
		sqrtS = sqrt(S);
		for (int i = 0; i < S; i++) {
			for (int j = 0; j < S; j++) {
				int r = i / sqrtS;
				int c = j / sqrtS;
				
				cell[i][j].tl = POINT(r*sqrtS, c*sqrtS);
				cell[i][j].br = POINT(min(S, (r + 1)*sqrtS), min(S, (c + 1)*sqrtS));
			}
		}

		int num = 0;

		for (int i = 0; i < S; i++) {
			for (int j = 0; j < S; j++) {
				GRID[i][j] = matrix[ret[i] * S + ret[j]];
				if (GRID[i][j])num++;
			}
		}
		//cerr << (double)num / (S*S) << endl;
		double p = (double)num / (S*S);
		R = (S / 2 - 1) * (1. - (p - 0.473684)*8.);
		R = min(S / 2 - 1, max(0, R));
		//cerr << S / 2 - 1 << " " << R << endl;
		l = 0, r = S - l;
	}

	vector<int> permute(vector<int>& mat) {
		matrix = mat;

		tmr.setLimit(9.8);
		tmr.setStart();

		init();

		change();

		return ret;
	}

	void change() {
		vector<int>tmpret(ret);
		double score;
		int cnt;
		pair<double, int>p = full_scoring2();
		max_score = score = p.first;
		max_cnt = cnt = p.second;

		int loop = 0;
		while (!tmr.Over()) {
			l = R*pow(tmr.getLimit() - tmr.getTime(), 2) / pow(tmr.getLimit(), 2);
			//l = R * (tmr.getLimit() - tmr.getTime()) / tmr.getLimit();
			l = 0;
			r = S - l;

			int r1, r2;
			r1 = xor128.rand() % (r - l);
			r1 += l;
			do {
				r2 = xor128.rand() % S;
			} while (r1 == r2);

			vector<POINT>tmp = swapping(r1, r2);

			//Timer tmr2;
			cell_update(tmp);
			//cerr << tmr2.getTime() << endl;
			//tmr2.setStart();
			pair<double, int>next_p = part_scoring2(tmp);
			double next_score = next_p.first;
			int next_cnt = next_p.second;
			//cerr << next_score << endl;
			//cerr << tmr2.getTime() << endl;

			double startTemp = S * 5;
			double endTemp = S * 2;
			double temp = startTemp + (endTemp - startTemp) * tmr.getTime() / tmr.getLimit();
			double prob = exp((next_score - max_score) / temp);
			bool force = false;
			force = prob > (double)xor128.rand() / UINT32_MAX;
			//if(force)cerr << ((double)next_score / max_score) <<" "<< prob << endl;

			if (force || next_score >= score) {
				if (next_score > max_score) {
					max_score = next_score;
					tmpret = ret;
				}
				max_cnt = max(max_cnt, next_cnt);

				score = next_score;
				cnt = next_cnt;

				koho = tmp;
			}
			else {
				swapping(r1, r2);
				cell_update(tmp);
			}

			loop++;
		}
		ret = tmpret;

		cerr << loop << endl;
		//cerr << max_score << endl;
	}

	vector<POINT> swapping(int r1, int r2) {
		vector<POINT>tmp;

		swap(ret[r1], ret[r2]);


		if ((bool)GRID[r2][r1] != (bool)GRID[r1][r2]) {
			if (GRID[r2][r1])tmp.push_back(POINT(r1, r2));
			if (GRID[r1][r2])tmp.push_back(POINT(r2, r1));
		}
		if ((bool)GRID[r1][r1] != (bool)GRID[r2][r2]) {
			if (GRID[r1][r1])tmp.push_back(POINT(r2, r2));
			if (GRID[r2][r2])tmp.push_back(POINT(r1, r1));
		}



		for (int i = 0; i < S; i++) {
			if (i != r1 && i != r2) {
				if ((bool)GRID[r1][i] != (bool)GRID[r2][i]) {
					if (GRID[r1][i])tmp.push_back(POINT(r2, i));
					if (GRID[r2][i])tmp.push_back(POINT(r1, i));
				}
			}
			swap(GRID[r1][i], GRID[r2][i]);
		}
		for (int i = 0; i < S; i++) {
			if (i != r1 && i != r2) {
				if ((bool)GRID[i][r1] != (bool)GRID[i][r2]) {
					if (GRID[i][r1])tmp.push_back(POINT(i, r2));
					if (GRID[i][r2])tmp.push_back(POINT(i, r1));
				}
			}
			swap(GRID[i][r1], GRID[i][r2]);
		}

		return tmp;
	}

	inline void bfs(POINT s) {
		Timer tmr2;
		if (used[s.x][s.y])return;
		if (GRID[s.x][s.y] == 0)return;

		REGION regi = cell[s.x][s.y];

		vector<POINT>tmp;

		int id;
		do {
			id = xor128.rand() % (S*S-1);
			id++;
		} while (used2[id]);
		used2[id] = true;

		int sum = 0;
		int cnt = 0;
		int last = 0;
		que[last++] = s;

		for (int q = 0; q < last; q++) {
			POINT p = que[q];
			if (used[p.x][p.y])continue;
			used[p.x][p.y] = true;

			tmp.push_back(p);

			cnt++;
			sum += GRID[p.x][p.y];

			for (int k = 0; k < 4; k++) {
				POINT np = p + POINT(v[k], h[k]);
				if (over(np, regi.tl, regi.br)) {
					if (!over(np, P0, PS)) {
						Info[id].links[k].push_back(p);
					}
					continue;
				}
				if (GRID[np.x][np.y] == 0)continue;
				if (used[np.x][np.y])continue;

				que[last++] = np;
			}
		}

		for (int k = 0; k < tmp.size(); k++) {
			POINT p = tmp[k];
			cell[p.x][p.y].id = id;
			Info[id].sum = sum;
			Info[id].cnt = cnt;

			used[p.x][p.y] = true;
		}

		cerr << tmr2.getTime() << endl;
	}

	void cell_update(vector<POINT>&P) {
		for (int i = 0; i < S; i++) {
			for (int j = 0; j < S; j++) {
				used[i][j] = false;
			}
		}

		for (int i = 0; i < P.size(); i++) {
			POINT p = P[i];
			if (used2[cell[p.x][p.y].id]) {
				used2[cell[p.x][p.y].id] = false;
				Info[cell[p.x][p.y].id].sum = 0;
				Info[cell[p.x][p.y].id].cnt = 0;
				for (int k = 0; k < 4; k++) {
					Info[cell[p.x][p.y].id].links[k].clear();
				}
			}
		}

		//Timer tmr2;	
		for (int i = 0; i < P.size(); i++) {
			POINT p = P[i];
			if (used[p.x][p.y])continue;

			REGION regi = cell[p.x][p.y];

			if (GRID[p.x][p.y]) {
				bfs(p);
			}
			else {
				for (int k = 0; k < 4; k++) {
					POINT np = p + POINT(v[k], h[k]);
					if (over(np, regi.tl, regi.br))continue;
					if (used[np.x][np.y])continue;
					if (GRID[np.x][np.y] == 0)continue;

					bfs(np);
				}
			}
		}
		//cerr << 2 << " " << tmr2.getTime() << endl;
	}

	pair<double, int> full_scoring2() {
		for (int i = 0; i < S; i++) {
			for (int j = 0; j < S; j++) {
				used[i][j] = 0;
			}
		}

		double maxscore = -S*S * 10 * S;
		int maxcnt = 0;

		for (int i = 0; i < S; i++) {
			for (int j = 0; j < S; j++) {
				if (used[i][j])continue;
				if (GRID[i][j] == 0)continue;

				REGION regi = cell[i][j];

				vector<POINT>tmp;

				int id;
				do {
					id = xor128.rand() % (S*S-1);
					id++;
				} while (used2[id]);
				used2[id] = true;

				int sum = 0;
				int cnt = 0;
				int last = 0;
				que[last++] = (POINT(i, j));

				for (int q = 0; q < last; q++) {
					POINT p = que[q];
					if (used[p.x][p.y])continue;
					used[p.x][p.y] = true;

					tmp.push_back(p);

					cnt++;
					sum += GRID[p.x][p.y];

					for (int k = 0; k < 4; k++) {
						POINT np = p + POINT(v[k], h[k]);
						if (over(np, regi.tl, regi.br)) {
							if (!over(np, P0, PS)) {
								Info[id].links[k].push_back(p);
							}
							continue;
						}
						if (GRID[np.x][np.y] == 0)continue;
						if (used[np.x][np.y])continue;

						que[last++] = np;
					}
				}

				for (int k = 0; k < tmp.size(); k++) {
					POINT p = tmp[k];
					cell[p.x][p.y].id = id;
					Info[id].sum = sum;
					Info[id].cnt = cnt;

					used[p.x][p.y] = true;
				}

				double score = sum*sqrt(cnt);

				if (score > maxscore) {
					maxscore = score;
					maxcnt = cnt;
					tmpkoho = tmp;
				}
			}
		}

		return make_pair(maxscore, maxcnt);
	}

	pair<double, int> part_scoring2(vector<POINT>&P) {
		for (int i = 0; i < S; i++) {
			for (int j = 0; j < S; j++) {
				used[i][j] = false;
			}
		}
		vector<char>used3(S*S);

		double maxscore = -S*S * 10 * S;
		int maxcnt = 0;

		for (int i = 0; i < P.size(); i++) {
			POINT s = P[i];
			if (used[s.x][s.y])continue;
			if (GRID[s.x][s.y] == 0)continue;

			REGION s_regi = cell[s.x][s.y];

			int id = s_regi.id;
			int sum = Info[id].sum;
			int cnt = Info[id].cnt;

			if (used3[id])continue;

			int last = 0;
			que2[last++] = id;

			for (int q = 0; q < last; q++) {
				int pid = que2[q];
				if (used3[pid])continue;
				used3[pid] = true;

				sum += Info[pid].sum;
				cnt += Info[pid].cnt;

				for (int k = 0; k < 4; k++) {
					vector<POINT>link = Info[pid].links[k];
					for (int j = 0; j < link.size(); j++) {
						POINT p = link[j];
						POINT np = p + POINT(v[k], h[k]);
						//if (over(np, POINT(l, l), POINT(r, r)))continue;
						if (used[np.x][np.y])continue;
						if (GRID[np.x][np.y] == false)continue;

						que2[last++] = cell[np.x][np.y].id;
					}
				}
			}

			double score = sum*sqrt(cnt);

			if (score > maxscore) {
				maxscore = score;
				maxcnt = cnt;
			}
		}

		return make_pair(maxscore, maxcnt);
	}

	bool over(POINT np, POINT tl, POINT br) {
		if (np.x < tl.x || np.x >= br.x || np.y < tl.y || np.y >= br.y)return true;
		return false;
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
