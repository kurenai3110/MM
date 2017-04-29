#include <algorithm>
#include <iostream>
#include <vector>
#include <time.h>
#include <cstring>
#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2500000000ULL
#define SIZE 701
#define LIMIT 9.99
#define INF 1e7
using namespace std;

int v[] = { -1,-1,-1,0,0,0,1,1,1 };
int h[] = { -1,0,1,-1,0,1,-1,0,1 };
int split[] = { 2,2,4,4,7,7,10,10,15,15,25,25,35,35,70,70,100,100,100,175,175 };
int used[701];

class Timer {
	double start, end;
	double limit;

	double getCycle() {
#ifdef _DEBUG
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

	void setLimit(double l) {
		limit = l;
	}
	void setStart() { start = getCycle(); }
};

class RandomXor {
	unsigned static int x, y, z, w;
public:
	RandomXor() {
		x = 31103110, y = 123456789, z = 521288629, w = 88675123;
	}

	unsigned int rand()
	{
		unsigned int t;
		t = (x ^ (x << 11)); x = y; y = z; z = w;
		return(w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
	}
};
unsigned int RandomXor::x, RandomXor::y, RandomXor::z, RandomXor::w;

pair<double, int>maxdat[32767], mindat[32767];
struct RMQ {
	int n, size;
	RandomXor rx;

	void init(int& n_) {
		n = 1;
		while (n < n_)n *= 2;
		size = 2 * n - 1;

		for (int i = 0; i < size; ++i) {
			mindat[i] = make_pair(INF, i - n + 1);
			maxdat[i] = make_pair(0, i - n + 1);
		}
	}

	void update(int k, double& a) {
		k += n - 1;
		maxdat[k].first = a;
		mindat[k].first = a;

		int x;
		x = k;
		while (x) {
			x = (x - 1) / 2;
			if (maxdat[2 * x + 1].first > maxdat[2 * x + 2].first) {
				if (maxdat[x] == maxdat[2 * x + 1])break;
				maxdat[x] = maxdat[2 * x + 1];
			}
			else if (maxdat[2 * x + 1].first < maxdat[2 * x + 2].first) {
				if (maxdat[x] == maxdat[2 * x + 2])break;
				maxdat[x] = maxdat[2 * x + 2];
			}
			else {
				int r = rx.rand() % 2;
				if (maxdat[x] == maxdat[2 * x + 1 + r])break;
				maxdat[x] = maxdat[2 * x + 1 + r];
			}
		}

		x = k;
		while (x) {
			x = (x - 1) / 2;
			if (mindat[2 * x + 1].first < mindat[2 * x + 2].first) {
				if (mindat[x] == mindat[2 * x + 1])break;
				mindat[x] = mindat[2 * x + 1];
			}
			else if (mindat[2 * x + 1].first > mindat[2 * x + 2].first) {
				if (mindat[x] == mindat[2 * x + 2])break;
				mindat[x] = mindat[2 * x + 2];
			}
			else {
				int r = rx.rand() % 2;
				if (mindat[x] == mindat[2 * x + 1 + r])break;
				mindat[x] = mindat[2 * x + 1 + r];
			}
		}
	}

	void push(int k, double& a) {
		k += n - 1;
		maxdat[k].first = a;
		mindat[k].first = a;
	}

	void querymax() {
		for (int i = n - 2; i + 1; --i) {
			if (maxdat[2 * i + 1].first > maxdat[2 * i + 2].first) {
				maxdat[i] = maxdat[2 * i + 1];
			}
			else if (maxdat[2 * i + 1].first < maxdat[2 * i + 2].first) {
				maxdat[i] = maxdat[2 * i + 2];
			}
			else {
				int r = rx.rand() % 2;
				maxdat[i] = maxdat[2 * i + 1 + r];
			}
		}
	}
	void querymin() {
		for (int i = n - 2; i + 1; --i) {
			if (mindat[2 * i + 1].first < mindat[2 * i + 2].first) {
				mindat[i] = mindat[2 * i + 1];
			}
			else if (mindat[2 * i + 1].first > mindat[2 * i + 2].first) {
				mindat[i] = mindat[2 * i + 2];
			}
			else {
				int r = rx.rand() % 2;
				mindat[i] = mindat[2 * i + 1 + r];
			}
		}
	}
};

bool G[SIZE][SIZE];
int VM[10000][2];
int COST[1000][1000];
int E_index[1000][1000];
int E[1000][1000];
int Esize[1000];
pair<int, int>Pos[1000], maxPos[1000];
pair<int, int> kouho[10000];

class GraphDrawing {
	int N, M;
	double maxScore;
	int pre, prepre;
	int SPLIT;
	double goal;
	int cnt = 0;

	RandomXor rx;
	Timer tmr;
	RMQ rmq;
public:
	vector<int> plot(int& N_, vector<int>& edges) {
		vector<int> ret;

		init(N_, edges);

		solve();

		ret.reserve(2 * N);
		for (int i = 0; i < N; ++i) {
			ret.push_back(maxPos[i].first);
			ret.push_back(maxPos[i].second);
		}

		return ret;
	}

	void init(int& N_, vector<int>&edges) {
		N = N_;
		M = edges.size() / 3;
		pre = prepre = -1, goal = 1;
		used[2] = true;

		tmr.setStart();
		tmr.setLimit(LIMIT);
		rmq.init(M);

		for (int i = 0; i < N; ++i)Esize[i] = 0;

		for (int i = 0; i < M; ++i) {
			int a, b, l;
			a = edges[i * 3], b = edges[i * 3 + 1], l = edges[i * 3 + 2];
			E[a][Esize[a]] = b; E[b][Esize[b]] = a;
			++Esize[a]; ++Esize[b];
			COST[a][b] = l; COST[b][a] = l;
			E_index[a][b] = i; E_index[b][a] = i;
			VM[i][0] = a; VM[i][1] = b;
		}

		for (int i = 0; i < N; ++i) {
			pair<int, int>pi;
			do {
				pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
			} while (G[pi.first][pi.second]);

			Pos[i] = pi;
			G[pi.first][pi.second] = 1;
		}

		for (int i = 0; i < M; ++i) {
			double d = dist(Pos[VM[i][0]], Pos[VM[i][1]]) / COST[VM[i][0]][VM[i][1]];
			rmq.push(i, d);
		}
		rmq.querymax();
		rmq.querymin();

		maxScore = mindat[0].first / maxdat[0].first;
		memcpy(maxPos, Pos, sizeof(Pos));
	}

	void solve() {
		pair<int, int>pi;
		pair<int, int>prevPos;
		pair<int, int>tmppi;
		pair<double, double>tmpdat;
		pair<pair<int, int>, int>section;
		pair<int, int>pos;

		while (1) {
			++cnt;
			setSPLIT();

			section = make_pair(make_pair(0, 0), SIZE);

			int i = First(pre, prepre);

			pi = Pos[i];
			prevPos = Pos[i];

			int size = Esize[i];

			for (int j = 0; j < size; ++j) {
				int i2 = E[i][j];
				int k = E_index[i][i2];
				rmq.update(k, goal);
			}

			double score = 0;

			bool flag = prob();

			while (1) {
				if (tmr.Over()) {
					return;
				}

				bool find = false;
				int k = 0;

				int L = section.second / SPLIT;
				pos.first = (pi.first - section.first.first) / L;
				pos.second = (pi.second - section.first.second) / L;

				for (int s = 0; s < 9; ++s) {
					int t = (pos.first + v[s]) * SPLIT + (pos.second + h[s]);
					if (t < 0 || t >= SPLIT*SPLIT)continue;

					tmppi = make_pair(section.first.first + L * (t / SPLIT) + rx.rand() % L, section.first.second + L * (t % SPLIT) + rx.rand() % L);
					if (G[tmppi.first][tmppi.second])continue;

					Pos[i] = tmppi;

					if (flag)tmpdat = make_pair(maxdat[0].first, mindat[0].first);
					else tmpdat = make_pair(goal, goal);

					for (int j = 0; j < size; ++j) {
						int i2 = E[i][j];

						double d = dist(Pos[i], Pos[i2]) / COST[i][i2];
						tmpdat.first = max(tmpdat.first, d);
						tmpdat.second = min(tmpdat.second, d);

						if (tmpdat.second / tmpdat.first < score)break;
					}

					double tmpscore = tmpdat.second / tmpdat.first;
					if (tmpscore > score) {
						score = tmpscore;
						find = true;

						k = 0;
						kouho[k++] = tmppi;
					}
					else if (tmpscore == score) {
						kouho[k++] = tmppi;
					}
				}

				if (find) {
					pi = kouho[rx.rand() % k];

					if (flag && score == mindat[0].first / maxdat[0].first) {
						//
						break;
					}
				}
				else break;

				section.first.first += L * ((pi.first - section.first.first) / L);
				section.first.second += L * ((pi.second - section.first.second) / L);
				section.second = L;

				if (section.second < SPLIT)break;
			}

			Pos[i] = pi;

			for (int j = 0; j < size; ++j) {
				int i2 = E[i][j];
				int k = E_index[i][i2];
				double d = dist(Pos[i], Pos[i2]) / COST[i][i2];
				rmq.update(k, d);
			}

			G[prevPos.first][prevPos.second] = 0;
			G[Pos[i].first][Pos[i].second] = 1;

			double datscore = mindat[0].first / maxdat[0].first;
			if (maxScore < datscore) {
				maxScore = datscore;
				memcpy(maxPos, Pos, sizeof(Pos));
			}

			prepre = pre;
			pre = i;
		}
	}

	inline void setSPLIT() {
		double t = tmr.getTime() / LIMIT;
		double score = mindat[0].first / maxdat[0].first;
		bool flag = false;

		int index = t * 21.;
		SPLIT = split[max(0, min(21, index))];
		if (used[SPLIT] == false) {
			used[SPLIT] = true;

			flag = true;
		}

		if (flag) {
			for (int i = 0; i < N; ++i) {
				G[Pos[i].first][Pos[i].second] = 0;
			}

			memcpy(Pos, maxPos, sizeof(maxPos));

			for (int i = 0; i < N; ++i) {
				G[Pos[i].first][Pos[i].second] = 1;
			}

			for (int i = 0; i < M; ++i) {
				double d = dist(Pos[VM[i][0]], Pos[VM[i][1]]) / COST[VM[i][0]][VM[i][1]];
				rmq.push(i, d);
			}
			rmq.querymax();
			rmq.querymin();
		}
	}

	inline int First(int& pre, int& prepre) {
		int target = maxdat[0].first / goal >= goal / mindat[0].first ? maxdat[0].second : mindat[0].second;

		int next = rx.rand() % 2;
		if (VM[target][next] == pre)next ^= 1;
		else if (VM[target][next] == prepre && VM[target][next ^ 1] != pre)next ^= 1;

		return VM[target][next];
	}

	inline double dist(pair<int, int>&a, pair<int, int>&b) {
		return sqrt((a.first - b.first)*(a.first - b.first) + (a.second - b.second)*(a.second - b.second));
	}

	inline bool prob() {
		double probability;
		double t = tmr.getTime() / LIMIT;

		if (t > 0.95)probability = 1;
		else if (t > 0.7)probability = 1;
		else if (t > 0.5)probability = 0.9;
		else if (t > 0.3)probability = 0.7;
		else if (t > 0.1)probability = 0.5;
		else probability = 0.2;
		return probability > (double)rx.rand() / UINT32_MAX;
	}
};


// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
	for (int i = 0; i < v.size(); ++i)
		cin >> v[i];
}

int main() {
	GraphDrawing gd;
	int N;
	cin >> N;
	int E;
	cin >> E;
	vector<int> edges(E);
	getVector(edges);

	vector<int> ret = gd.plot(N, edges);
	cout << ret.size() << endl;
	for (int i = 0; i < (int)ret.size(); ++i)
		cout << ret[i] << endl;
	cout.flush();
}
