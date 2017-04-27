// C++11
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <functional>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <time.h>
#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2500000000ULL
#define SIZE 700
#define SPLIT 4
#define LIMIT 180
#define Pi 3.141592653589793
#define BATCH 5
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
		//cerr << end - start << endl;
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
	unsigned int x, y, z, w;
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

struct RMQ {
	int n;
	vector<pair<double,int>>maxdat,mindat;

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
			maxdat[k] = max(maxdat[2 * k + 1], maxdat[2 * k + 2]);
			mindat[k] = min(mindat[2 * k + 1], mindat[2 * k + 2]);
		}
	}

	void add1(int k) {
		k += n - 1;
		if (mindat[k].first == INF) {
			mindat[k].first = 0;
			mindat[k].second = k - n + 1;
		}
		mindat[k].first++;
		while (k) {
			k = (k - 1) / 2;
			mindat[k] = min(mindat[2 * k + 1], mindat[2 * k + 2]);
		}
	}
};

class GraphDrawing {
	int N, M;
	double maxScore;
	int pre, loop, cnt;

	vector<vector<int>>G;
	vector<vector<int>>E;
	vector<vector<int>>VM;
	vector<vector<int>>COST;
	vector<vector<int>>E_index;

	vector<pair<int, int>>Pos;
	vector<pair<int, int>>maxPos;

	vector<vector<int>>GRID;
	vector<int>HAS;
	int has;

	RandomXor rx;
	Timer tmr;
	RMQ rmq;
	RMQ rmq2;

	pair<double, double>PI;
	pair<pair<int, int>, int>section;

	ofstream foo;
public:
	vector<int> plot(int N_, vector<int> edges) {
		vector<int> ret;

		init(N_, edges);

		solve();

		for (int i = 0; i < N; i++) {
			pair<int, int>pi = maxPos[i];
			ret.push_back(pi.first);
			ret.push_back(pi.second);
		}

		return ret;
	}

	void init(int N_, vector<int>&edges) {
		tmr.setLimit(LIMIT);
		foo.open("log.txt", ios::out);

		N = N_;
		M = edges.size() / 3;
		maxScore = 0;
		pre = -1;
		loop = cnt = 0;

		section = make_pair(make_pair(0, 0), SIZE);

		rmq.init(M);
		rmq2.init(N);

		Pos.resize(N);
		E.resize(N);
		VM.resize(M);
		for (int i = 0; i < M; i++)VM[i].resize(2);
		COST.resize(N);
		for (int i = 0; i < N; i++)COST[i].resize(N);
		E_index.resize(N);
		for (int i = 0; i < N; i++)E_index[i].resize(N, -1);
		G.resize(SIZE);
		for (int i = 0; i < SIZE; i++)G[i].resize(SIZE);

		for (int i = 0; i < M; i++) {
			int a, b, l;
			a = edges[i * 3], b = edges[i * 3 + 1], l = edges[i * 3 + 2];
			E[a].push_back(b); E[b].push_back(a);
			COST[a][b] = l; COST[b][a] = l;
			E_index[a][b] = i; E_index[b][a] = i;
			VM[i][0] = a; VM[i][1] = b;
		}

		GRID.resize(10);
		for (int j = 0; j < 10; j++)GRID[j].resize(10);
		HAS.resize(10 * 10 * 1000 * 1000);
		has = 0;

		for (int i = 0; i < N; i++) {
			pair<int, int>pi;
			do {
				pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
			} while (G[pi.first][pi.second]);

			sort(E[i].begin(), E[i].end());
			foo << "Esize " << i << " " << E[i].size() << endl;

			Pos[i] = pi;
			G[pi.first][pi.second] = i + 1;

			GRID[Pos[i].first / 70][Pos[i].second / 70] += (i + 1);
			has += (pi.first / 70 * 10 + pi.second / 70)*(i + 1);

			for (int j = 0; j < E[i].size(); j++) {
				if (E[i][j] > i)break;
				int i2 = E[i][j];
				int k = E_index[i][i2];
				rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
			}
		}
		PI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
		HAS[has]++;
	}

	void solve() {
		maxScore = PI.second / PI.first;
		maxPos = Pos;
		while (1) {
			cnt++;

			section = make_pair(make_pair(0, 0), SIZE);
			int i = First(pre);
			foo << "INDEX " << i << endl;
			rmq2.add1(i);
			while (1) {
				if (tmr.Over()) {
					cerr << "Count:" << cnt << endl;
					return;
				}

				bool rule = true;
				pair<int, int>prevPos = Pos[i];
				pair<double, double>tmpPI(1, 0);
				pair<int, int>pi = Pos[i];
				pair<int, int>tmppi;
				foo << "SIZE " << section.second << endl;
				for (int t = 0; t < 16; t++) {
					//for (int t = 0; t < BATCH; t++) {
						//do {
					tmppi = make_pair(section.first.first + (section.second / SPLIT) * (t / SPLIT) + rx.rand() % (section.second / SPLIT), section.first.second + (section.second / SPLIT) * (t % SPLIT) + rx.rand() % (section.second / SPLIT));
					//if (tmppi.first >= section.first.first + section.second)continue;
					//if (tmppi.second >= section.first.second + section.second)continue;
					//} while (G[tmppi.first][tmppi.second]);
					if (G[tmppi.first][tmppi.second])continue;

					Pos[i] = tmppi;

					for (int j = 0; j < E[i].size(); j++) {
						int i2 = E[i][j];
						int k = E_index[i][i2];
						rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
					}

					if (rmq.mindat[0].first / rmq.maxdat[0].first > tmpPI.second / tmpPI.first) {
						tmpPI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
						pi = tmppi;
					}
				}
				section.first.first += (section.second / SPLIT) * ((pi.first - section.first.first) / (section.second / SPLIT));
				section.first.second += (section.second / SPLIT) * ((pi.second - section.first.second) / (section.second / SPLIT));
				section.second = section.second / SPLIT;

				GRID[prevPos.first / 70][prevPos.second / 70] -= (i + 1);
				has -= (prevPos.first / 70 * 10 + prevPos.second / 70)*(i + 1);
				GRID[pi.first / 70][pi.second / 70] += (i + 1);
				has += (pi.first / 70 * 10 + pi.second / 70)*(i + 1);

				//foo << tmpPI.second / tmpPI.first << " " << PI.second / PI.first << endl;
				if (tmpPI.second / tmpPI.first > PI.second / PI.first);
				else if (Chance()) {
					GRID[pi.first / 70][pi.second / 70] -= (i + 1);
					has -= (pi.first / 70 * 10 + pi.second / 70)*(i + 1);
					do {
						pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
					} while (G[pi.first][pi.second]);
					Pos[i] = pi;
					GRID[pi.first / 70][pi.second / 70] += (i + 1);
					has += (pi.first / 70 * 10 + pi.second / 70)*(i + 1);

					for (int j = 0; j < E[i].size(); j++) {
						int i2 = E[i][j];
						int k = E_index[i][i2];
						rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
					}
					tmpPI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
				}
				/*else if (Chance()) {
					GRID[prevPos.first / 70][prevPos.second / 70] += (i + 1);
					has += (prevPos.first / 70 * 10 + prevPos.second / 70)*(i + 1);
					GRID[pi.first / 70][pi.second / 70] -= (i + 1);
					has -= (pi.first / 70 * 10 + pi.second / 70)*(i + 1);
					Pos[i] = prevPos;
					change();
					break;
				}*/
				//else if (Chance3(HAS[has], cnt));
				else rule = false;

				if (section.second < SPLIT)rule = true;

				loop++;

				if (rule) {
					Pos[i] = pi;
					//cerr << Pos[i].first << " " << Pos[i].second << endl;
					for (int j = 0; j < E[i].size(); j++) {
						int i2 = E[i][j];
						int k = E_index[i][i2];
						rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
					}

					G[prevPos.first][prevPos.second] = 0;
					G[pi.first][pi.second] = i + 1;

					PI = tmpPI;
					if (maxScore < tmpPI.second / tmpPI.first) {
						maxScore = tmpPI.second / tmpPI.first;
						cerr << maxScore << endl;
						foo << "maxScore " << maxScore << endl;
						foo << "場面 " << HAS[has] << endl;
						foo << "LOOP " << loop << endl;
						maxPos = Pos;
						//PI = tmpPI;
						loop = 0;
					}
					else foo << "あがり場面 " << HAS[has] << endl;
					pre = i;

					HAS[has]++;
					break;
				}
				else {
					foo << "はずれ場面 " << HAS[has] << endl;
					GRID[prevPos.first / 70][prevPos.second / 70] += (i + 1);
					has += (prevPos.first / 70 * 10 + prevPos.second / 70)*(i + 1);
					GRID[pi.first / 70][pi.second / 70] -= (i + 1);
					has -= (pi.first / 70 * 10 + pi.second / 70)*(i + 1);
					Pos[i] = prevPos;
				}
			}
		}
	}

	bool swap10(int i) {
		for (int t = 0; t < 1; t++) {
			bool rule = true;

			int i2;
			do {
				i2 = rx.rand() % N;
			} while (i == i2);

			swap(Pos[i], Pos[i2]);

			for (int j = 0; j < E[i].size(); j++) {
				int j2 = E[i][j];
				int k = E_index[i][j2];
				rmq.update(k, dist(Pos[i], Pos[j2]) / COST[i][j2]);
			}
			for (int j = 0; j < E[i2].size(); j++) {
				int j2 = E[i2][j];
				int k = E_index[i2][j2];
				rmq.update(k, dist(Pos[i2], Pos[j2]) / COST[i2][j2]);
			}

			//if (rmq.mindat[0].first / rmq.maxdat[0].first > PI.second / PI.first);
			//else if (Chance());
			//else if (Chance3(PASS[i][pi.first / 70][pi.second / 70], PASSCNT[i]/100.));
			//else rule = false;

			if (rule) {
				swap(G[Pos[i].first][Pos[i].second], G[Pos[i2].first][Pos[i2].second]);

				PI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
				if (maxScore < PI.second / PI.first) {
					maxScore = PI.second / PI.first;
					cerr << maxScore << endl;
					maxPos = Pos;
				}
				return true;
			}
			else {
				swap(Pos[i], Pos[i2]);
				for (int j = 0; j < E[i].size(); j++) {
					int j2 = E[i][j];
					int k = E_index[i][j2];
					rmq.update(k, dist(Pos[i], Pos[j2]) / COST[i][j2]);
				}
				for (int j = 0; j < E[i2].size(); j++) {
					int j2 = E[i2][j];
					int k = E_index[i2][j2];
					rmq.update(k, dist(Pos[i2], Pos[j2]) / COST[i2][j2]);
				}
				return false;
			}
		}
	}

	void change() {
		int i = rmq2.mindat[0].second;
		foo << "CHANGE " << i << endl;

		pair<int, int>pi;
		do {
			pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
		} while (G[pi.first][pi.second]);

		G[Pos[i].first][Pos[i].second] = 0;
		G[pi.first][pi.second] = i + 1;

		GRID[Pos[i].first / 70][Pos[i].second / 70] -= (i + 1);
		has -= (Pos[i].first / 70 * 10 + Pos[i].second / 70)*(i + 1);
		Pos[i] = pi;
		GRID[pi.first / 70][pi.second / 70] += (i + 1);
		has += (pi.first / 70 * 10 + pi.second / 70)*(i + 1);

		for (int j = 0; j < E[i].size(); j++) {
			int i2 = E[i][j];
			int k = E_index[i][i2];
			rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
		}

		PI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);

		if (maxScore < PI.second / PI.first) {
			maxScore = PI.second / PI.first;
			foo << "maxScore " << maxScore << endl;
			foo << "場面 " << HAS[has] << endl;
			foo << "LOOP " << loop << endl;
			maxPos = Pos;
			//PI = tmpPI;
			loop = 0;
		}

		if (rmq2.mindat[i + rmq2.n - 1].first == INF)rmq2.update(i, 0);
		rmq2.update(i, rmq2.mindat[i + rmq2.n - 1].first + 1);
		pre = i;
	}

	int First(int pre) {
		int target = rmq.maxdat[0].first >= 1 / rmq.mindat[0].first ? rmq.maxdat[0].second : rmq.mindat[0].second;
		int next = rx.rand() % 2;
		if (VM[target][next] == pre)next = 1 - next;
		return VM[target][next];
	}

	bool Chance() {
		double startTemp = 50;
		double endTemp = 10;
		double temp = startTemp + (endTemp - startTemp) * tmr.getTime() / LIMIT;
		//temp = 25;
		//double probability = exp(-250*(1- nextScore/Score) / temp);
		double probability = exp(-100 / temp);
		foo << "TEMP " << temp << endl;
		foo << "P " << probability << endl;
		//foo << nextScore << " " << Score << endl;
		//foo << probability << endl;
		return probability > (double)rx.rand() / UINT32_MAX;
	}

	bool Chance3(double nextScore, double Score) {
		double startTemp = 100;
		double endTemp = 10;
		double temp = startTemp + (endTemp - startTemp) * tmr.getTime() / LIMIT;
		//temp = 20;
		//cerr << nextScore << " " << Score << endl;
		//cerr<< -10*(nextScore/Score) << " "<<temp <<endl;
		double probability = exp(-nextScore);
		//cerr << probability << endl;
		return probability > (double)rx.rand() / UINT32_MAX;
	}

	bool Chance4() {
		double startTemp = (M / N) * 1000;
		double endTemp = (M / N) * 100;
		double temp = startTemp + (endTemp - startTemp) * (1 / (1 + exp(-(tmr.getTime() - LIMIT / 10.) / (LIMIT / 10.))));
		double probability = exp(-startTemp / temp);
		//cerr << probability << endl;
		return probability > (double)rx.rand() / UINT32_MAX;
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
