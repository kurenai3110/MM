#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2500000000ULL
#define SIZE 700
#define LIMIT 1600
#define INF 1e7
using namespace std;

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
			cerr << "LIMIT:" << limit << "s" << " " << "TIME:" << getTime() << "s" << endl;
			cerr << "OVER:" << getTime() - limit << "s" << endl;
			return true;
		}
		return false;
	}

	void setLimit(double l) {
		limit = l;
	}
};

class RandomXor {
	unsigned static int x, y, z, w;
public:
	RandomXor() {
		x = (unsigned)time(NULL), y = 123456789, z = 521288629, w = 88675123;
	}

	unsigned int rand()
	{
		unsigned int t;
		t = (x ^ (x << 11)); x = y; y = z; z = w;
		return(w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
	}
};
unsigned int RandomXor::x, RandomXor::y, RandomXor::z, RandomXor::w;

struct RMQ {
	int n;
	vector<pair<double, int>>maxdat, mindat;
	RandomXor rx;

	void init(int n_) {
		n = 1;
		while (n < n_)n *= 2;

		maxdat.resize(2 * n - 1);
		mindat.resize(2 * n - 1, pair<double, int>(INF, 0));
	}

	void update(int k, double a) {
		k += n - 1;
		maxdat[k] = make_pair(a, k - n + 1);
		mindat[k] = make_pair(a, k - n + 1);

		while (k) {
			k = (k - 1) / 2;
			if (maxdat[2 * k + 1].first > maxdat[2 * k + 2].first)maxdat[k] = maxdat[2 * k + 1];
			else if (maxdat[2 * k + 1].first < maxdat[2 * k + 2].first)maxdat[k] = maxdat[2 * k + 2];
			else maxdat[k] = maxdat[2 * k + 1 + rx.rand() % 2];

			if (mindat[2 * k + 1].first < mindat[2 * k + 2].first)mindat[k] = mindat[2 * k + 1];
			else if (mindat[2 * k + 1].first > mindat[2 * k + 2].first)mindat[k] = mindat[2 * k + 2];
			else mindat[k] = mindat[2 * k + 1 + rx.rand() % 2];
		}
	}
};

class GraphDrawing {
	int N, M;
	double maxScore;
	int pre, prepre, cnt;
	double last;
	int SPLIT;

	vector<vector<int>>G;
	vector<vector<int>>E;
	vector<vector<int>>VM;
	vector<vector<int>>COST;
	vector<vector<int>>E_index;

	vector<pair<int, int>>Pos;
	vector<pair<int, int>>maxPos;

	pair<pair<int, int>, int>section;

	RandomXor rx;
	Timer tmr;
	RMQ rmq;
	mt19937 mt;
public:
	vector<int> plot(int N_, vector<int>& edges) {
		vector<int> ret;

		init(N_, edges);

		solve();

		for (int i = 0; i < N; i++) {
			ret.push_back(maxPos[i].first);
			ret.push_back(maxPos[i].second);
		}

		return ret;
	}

	void init(int N_, vector<int>&edges) {
		N = N_;
		M = edges.size() / 3;
		cnt = 0, pre = prepre = -1;

		tmr.setLimit(LIMIT);
		rmq.init(M);
		mt = mt19937((unsigned)time(NULL));

		Pos.resize(N);
		E.resize(N);
		VM.resize(M); for (int i = 0; i < M; i++)VM[i].resize(2);
		COST.resize(N); for (int i = 0; i < N; i++)COST[i].resize(N);
		E_index.resize(N); for (int i = 0; i < N; i++)E_index[i].resize(N, -1);
		G.resize(SIZE); for (int i = 0; i < SIZE; i++)G[i].resize(SIZE);

		for (int i = 0; i < M; i++) {
			int a, b, l;
			a = edges[i * 3], b = edges[i * 3 + 1], l = edges[i * 3 + 2];
			E[a].push_back(b); E[b].push_back(a);
			COST[a][b] = l; COST[b][a] = l;
			E_index[a][b] = i; E_index[b][a] = i;
			VM[i][0] = a; VM[i][1] = b;
		}

		for (int i = 0; i < N; i++) {
			pair<int, int>pi;
			do {
				pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
			} while (G[pi.first][pi.second]);

			sort(E[i].begin(), E[i].end());

			Pos[i] = pi;
			G[pi.first][pi.second] = i + 1;
		}
		for (int i = 0; i < M; i++) {
			rmq.update(i, dist(Pos[VM[i][0]], Pos[VM[i][1]]) / COST[VM[i][0]][VM[i][1]]);
		}

		maxPos = Pos;
		maxScore = rmq.mindat[0].first / rmq.maxdat[0].first;
		last = tmr.getTime();
	}

	void solve() {
		while (1) {
			cnt++;

			setSPLIT();

			section = make_pair(make_pair(0, 0), SIZE);
			int i = First(pre, prepre);
			pair<int, int>pi = Pos[i];
			pair<int, int>prevPos = Pos[i];
			pair<double, double>PI(1, 0);
			for (int j = 0; j < E[i].size(); j++) {
				int i2 = E[i][j];
				int k = E_index[i][i2];
				rmq.update(k, 1);
			}
			while (1) {
				if (tmr.Over()) {
					cerr << "Count " << cnt << endl;
					cerr << "LastUpdate " << last << endl;
					return;
				}

				bool find = false;
				pair<int, int>tmppi;
				vector<pair<int, int>>kouho;
				if (SPLIT == 10 && section.second == 7)SPLIT = 7;
				for (int t = 0; t < SPLIT*SPLIT; t++) {
					tmppi = make_pair(section.first.first + (section.second / SPLIT) * (t / SPLIT) + rx.rand() % (section.second / SPLIT), section.first.second + (section.second / SPLIT) * (t % SPLIT) + rx.rand() % (section.second / SPLIT));
					if (G[tmppi.first][tmppi.second])continue;

					Pos[i] = tmppi;

					pair<double, double>tmpdat = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
					for (int j = 0; j < E[i].size(); j++) {
						int i2 = E[i][j];
						tmpdat.first = max(tmpdat.first, dist(Pos[i], Pos[i2]) / COST[i][i2]);
						tmpdat.second = min(tmpdat.second, dist(Pos[i], Pos[i2]) / COST[i][i2]);
						if (tmpdat.second / tmpdat.first < PI.second / PI.first)break;
					}

					if (tmpdat.second / tmpdat.first > PI.second / PI.first) {
						PI = make_pair(tmpdat.first, tmpdat.second);
						find = true;

						kouho.clear();
						kouho.push_back(tmppi);
					}
					else if (tmpdat.second / tmpdat.first == PI.second / PI.first) {
						kouho.push_back(tmppi);
					}
				}
				if (find) {
					shuffle(kouho.begin(), kouho.end(), mt);
					pi = kouho[0];
				}

				section.first.first += (section.second / SPLIT) * ((Pos[i].first - section.first.first) / (section.second / SPLIT));
				section.first.second += (section.second / SPLIT) * ((Pos[i].second - section.first.second) / (section.second / SPLIT));
				section.second /= SPLIT;

				bool rule = false;
				if (Chance2())rule = true;
				if (SPLIT != 10 && section.second < SPLIT)rule = true;
				if (find == false)rule = true;

				if (rule) {
					Pos[i] = pi;
					for (int j = 0; j < E[i].size(); j++) {
						int i2 = E[i][j];
						int k = E_index[i][i2];
						rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
					}

					G[prevPos.first][prevPos.second] = 0;
					G[Pos[i].first][Pos[i].second] = i + 1;

					if (maxScore < PI.second / PI.first) {
						maxScore = PI.second / PI.first;
						last = tmr.getTime();
						maxPos = Pos;
						cerr << "SPLIT:" << SPLIT << endl;
						cerr << tmr.getTime() / LIMIT << endl;
						cerr << "MaxScore:" << maxScore << endl;
					}

					prepre = pre;
					pre = i;
					break;
				}
			}
		}
	}

	int First(int& pre, int& prepre) {
		int target = rmq.maxdat[0].first >= 1 / rmq.mindat[0].first ? rmq.maxdat[0].second : rmq.mindat[0].second;

		int next = rx.rand() % 2;
		if (VM[target][next] == pre)next ^= 1;
		else if (VM[target][next] == prepre && VM[target][next ^ 1] != pre)next ^= 1;

		return VM[target][next];
	}

	bool Chance2() {
		double probability;
		double t = tmr.getTime() / LIMIT;
		if (t > 0.8)probability = 0;
		else if (t > 0.6)probability = 0.0000;
		else if (t > 0.3)probability = 0.0000;
		else if (t > 0.05)probability = 0.000;
		else probability = 0.2;

		return probability > (double)rx.rand() / UINT32_MAX;
	}

	void setSPLIT() {
		double t = tmr.getTime() / LIMIT;
		if (t > 0.8)SPLIT = 10;
		else if (t > 0.6)SPLIT = 8;
		else if (t > 0.2)SPLIT = 6;
		else if (t > 0.05)SPLIT = 4;
		else SPLIT = 2;
	}

	double dist(pair<int, int>a, pair<int, int>b) {
		return sqrt((a.first - b.first)*(a.first - b.first) + (a.second - b.second)*(a.second - b.second));
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
