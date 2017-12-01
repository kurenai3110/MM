#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <chrono>
#include <random>
#include <functional>
using namespace std;

#define TIME_LIMIT 9.9

//タイマー
class Timer {
	chrono::high_resolution_clock::time_point start, end;
	double limit;

public:
	Timer() {
		start = chrono::high_resolution_clock::now();
	}
	Timer(double l) {
		start = chrono::high_resolution_clock::now();
		limit = l;
	}

	double getTime() {
		end = chrono::high_resolution_clock::now();
		return chrono::duration<double>(end - start).count();
	}

	bool isTimeOver() {
		if (getTime() > limit) {
			return true;
		}
		return false;
	}

	double getLimit() { return limit; }
	void setLimit(double l) {
		limit = l;
	}
	void setStart() { start = chrono::high_resolution_clock::now(); }
};

//疑似乱数
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

struct Edge {
	int from, to, cost;

	Edge(int from = 0, int to = 0, int cost = 0) :from(from), to(to), cost(cost) {}

	bool operator<(const Edge& r)const {
		return cost < r.cost;
	}

	bool operator>(const Edge& r)const {
		return cost > r.cost;
	}
};

int sqrt_Vemb_N;

struct POINT {
	int r, c;
	int rc;

	POINT(int r = 0, int c = 0) :r(r), c(c), rc(r*sqrt_Vemb_N + c) { ; }

	POINT operator+(const POINT p) { return POINT(r + p.r, c + p.c); }
};

int r8[] = { -1,-1,-1,0,1,1,1,0 };
int c8[] = { -1,0,1,1,1,0,-1,-1 };
POINT dir8[8];

int COST[501][501];
Edge V[501][501];
int Vsize[501];

bool Vemb_link[3601][3601];

int nextSTATE[501][3601];
int STATE[501];

int Kouho[501][10000];
int Kouho_size[501];

POINT ret[501];

int Index[3601];
POINT Point[501];

class Solver {
public:
	int V_N, E_N;
	int Vemb_N, Eemb_N;

	Timer tmr;
	Xor128 xor128;

	Solver() { ; }

	void V_init() {
		cin >> V_N >> E_N;

		for (int i = 0; i < E_N; i++) {
			int from, to, cost;
			cin >> from >> to >> cost;
			from--; to--;

			if (cost == 0)continue;

			COST[from][to] = cost;
			COST[to][from] = cost;

			V[from][Vsize[from]++] = Edge(from, to, cost);
			V[to][Vsize[to]++] = Edge(to, from, cost);

			for (int j = 0; j < cost; j++) {
				Kouho[from][Kouho_size[from]++] = to;
				Kouho[to][Kouho_size[to]++] = from;
			}
		}
	}

	void Vemb_init() {
		cin >> Vemb_N >> Eemb_N;

		sqrt_Vemb_N = sqrt(Vemb_N);

		for (int i = 0; i < Eemb_N; i++) {
			int from, to;
			cin >> from >> to;
			from--; to--;

			Vemb_link[from][to] = true;
			Vemb_link[to][from] = true;
		}

		for (int i = 0; i < Vemb_N; i++)Index[i] = -1;
	}

	void dir8_init() {
		for (int k = 0; k < 8; k++) {
			dir8[k] = POINT(r8[k], c8[k]);
		}
	}

	void init() {
		tmr.setLimit(TIME_LIMIT);

		V_init();
		Vemb_init();

		dir8_init();
	}

	bool Over(POINT& P, int N) {
		return P.r < 0 || P.r >= N || P.c < 0 || P.c >= N;
	}

	int scoring(POINT* order) {
		int score = 0;
		for (int u = 0; u < V_N; u++) {
			Edge *Vu = V[u];
			int Vu_size = Vsize[u];

			for (int j = 0; j < Vu_size; j++) {
				POINT p, q;
				p = order[Vu[j].from];
				q = order[Vu[j].to];

				if (Vemb_link[p.rc][q.rc])score += Vu[j].cost;
			}
		}

		return score / 2;
	}

	void greedyInit() {
		vector<pair<int, int>>O;
		for (int i = 0; i < V_N; i++) {
			int cost = 0;
			for (int j = 0; j < Vsize[i]; j++) {
				cost += V[i][j].cost;
			}

			O.emplace_back(make_pair(cost, i));
		}
		sort(O.begin(), O.end());
		reverse(O.begin(), O.end());

		vector<char>status(3601);
		vector<POINT>kouho;
		kouho.emplace_back(POINT(sqrt_Vemb_N / 2, sqrt_Vemb_N / 2));
		status[kouho[0].rc] = 1;
		for (int i = 0; i < V_N; i++) {
			int u = O[i].second;

			int max_cost = 0;
			vector<int>next;
			for (int j = 0; j < kouho.size(); j++) {
				int cost = 0;
				POINT p = kouho[j];
				for (int k = 0; k < 8; k++) {
					POINT np = p + dir8[k];
					if (Over(np, sqrt_Vemb_N))continue;
					if (Index[np.rc] == -1)continue;

					cost += COST[u][Index[np.rc]];
				}

				if (cost > max_cost) {
					max_cost = cost;
					next.clear();
					next.emplace_back(j);
				}
				else if (cost == max_cost) {
					next.emplace_back(j);
				}
			}

			int ind = next[xor128.rand() % next.size()];
			POINT s = kouho[ind];
			Index[s.rc] = u;
			Point[u] = s;

			status[s.rc] = -1;
			swap(kouho[ind], kouho[kouho.size() - 1]);
			kouho.pop_back();

			for (int k = 0; k < 8; k++) {
				POINT t = s + dir8[k];
				if (Over(t, sqrt_Vemb_N))continue;
				if (status[t.rc])continue;
				status[t.rc] = 1;

				kouho.emplace_back(t);
			}
		}

		memcpy(ret, Point, sizeof(Point));
	}

	void initSTATE() {
		for (int u = 0; u < V_N; u++) {
			POINT p = Point[u];

			Edge *Vu = V[u];
			int Vu_size = Vsize[u];

			for (int j = 0; j < Vu_size; j++) {
				int t = Vu[j].to;
				int cost = Vu[j].cost;

				for (int k = 0; k < 8; k++) {
					POINT np = p + dir8[k];
					if (Over(np, sqrt_Vemb_N))continue;

					nextSTATE[t][np.rc] += cost;
				}
			}

			for (int k = 0; k < 8; k++) {
				POINT np = p + dir8[k];
				if (Over(np, sqrt_Vemb_N))continue;

				int t = Index[np.rc];
				if (t == -1)continue;

				STATE[u] += COST[u][t];

				nextSTATE[u][np.rc] += COST[u][t];
			}
		}
	}

	void updateSTATE(int u, POINT& p, int v, POINT& q) {
		Edge *Vu = V[u], *Vv = V[v];
		int Vu_size = Vsize[u], Vv_size = Vsize[v];

		for (int k = 0; k < 8; k++) {
			POINT np = p + dir8[k];
			if (Over(np, sqrt_Vemb_N))continue;

			for (int j = 0; j < Vu_size; j++) {
				int t = Vu[j].to;
				int cost = Vu[j].cost;

				nextSTATE[t][np.rc] -= cost;
			}

			if (v >= 0) for (int j = 0; j < Vv_size; j++) {
				int t = Vv[j].to;
				int cost = Vv[j].cost;

				nextSTATE[t][np.rc] += cost;
			}

			int t = Index[np.rc];
			if (t == -1)continue;

			STATE[t] -= COST[u][t];
			nextSTATE[u][np.rc] -= COST[u][t];
			nextSTATE[t][p.rc] -= COST[u][t];
			if (v >= 0) {
				STATE[t] += COST[v][t];
				nextSTATE[v][np.rc] += COST[v][t];
				nextSTATE[t][p.rc] += COST[v][t];
			}
		}


		for (int k = 0; k < 8; k++) {
			POINT nq = q + dir8[k];
			if (Over(nq, sqrt_Vemb_N))continue;

			if (v >= 0) for (int j = 0; j < Vv_size; j++) {
				int t = Vv[j].to;
				int cost = Vv[j].cost;

				nextSTATE[t][nq.rc] -= cost;
			}

			for (int j = 0; j < Vu_size; j++) {
				int t = Vu[j].to;
				int cost = Vu[j].cost;

				nextSTATE[t][nq.rc] += cost;
			}

			int t = Index[nq.rc];
			if (t == -1)continue;

			if (v >= 0) {
				STATE[t] -= COST[v][t];
				nextSTATE[v][nq.rc] -= COST[v][t];
				nextSTATE[t][q.rc] -= COST[v][t];
			}
			STATE[t] += COST[u][t];
			nextSTATE[u][nq.rc] += COST[u][t];
			nextSTATE[t][q.rc] += COST[u][t];
		}

		if (v >= 0)if (Vemb_link[p.rc][q.rc]) {
			nextSTATE[u][q.rc] -= COST[u][v];
			nextSTATE[v][p.rc] -= COST[u][v];
			nextSTATE[u][p.rc] -= COST[u][v];
			nextSTATE[v][q.rc] -= COST[u][v];
		}

		STATE[u] = nextSTATE[u][q.rc];
		if (v >= 0)STATE[v] = nextSTATE[v][p.rc];
	}


	void SA() {
		int max_score, score;
		max_score = score = 0;

		int ave_deg = 0;
		int zero = 0;
		for (int i = 0; i < V_N; i++) {
			if (Vsize[i] == 0)zero++;
			ave_deg += Vsize[i];
		}
		if (zero == V_N)ave_deg = 1.;
		else ave_deg = max(1, (int)ceil(1.*ave_deg / (V_N - zero)));


		double startTemp = 10, endTemp = 0.5;
		startTemp = 10 - 5. * ave_deg / V_N;
		if (ave_deg <= 8)startTemp = 5;
		double diffTemp = (endTemp - startTemp) / tmr.getLimit();

		int C = -7 - ave_deg / 10;

		//int loop = 0;
		double currentTime = tmr.getTime();
		while (currentTime < TIME_LIMIT) {
			for (int u = 0; u < V_N; u++) {
				POINT p = Point[u];
				POINT q,tmp_q;
				//loop++;
				int change = -10000;
				int v = -1;
				if (Vsize[u]) {
					int* Ku = Kouho[u];
					int Ku_size = Kouho_size[u];
					int* nSu = nextSTATE[u];

					int Vu_size = Vsize[u];
					for (int d = 0; d < Vu_size; d++) {
						tmp_q = Point[Ku[xor128.rand() % Ku_size]] + dir8[xor128.rand() % 8];
						if (Over(tmp_q, sqrt_Vemb_N))continue;

						int tmp_v = Index[tmp_q.rc];
						if (u == tmp_v)continue;

						int tmp_change = nSu[tmp_q.rc];
						if (tmp_v >= 0)tmp_change += nextSTATE[tmp_v][p.rc] - STATE[tmp_v];

						if (tmp_change > change) {
							change = tmp_change;
							v = tmp_v;
							q = tmp_q;
						}
					}
				}
				else continue;

				change -= STATE[u];
				if (change < C)continue;

				bool force = false;
				if (change < 0) force = exp(change / (startTemp + diffTemp * currentTime)) * UINT32_MAX > xor128.rand();
				else force = true;

				if (force) {
					score += change;

					Point[u] = q;
					if (v >= 0)Point[v] = p;
					swap(Index[p.rc], Index[q.rc]);

					if (score > max_score) {
						max_score = score;
						//cerr << score << endl;
						memcpy(ret, Point, sizeof(Point));
					}

					updateSTATE(u, p, v, q);
				}
			}

			currentTime = tmr.getTime();
		}

		//cerr << tmr.getTime() << endl;
		//cerr << loop << endl;
		//cerr << scoring(ret) << " " << max_score << endl;
	}


	void solve() {
		greedyInit();

		initSTATE();

		SA();
	}

	void output() {
		for (int i = 0; i < V_N; i++) {
			cout << i + 1 << " " << ret[i].rc + 1 << endl;
		}
	}

	void run() {
		init();

		solve();

		output();
	}
};

int main() {
	cin.tie(0);
	ios_base::sync_with_stdio(false);

	Solver solver;

	solver.run();

	return 0;
}
