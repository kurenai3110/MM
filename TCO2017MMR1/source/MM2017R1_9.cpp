#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <cmath>
#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2500000000ULL
#define SIZE 701
#define LIMIT 1000
#define PI 3.141592653589793
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
	void setStart() { start = getCycle(); }
};

class RandomXor {
	unsigned static int x, y, z, w;
public:
	RandomXor() {
		x = (unsigned)time(NULL), y = 123456789, z = 521288629, w = 88675123;
		//x = 31103110, y = 123456789, z = 521288629, w = 88675123;
		cerr <<"seed:"<< x << endl;
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
	int pre, prepre, cnt, loop;
	double last;
	int SPLIT;

	vector<vector<int>>G;
	vector<vector<int>>E;
	vector<vector<int>>VM;
	vector<vector<int>>COST;
	vector<vector<int>>E_index;

	vector<pair<int, int>>Pos;
	vector<pair<int, int>>maxPos;

	vector<pair<int, int>>kouho;

	RandomXor rx;
	Timer tmr;
	RMQ rmq;
public:
	vector<int> plot(int N_, vector<int>& edges) {
		vector<int> ret;

		init(N_, edges);

		solve();

		for (int i = 0; i < N; ++i) {
			ret.push_back(maxPos[i].first);
			ret.push_back(maxPos[i].second);
		}

		return ret;
	}

	void init(int N_, vector<int>&edges) {
		N = N_;
		M = edges.size() / 3;
		cnt = 0, pre = prepre = -1, loop = 0;

		tmr.setLimit(LIMIT);
		rmq.init(M);

		Pos.resize(N);
		E.resize(N);
		VM.resize(M);
		for (int i = 0; i < M; ++i)VM[i].resize(2);
		COST.resize(N);
		for (int i = 0; i < N; ++i)COST[i].resize(N);
		E_index.resize(N);
		for (int i = 0; i < N; ++i)E_index[i].resize(N, -1);
		G.resize(SIZE);
		for (int i = 0; i < SIZE; i++)G[i].resize(SIZE);
		kouho.reserve(100*100);

		for (int i = 0; i < M; ++i) {
			int a, b, l;
			a = edges[i * 3], b = edges[i * 3 + 1], l = edges[i * 3 + 2];
			E[a].push_back(b); E[b].push_back(a);
			COST[a][b] = l; COST[b][a] = l;
			E_index[a][b] = i; E_index[b][a] = i;
			VM[i][0] = a; VM[i][1] = b;
		}

		pair<int, int>pi;
		for (int i = 0; i < N; ++i) {
			do {
				pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
			} while (G[pi.first][pi.second]);

			sort(E[i].begin(), E[i].end());

			Pos[i] = pi;
			++G[pi.first][pi.second];
		}

		for (int i = 0; i < M; ++i) {
			rmq.update(i, dist(Pos[VM[i][0]], Pos[VM[i][1]]) / COST[VM[i][0]][VM[i][1]]);
		}

		maxPos = Pos;
		maxScore = rmq.mindat[0].first / rmq.maxdat[0].first;
		last = tmr.getTime();
	}

	void solve() {
		pair<pair<int, int>, int>section;
		pair<int, int>tmppi;
		pair<int, int>pi;
		pair<int, int>prevPos;
		pair<double, double>Pi;
		pair<double, double>tmpdat;

		while (1) {
			++cnt;
			++loop;
			double t = tmr.getTime() / LIMIT;

			if (prob(t)) {
				rotate();
				loop = 0;
				pre = prepre = -1;
			}

			setSPLIT(t);

			int i = First(pre, prepre);
			int size = E[i].size();
			section = make_pair(make_pair(0, 0), SIZE);
			pi = Pos[i];
			prevPos = Pos[i];
			Pi = make_pair(1, 0);

			for (int j = 0; j < size; ++j) {
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

				for (int t = 0; t < SPLIT*SPLIT; ++t) {
					tmppi = make_pair(section.first.first + (section.second / SPLIT) * (t / SPLIT) + rx.rand() % (section.second / SPLIT), section.first.second + (section.second / SPLIT) * (t % SPLIT) + rx.rand() % (section.second / SPLIT));
					if (G[tmppi.first][tmppi.second])continue;

					Pos[i] = tmppi;

					tmpdat = make_pair(rmq.maxdat[0].first, rmq.mindat[0].first);
					for (int j = 0; j < size; ++j) {
						int i2 = E[i][j];
						tmpdat.first = max(tmpdat.first, max(0.1, dist(Pos[i], Pos[i2])) / COST[i][i2]);
						tmpdat.second = min(tmpdat.second, max(0.1, dist(Pos[i], Pos[i2])) / COST[i][i2]);
						if (tmpdat.second / tmpdat.first < Pi.second / Pi.first)break;
					}

					if (tmpdat.second / tmpdat.first > Pi.second / Pi.first) {
						Pi = make_pair(tmpdat.first, tmpdat.second);
						find = true;

						kouho.clear();
						kouho.push_back(tmppi);
					}
					else if (tmpdat.second / tmpdat.first == Pi.second / Pi.first) {
						kouho.push_back(tmppi);
					}
				}
				if (find) {
					pi = kouho[rx.rand() % kouho.size()];
					Pos[i] = pi;
				}

				section.first.first += (section.second / SPLIT) * ((pi.first - section.first.first) / (section.second / SPLIT));
				section.first.second += (section.second / SPLIT) * ((pi.second - section.first.second) / (section.second / SPLIT));
				section.second /= SPLIT;

				bool rule = false;
				if (section.second < SPLIT) {
					if (SPLIT == 10)SPLIT = 7;
					else rule = true;
				}
				if (find == false)rule = true;

				if (rule) {
					Pos[i] = pi;

					for (int j = 0; j < size; ++j) {
						int i2 = E[i][j];
						int k = E_index[i][i2];
						rmq.update(k, max(0.1, dist(Pos[i], Pos[i2])) / COST[i][i2]);
					}

					--G[prevPos.first][prevPos.second];
					++G[pi.first][pi.second];

					if (maxScore < Pi.second / Pi.first) {
						maxScore = Pi.second / Pi.first;
						maxPos = Pos;
						cerr << "SPLIT:" << SPLIT << endl;
						cerr << tmr.getTime() / LIMIT << endl;
						cerr << "MaxScore:" << maxScore << endl;
						loop = 0;
						last = tmr.getTime();
					}

					prepre = pre;
					pre = i;
					break;
				}
			}
		}
	}

	void setSPLIT(double& t) {
		bool flag = false;

		/*if (t > 0.95) {
			static bool first = true;
			if (first) {
				SPLIT = 100;
				flag = true;
				first = false;
			}
		}
		else */if (t > 0.8) {
			static bool first = true;
			SPLIT = 10;
			if (first) {
				flag = true;
				first = false;
			}
		}
		else if (t > 0.7) {
			static bool first = true;
			SPLIT = 8;
			if (first) {
				flag = true;
				first = false;
			}
		}
		else if (t > 0.5) {
			static bool first = true;
			SPLIT = 6;
			if (first) {
				flag = true;
				first = false;
			}
		}
		else if (t > 0.2) {
			static bool first = true;
			SPLIT = 4;
			if (first) {
				flag = true;
				first = false;
			}
		}
		else SPLIT = 2;

		if (flag) {
			cerr << tmr.getTime() / LIMIT << endl;
			cerr << "MaxScore:" << maxScore << endl;

			for (int i = 0; i < N; ++i) {
				--G[Pos[i].first][Pos[i].second];
			}
			Pos = maxPos;
			for (int i = 0; i < N; ++i) {
				++G[Pos[i].first][Pos[i].second];
			}

			for (int i = 0; i < M; ++i) {
				rmq.update(i, max(0.1, dist(Pos[VM[i][0]], Pos[VM[i][1]])) / COST[VM[i][0]][VM[i][1]]);
			}

			loop = 0;
		}
	}

	int First(int& pre, int& prepre) {
		int target = rmq.maxdat[0].first >= 1 / rmq.mindat[0].first ? rmq.maxdat[0].second : rmq.mindat[0].second;

		int next = rx.rand() % 2;
		if (VM[target][next] == pre)next ^= 1;
		else if (VM[target][next] == prepre && VM[target][next ^ 1] != pre)next ^= 1;

		return VM[target][next];
	}

	bool prob(double t) {
		double probability;

		if (t > 0.90)probability = 0;
		else if (t > 0.8)probability = loop*4e-7;
		else if (t > 0.7)probability = loop*4e-7;
		else if (t > 0.5)probability = loop*4e-7;
		else if (t > 0.2)probability = loop*4e-7;
		else probability = loop*4e-7;

		return probability > (double)rx.rand() / UINT32_MAX;
	}

	void rotate() {
		constexpr int SIZE2 = SIZE / 2;
		constexpr int to_rad = PI / 180.;
		cerr << loop << endl;
		for (int i = 0; i < N; ++i)--G[Pos[i].first][Pos[i].second];
		int deg = pow(-1, rx.rand() % 2) * (rx.rand() % 10 + 1) / 10.;
		for (int i = 0; i < N; ++i) {
			int x2 = (Pos[i].first - SIZE2)*cos(deg * to_rad) - (Pos[i].second - SIZE2)*sin(deg * to_rad);
			int y2 = (Pos[i].first - SIZE2)*sin(deg * to_rad) + (Pos[i].second - SIZE2)*cos(deg * to_rad);
			if (abs(x2) <= SIZE2 && abs(y2) <= SIZE2) {
				Pos[i] = make_pair(x2 + SIZE2, y2 + SIZE2);
			}
			++G[Pos[i].first][Pos[i].second];
		}
		for (int i = 0; i < M; ++i) {
			rmq.update(i, max(0.1, dist(Pos[VM[i][0]], Pos[VM[i][1]])) / COST[VM[i][0]][VM[i][1]]);
		}
	}

	double dist(pair<int, int>&a, pair<int, int>&b) {
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
