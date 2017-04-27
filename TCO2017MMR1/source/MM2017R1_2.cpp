#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2500000000ULL
#define SIZE 700
#define SPLIT 4
#define LIMIT 300
#define BATCH 10
#define Pi 3.141592653589793
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
};

class GraphDrawing {
	int N, M;
	double maxScore = 0;
	int pre = -1, prepre = -1, prek = -1, cnt = 0;
	int order[16] = { 0,3,15,12,1,7,14,8,2,11,13,4,5,6,10,9 };
	double last = 0;
	int p = 2;
	ofstream foo;

	vector<vector<int>>G;
	vector<vector<int>>E;
	vector<vector<int>>VM;
	vector<vector<int>>COST;
	vector<vector<int>>E_index;

	vector<pair<int, int>>Pos;
	vector<pair<int, int>>maxPos;

	pair<double, double>PI;

	pair<pair<int, int>, int>section;

	RandomXor rx;
	Timer tmr;
	RMQ rmq;
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

		rmq.init(M);

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
		for (int i = 0; i < N; i++)sort(E[i].begin(), E[i].end());
		log();

		pos_init();
		//pos_init_random();
		//add(0);
		//add(1);
	}

	void log() {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < E[i].size(); j++) {
				foo << i <<" "<< E[i][j] <<" "<< COST[i][E[i][j]] << endl;
			}
		}
	}

	void pos_init_random() {
		for (int i = 0; i < N; i++) {
			pair<int, int>pi;
			do {
				pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
			} while (G[pi.first][pi.second]);

			Pos[i] = pi;
			G[pi.first][pi.second] = i + 1;

			for (int j = 0; j < E[i].size(); j++) {
				if (E[i][j] > i)break;
				int i2 = E[i][j];
				int k = E_index[i][i2];
				rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
			}
		}
		PI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
	}

	void pos_init() {
		for (int i = 0; i < N; i++) {
			pair<int, int>pi;
			do {
				pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
			} while (G[pi.first][pi.second]);

			pair<double, double>tmpPI(1, 0);
			pair<int, int>tmppi;
			section = make_pair(make_pair(0, 0), SIZE);
			bool find = true;
			while (find && section.second / SPLIT) {
				find = false;
				for (int t1 = 0; t1 < 16; t1++) {
					int t = order[t1];
					tmppi = make_pair(section.first.first + (section.second / SPLIT) * (t / SPLIT) + rx.rand() % (section.second / SPLIT), section.first.second + (section.second / SPLIT) * (t % SPLIT) + rx.rand() % (section.second / SPLIT));

					if (G[tmppi.first][tmppi.second])continue;

					Pos[i] = tmppi;

					for (int j = 0; j < E[i].size(); j++) {
						if (E[i][j] > i)break;
						int i2 = E[i][j];
						int k = E_index[i][i2];
						rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
					}

					if (rmq.mindat[0].first / rmq.maxdat[0].first > tmpPI.second / tmpPI.first) {
						tmpPI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
						pi = tmppi;
						find = true;
					}
				}
				section.first.first += (section.second / SPLIT) * ((pi.first - section.first.first) / (section.second / SPLIT));
				section.first.second += (section.second / SPLIT) * ((pi.second - section.first.second) / (section.second / SPLIT));
				section.second /= SPLIT;
			}

			G[Pos[i].first][Pos[i].second] = 0;
			Pos[i] = pi;
			G[pi.first][pi.second] = i + 1;

			for (int j = 0; j < E[i].size(); j++) {
				if (E[i][j] > i)break;
				int i2 = E[i][j];
				int k = E_index[i][i2];
				rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
			}
		}
		PI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
	}

	void add(int i) {
		pair<int, int>pi;
		do {
			pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
		} while (G[pi.first][pi.second]);

		Pos[i] = pi;
		G[pi.first][pi.second] = i + 1;

		for (int j = 0; j < E[i].size(); j++) {
			if (E[i][j] > i)break;
			int i2 = E[i][j];
			int k = E_index[i][i2];
			rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
		}
		PI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);

		maxScore = PI.second / PI.first;
		maxPos = Pos;
	}

	void solve() {
		maxScore = PI.second / PI.first;
		last = tmr.getTime();
		maxPos = Pos;
		vector<int>CNT(N);
		while (1) {
			cnt++;

			section = make_pair(make_pair(0, 0), SIZE);
			int i = First(pre,prepre);
			//cerr << "INDEX " << i << endl;
			CNT[i]++;
			pair<int, int>pi = Pos[i];
			pair<int, int>prevPos = Pos[i];
			pair<double, double>tmpPI = PI;

			while (1) {
				/*if (p < N) {
					if (tmr.getTime() * 2 / LIMIT > (double)(p*p) / ((N - 1)*(N - 1))) {
						add(p);
						p++;
					}
				}*/
				if (tmr.Over()) {
					cerr << "Count:" << cnt << endl;
					cerr << "LastUpdate:" << last << endl;
					for (int i = 0; i < N; i++)cerr << CNT[i] << endl;
					return;
				}

				bool rule = true;

				bool chance1 = false;
				bool find = false;
				pair<int, int>tmppi;
				for (int t1 = 0; t1 < 16; t1++) {
					int t = order[t1];
					tmppi = make_pair(section.first.first + (section.second / SPLIT) * (t / SPLIT) + rx.rand() % (section.second / SPLIT), section.first.second + (section.second / SPLIT) * (t % SPLIT) + rx.rand() % (section.second / SPLIT));
					if (G[tmppi.first][tmppi.second])continue;
					//if (t == (prevPos.first - section.first.first) / (section.second / SPLIT)*SPLIT + (prevPos.second - section.first.second) / (section.second / SPLIT))continue;

					Pos[i] = tmppi;

					for (int j = 0; j < E[i].size(); j++) {
						//if (E[i][j] >= p)break;
						int i2 = E[i][j];
						int k = E_index[i][i2];
						rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
					}

					if (rmq.mindat[0].first / rmq.maxdat[0].first > tmpPI.second / tmpPI.first) {
						tmpPI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
						pi = tmppi;
						find = true;
						chance1 = false;
					}
					else if (Chance1()) {
						//tmpPI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
						pi = tmppi;
						find = true;
						chance1 = true;
					}
				}
				section.first.first += (section.second / SPLIT) * ((pi.first - section.first.first) / (section.second / SPLIT));
				section.first.second += (section.second / SPLIT) * ((pi.second - section.first.second) / (section.second / SPLIT));
				section.second /= SPLIT;

				if (chance1);//cerr << tmr.getTime() << endl;
				else if (tmpPI.second / tmpPI.first > PI.second / PI.first);
				//else if (Chance2());
				else rule = false;
				if (section.second < SPLIT)rule = true;
				bool change = false;
				if (find == false)change = true;
				//cerr <<"Score "<< tmpPI.second / tmpPI.first << endl;
				if (rule) {
					Pos[i] = pi;

					for (int j = 0; j < E[i].size(); j++) {
						//if (E[i][j] >= p)break;
						int i2 = E[i][j];
						int k = E_index[i][i2];
						rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
					}

					G[prevPos.first][prevPos.second] = 0;
					G[pi.first][pi.second] = i + 1;

					tmpPI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
					PI = tmpPI;
					if (maxScore < tmpPI.second / tmpPI.first) {
						maxScore = tmpPI.second / tmpPI.first;
						last = tmr.getTime();
						cerr <<"MaxScore "<< maxScore << endl;
						maxPos = Pos;
						//PI = tmpPI;
					}

					/*if (1) {
						for (int j = 0; j < E[i].size(); j++) {
							//if (E[i][j] >= p)break;
							PosChange(E[i][j]);
						}
					}*/

					prepre = pre;
					pre = i;
					break;
				}
				else if (change) {
					Pos[i] = pi;

					for (int j = 0; j < E[i].size(); j++) {
						//if (E[i][j] >= p)break;
						int i2 = E[i][j];
						int k = E_index[i][i2];
						rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
					}

					G[prevPos.first][prevPos.second] = 0;
					G[pi.first][pi.second] = i + 1;

					if (Change()) {
						for (int i = 0; i < max(1, N / 10); i++) {
							pair<int, int>tmp;
							int r = rx.rand() % N;
							//cerr << r << endl;
							do {
								tmp = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
							} while (G[tmp.first][tmp.second]);

							G[Pos[r].first][Pos[r].second] = 0;
							G[tmp.first][tmp.second] = r + 1;

							Pos[r] = tmp;

							for (int j = 0; j < E[r].size(); j++) {
								int i2 = E[r][j];
								int k = E_index[r][i2];
								rmq.update(k, dist(Pos[r], Pos[i2]) / COST[r][i2]);
							}
						}
					}

					PI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);

					if (maxScore < PI.second / PI.first) {
						maxScore = PI.second / PI.first;
						last = tmr.getTime();
						cerr << "MaxScore " << maxScore << endl;
						maxPos = Pos;
						//PI = tmpPI;
					}

					prepre = pre;
					pre = i;
					break;
				}
				else {
					Pos[i] = prevPos;
				}
			}
		}
	}

	void PosChange(int i) {
		section = make_pair(make_pair(0, 0), SIZE);
		pair<int, int>pi = Pos[i];
		pair<int, int>prevPos = Pos[i];
		pair<double, double>tmpPI = PI;

		bool find = true;
		while (find && section.second / SPLIT) {
			if (tmr.Over()) {
				cerr << "Count:" << cnt << endl;
				cerr << "LastUpdate:" << last << endl;
				return;
			}
			find = false;
			pair<int, int>tmppi;
			for (int t1 = 0; t1 < 16; t1++) {
				int t = order[t1];
				tmppi = make_pair(section.first.first + (section.second / SPLIT) * (t / SPLIT) + rx.rand() % (section.second / SPLIT), section.first.second + (section.second / SPLIT) * (t % SPLIT) + rx.rand() % (section.second / SPLIT));
				if (G[tmppi.first][tmppi.second])continue;

				Pos[i] = tmppi;

				for (int j = 0; j < E[i].size(); j++) {
					//if (E[i][j] >= p)break;
					int i2 = E[i][j];
					int k = E_index[i][i2];
					rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
				}

				if (rmq.mindat[0].first / rmq.maxdat[0].first > tmpPI.second / tmpPI.first) {
					tmpPI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
					pi = tmppi;
					find = true;
				}
			}
			section.first.first += (section.second / SPLIT) * ((pi.first - section.first.first) / (section.second / SPLIT));
			section.first.second += (section.second / SPLIT) * ((pi.second - section.first.second) / (section.second / SPLIT));
			section.second /= SPLIT;
		}

		Pos[i] = pi;

		for (int j = 0; j < E[i].size(); j++) {
			//if (E[i][j] >= p)break;
			int i2 = E[i][j];
			int k = E_index[i][i2];
			rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
		}

		G[prevPos.first][prevPos.second] = 0;
		G[pi.first][pi.second] = i + 1;

		tmpPI = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
		PI = tmpPI;
		if (maxScore < tmpPI.second / tmpPI.first) {
			maxScore = tmpPI.second / tmpPI.first;
			last = tmr.getTime();
			cerr << "MaxScore " << maxScore << endl;
			maxPos = Pos;
		}
	}

	int First(int& pre, int& prepre) {
		int target = rmq.maxdat[0].first >= 1 / rmq.mindat[0].first ? rmq.maxdat[0].second : rmq.mindat[0].second;

		int next = rx.rand() % 2;
		if (VM[target][next] == pre)next ^= 1;
		else if (VM[target][next] == prepre && VM[target][next^1] != pre)next ^= 1;

		return VM[target][next];
	}

	bool Chance1() {
		double startTemp = 25;
		double endTemp = 10;
		double temp = startTemp + (endTemp - startTemp) * tmr.getTime() / LIMIT;
		//temp = 25;
		double probability = exp(-100 / temp);
		//cerr << probability << endl;
		return probability > (double)rx.rand() / UINT32_MAX;
	}
	bool Chance2() {
		double startTemp = 100;
		double endTemp = 10;
		double temp = startTemp + (endTemp - startTemp) * tmr.getTime() / LIMIT;
		//temp = 25;
		double probability = exp(-100 / temp);
		//cerr << probability << endl;
		return probability > (double)rx.rand() / UINT32_MAX;
	}
	bool Change() {
		double startTemp = 25;
		double endTemp = 10;
		double temp = startTemp + (endTemp - startTemp) * tmr.getTime() / LIMIT;
		//temp = 50;
		double probability = exp(-100 / temp);
		//cerr<<" Change " << probability << endl;
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
