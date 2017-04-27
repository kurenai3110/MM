#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <cmath>
#include <cstring>
#include <array>
//#define LOCAL_SCALE 140784982935ULL
#define LOCAL_SCALE 30000000000ULL
#define TC_SCALE 2500000000ULL
#define SIZE 701
#define LIMIT 9.9
//#define 17.58
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
	void setStart() { start = getCycle(); }
};

class RandomXor {
	unsigned static int x, y, z, w;
public:
	RandomXor() {
		//x = (unsigned)time(NULL), y = 123456789, z = 521288629, w = 88675123;
		x = 31103110, y = 123456789, z = 521288629, w = 88675123;
		//x = 1492873948, y = 123456789, z = 521288629, w = 88675123;
		cerr << "seed " << x << endl;
	}

	unsigned int rand()
	{
		unsigned int t;
		t = (x ^ (x << 11)); x = y; y = z; z = w;
		return(w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
	}
};
unsigned int RandomXor::x, RandomXor::y, RandomXor::z, RandomXor::w;

//array<pair<double, int>, 32767>maxdat, mindat;
pair<double, int>maxdat[32767], mindat[32767];
struct RMQ {
	int n,size;
	RandomXor rx;

	void init(int n_) {
		n = 1;
		while (n < n_)n *= 2;
		size = 2 * n - 1;

		//maxdat.resize(2 * n - 1);
		//mindat.resize(2 * n - 1, pair<double, int>(INF, 0));
		for (int i = 0; i < size; i++) {
			mindat[i] = make_pair(INF, 0);
		}
	}

	void update(int k, double a) {
		k += n - 1;
		maxdat[k] = make_pair(a, k - n + 1);
		mindat[k] = make_pair(a, k - n + 1);

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
		while(x){
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

	void push(int k, double a) {
		k += n - 1;
		maxdat[k] = make_pair(a, k - n + 1);
		mindat[k] = make_pair(a, k - n + 1);
	}

	pair<double, int> querymax(int k) {
		if (k - n + 1 >= 0)return maxdat[k];
		pair<double, int>query1, query2;
		query1 = querymax(2 * k + 1);
		query2 = querymax(2 * k + 2);
		if (query1.first > query2.first)maxdat[k] = query1;
		else if (query1.first < query2.first)maxdat[k] = query2;
		else {
			if(rx.rand()%2)maxdat[k] = query1;
			else maxdat[k] = query2;
		}

		return maxdat[k];
	}
	pair<double, int> querymin(int k) {
		if (k - n + 1 >= 0)return mindat[k];
		pair<double, int>query1, query2;
		query1 = querymin(2 * k + 1);
		query2 = querymin(2 * k + 2);
		if (query1.first < query2.first)mindat[k] = query1;
		else if (query1.first > query2.first)mindat[k] = query2;
		else {
			if (rx.rand() % 2)mindat[k] = query1;
			else mindat[k] = query2;
		}

		return mindat[k];
	}
};

//array<array<int, SIZE>, SIZE>G;
//array<array<int, 2>, 10000>VM;
//array<array<int, 1000>, 1000>COST;
//array<array<int, 1000>, 1000>E_index;
//array<pair<int, int>, 1000>Pos;
//array<pair<int, int>, 1000>maxPos;
int G[SIZE][SIZE];
int VM[10000][2];
int COST[1000][1000];
int E_index[1000][1000];
int E[1000][1000];
//array<array<int, 1000>, 1000>E;
int Esize[1000];
pair<int, int>Pos[1000], maxPos[1000];
pair<int,int> kouho[10000];
int Step[SIZE][SIZE];
pair<int, int>Stepped[SIZE/2+1][2800];
double cntscore[10000000];

class GraphDrawing {
	int N, M;
	double maxScore;
	int pre, prepre, cnt, loop;
	double last;
	int SPLIT;
	double d;

	pair<pair<int, int>, int>section;

	RandomXor rx;
	Timer tmr;
	RMQ rmq;
public:
	vector<int> plot(int N_, vector<int>& edges) {
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
		tmr.setStart();
		N = N_;
		M = edges.size() / 3;
		cnt = 0, pre = prepre = -1, loop = 0, d = 1;

		tmr.setLimit(LIMIT);
		rmq.init(M);

		for (int i = 0; i < 32767; i++) {
			maxdat[i] = make_pair(0,0);
			mindat[i] = make_pair(INF,0);
		}

		for (int i = 0; i < N; i++)Esize[i] = 0;

		Stepped[0][0] = make_pair(350, 350);
		for (int i = 1; i <= 350; i++) {
			int cnt = 0;
			pair<int, int>pos(SIZE / 2 - i, SIZE / 2 - i);
			for (int j = 0; j < i * 2; j++) {
				Step[pos.first][pos.second] = cnt;
				Stepped[i][cnt] = pos;
				cnt++;
				pos.first++;
			}
			for (int j = 0; j < i * 2; j++) {
				Step[pos.first][pos.second] = cnt;
				Stepped[i][cnt] = pos;
				cnt++;
				pos.second++;
			}
			for (int j = 0; j < i * 2; j++) {
				Step[pos.first][pos.second] = cnt;
				Stepped[i][cnt] = pos;
				cnt++;
				pos.first--;
			}
			for (int j = 0; j < i * 2; j++) {
				Step[pos.first][pos.second] = cnt;
				Stepped[i][cnt] = pos;
				cnt++;
				pos.second--;
			}
		}

		for (int i = 0; i < M; i++) {
			int a, b, l;
			a = edges[i * 3], b = edges[i * 3 + 1], l = edges[i * 3 + 2];
			E[a][Esize[a]]=b; E[b][Esize[b]]=a;
			Esize[a]++; Esize[b]++;
			COST[a][b] = l; COST[b][a] = l;
			E_index[a][b] = i; E_index[b][a] = i;
			VM[i][0] = a; VM[i][1] = b;
		}

		for (int i = 0; i < N; i++) {
			pair<int, int>pi;
			do {
				pi = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
			} while (G[pi.first][pi.second]);

			//sort(E[i].begin(), E[i].end());

			Pos[i] = pi;
			G[pi.first][pi.second]++;
		}
		for (int i = 0; i < M; i++) {
			rmq.push(i, dist(Pos[VM[i][0]], Pos[VM[i][1]]) / COST[VM[i][0]][VM[i][1]]);
		}
		rmq.querymax(0);
		rmq.querymin(0);

		//maxPos = Pos;
		memcpy(maxPos,Pos,sizeof(Pos));
		maxScore = mindat[0].first / maxdat[0].first;
		last = tmr.getTime();
	}

	void solve() {
		int l = 0, r = 2*M;
		double tmpMaxScore = 0;
		while (1) {
			if(l==0 && r==2*M)cnt++;
			loop++;

			if (prob()) {
				//rotate2();
				//cerr << "loop " << loop << endl;
				loop = 0;

				if (rx.rand() % 2) {
					//cerr << maxdat[0].first << endl;
					d = 1 + (maxdat[0].first - 1) * (1-tmr.getTime()/LIMIT);
				}
				else {
					//cerr << mindat[0].first << endl;
					d = 1 + (mindat[0].first - 1) * (1 - tmr.getTime() / LIMIT);
				}
				//cerr <<"d "<< d << endl;
			}

			//cerr << tmpMaxScore << " " << cntscore[l + loop] << endl;
			/*if (loop % max(1,(int)((r-l)*(tmr.getTime()/LIMIT))) == 0) {
				double t = tmr.getTime();
				
				if (t < 0.2)rotate();
				else if(t < 0.5)rotate3();
				else rotate();
				tmpMaxScore = mindat[0].first / maxdat[0].first;
				cerr << mindat[0].first / maxdat[0].first << endl;
				cerr << maxScore << endl;
				//cerr << "loop " << loop << endl;
				//cerr << "count " << cnt << endl;
				loop = 0;

				l = nibu(mindat[0].first / maxdat[0].first, cnt);
				r = nibu(maxScore, cnt);

				cerr << l << " " << r << endl;
				if (l == r) {
					l = 0; r = 2 * M;
				}
			}
			else if (l!=0 && tmpMaxScore <= cntscore[l + loop]) {
				for (int i = 0; i < N; i++) {
					G[Pos[i].first][Pos[i].second]--;
				}
				//Pos = maxPos;
				memcpy(Pos, maxPos, sizeof(maxPos));
				for (int i = 0; i < N; i++) {
					G[Pos[i].first][Pos[i].second]++;
				}
				for (int i = 0; i < M; i++) {
					rmq.update(i, max(0.1, dist(Pos[VM[i][0]], Pos[VM[i][1]])) / COST[VM[i][0]][VM[i][1]]);
				}
				cnt = r;
				loop = 0;

				l = 0; r = 2 * M;	
			}*/

			setSPLIT();

			/*if (SPLIT == 2 && cnt % M == 0) {
				rotate();
				cerr << maxScore << endl;
			}*/

			section = make_pair(make_pair(0, 0), SIZE);
			int i = First(pre, prepre);

			pair<int, int>pi = Pos[i];
			pair<int, int>prevPos = Pos[i];
			pair<double, double>PI(1, 0);
			int size = Esize[i];
			for (int j = 0; j < size; j++) {
				int i2 = E[i][j];
				int k = E_index[i][i2];
				rmq.update(k, d);
			}
			while (1) {
				if (tmr.Over()) {
					cerr << "Count " << cnt << endl;
					cerr << "LastUpdate " << last << endl;
					return;
				}
				bool find = false;
				pair<int, int>tmppi;
				pair<double, double>tmpdat;
				int k = 0;

				for (int t = 0; t < SPLIT*SPLIT; t++) {
					tmppi = make_pair(section.first.first + (section.second / SPLIT) * (t / SPLIT) + rx.rand() % (section.second / SPLIT), section.first.second + (section.second / SPLIT) * (t % SPLIT) + rx.rand() % (section.second / SPLIT));
					if (G[tmppi.first][tmppi.second])continue;

					Pos[i] = tmppi;

					tmpdat = make_pair(maxdat[0].first, mindat[0].first);
					for (int j = 0; j < size; j++) {
						int i2 = E[i][j];
						tmpdat.first = max(tmpdat.first, dist(Pos[i], Pos[i2]) / COST[i][i2]);
						tmpdat.second = min(tmpdat.second, dist(Pos[i], Pos[i2]) / COST[i][i2]);
						if (tmpdat.second / tmpdat.first < PI.second / PI.first)break;
					}

					if (tmpdat.second / tmpdat.first > PI.second / PI.first) {
						PI = make_pair(tmpdat.first, tmpdat.second);
						find = true;

						k = 0;
						kouho[k] = tmppi;
						k++;
					}
					else if (tmpdat.second / tmpdat.first == PI.second / PI.first) {
						kouho[k] = tmppi;
						k++;
					}
				}

				if (find) {
					pi = kouho[rx.rand() % k];
					Pos[i] = pi;
				}
				section.first.first += (section.second / SPLIT) * ((pi.first - section.first.first) / (section.second / SPLIT));
				section.first.second += (section.second / SPLIT) * ((pi.second - section.first.second) / (section.second / SPLIT));
				section.second /= SPLIT;
				//SPLIT = min(10, SPLIT * 2);

				if (section.second < SPLIT) {
					if (SPLIT == 10)SPLIT = 7;
					else break;
				}
				double t = tmr.getTime();
				/*if (SPLIT == 2 && section.second == 87)break;
				else if (t < 0.2 && SPLIT == 4 && section.second == 43)break;
				else if (SPLIT == 4 && section.second == 10)break;
				else if (t < 0.4 && SPLIT == 6 && section.second == 19)break;
				else if (t < 0.6 && SPLIT == 8 && section.second == 10)break;*/
				//if (SPLIT == 2 && section.second == 43)break;
				//else if (SPLIT == 4 && section.second == 10)break;
				//else if (SPLIT == 6 && section.second == 19)break;
				//else if (SPLIT == 8 && section.second == 10)break;

				if (find == false)break;
			}
			Pos[i] = pi;

			for (int j = 0; j < size; j++) {
				int i2 = E[i][j];
				int k = E_index[i][i2];
				rmq.update(k, dist(Pos[i], Pos[i2]) / COST[i][i2]);
			}

			G[prevPos.first][prevPos.second]--;
			G[pi.first][pi.second]++;

			if (maxScore < mindat[0].first / maxdat[0].first) {
				maxScore = mindat[0].first / maxdat[0].first;
				last = tmr.getTime();
				//maxPos = Pos;
				memcpy(maxPos, Pos, sizeof(Pos));
				//cerr << "SPLIT:" << SPLIT << endl;
				//cerr << tmr.getTime() / LIMIT << endl;
				//cerr << "MaxScore:" << maxScore << endl;
				loop = 0;
				l = 0, r = 2*M;
			}
			else if(tmpMaxScore < mindat[0].first / maxdat[0].first) {
				tmpMaxScore = mindat[0].first / maxdat[0].first;
			}

			prepre = pre;
			pre = i;
			cntscore[cnt] = max(cntscore[cnt], maxScore);
			if(l!=0)cntscore[l + loop] = max(cntscore[l + loop], tmpMaxScore);
		}
	}

	int nibu(float score, int turn) {
		int l = 0, r = turn-1;
		int m;

		while (l < r) {
			m = (l + r) / 2;
			if (cntscore[m] < score)l = m + 1;
			else r = m;
		}

		return l;
	}

	void setSPLIT() {
		/*double score = mindat[0].first / maxdat[0].first;
		if (score < 0.1)SPLIT = 2;
		else if (score < 0.3)SPLIT = 4;
		else if (score < 0.5)SPLIT = 6;
		else if (score < 0.7)SPLIT = 8;
		else SPLIT = 10;*/
		
		double t = tmr.getTime() / LIMIT;
		bool flag = false;
		if (t > 0.95) {
			static bool first = true;
			SPLIT = 100;
			if (first) {
				flag = true;
				first = false;
			}
		}
		else if (t > 0.7) {
			static bool first = true;
			SPLIT = 10;
			if (first) {
				flag = true;
				first = false;
			}
		}
		else if (t > 0.5) {
			static bool first = true;
			SPLIT = 5;
			if (first) {
				flag = true;
				first = false;
			}
		}
		else if (t > 0.3) {
			static bool first = true;
			SPLIT = 4;
			if (first) {
				flag = true;
				first = false;
			}
		}
		else if (t > 0.1) {
			static bool first = true;
			SPLIT = 4;
			if (first) {
				flag = true;
				first = false;
			}
		}
		else SPLIT = 2;

		if (flag) {
			cerr << "COUNT:" << cnt << endl;
			cerr << tmr.getTime() / LIMIT << endl;
			cerr << "MaxScore:" << maxScore << endl;
			for (int i = 0; i < N; i++) {
				G[Pos[i].first][Pos[i].second]--;
			}
			//Pos = maxPos;
			memcpy(Pos, maxPos, sizeof(maxPos));
			for (int i = 0; i < N; i++) {
				G[Pos[i].first][Pos[i].second]++;
			}
			for (int i = 0; i < M; i++) {
				rmq.update(i, max(0.1, dist(Pos[VM[i][0]], Pos[VM[i][1]])) / COST[VM[i][0]][VM[i][1]]);
			}
			loop = 0;
		}
		
	}

	int First(int& pre, int& prepre) {
		int target = maxdat[0].first/d >= d / mindat[0].first ? maxdat[0].second : mindat[0].second;
		//if (rx.rand() % 2)target = maxdat[0].second;
		//else target = mindat[0].second;

		int next = rx.rand() % 2;
		//if (E[VM[target][next]].size() > E[VM[target][next ^ 1]].size())next ^= 1;
		if (VM[target][next] == pre)next ^= 1;
		else if (VM[target][next] == prepre && VM[target][next ^ 1] != pre)next ^= 1;

		return VM[target][next];
	}

	bool prob() {
		double probability;
		
		double t = tmr.getTime() / LIMIT;
		if (t > 0.95)probability = 0;
		else if (t > 0.7)probability = loop*16.0*1e-4/M;
		else if (t > 0.5)probability = loop*8.0*1e-4/M;
		else if (t > 0.3)probability = loop*4.0*1e-4/M;
		else if (t > 0.1)probability = loop*2.0*1e-4/M;
		else probability = loop*1.0*1e-4/M;
		
		/*
		double score = mindat[0].first / maxdat[0].first;
		if (score < 0.1)probability = loop*4.0*1e-7;
		else if (score < 0.3)probability = loop*16.0*1e-7;
		else if (score < 0.5)probability = loop*32.0*1e-7;
		else if (score < 0.7)probability = loop*64.0*1e-7;
		else probability = 0;
		*/
		return probability > (double)rx.rand() / UINT32_MAX;
	}

	void rotate() {
		//cerr << loop << endl;
		int move = rx.rand() % (4*SIZE - 4) + 1;
		//int move = 1;
		//int i = rx.rand() % N;
		for (int i = 0; i < N; i++) {
			int d = max(abs(Pos[i].first - SIZE / 2), abs(Pos[i].second - SIZE / 2));
			//int step = move*d / (SIZE / 2);
			int step = move;
			int prestep = Step[Pos[i].first][Pos[i].second];
			G[Pos[i].first][Pos[i].second]--;
			Pos[i] = Stepped[d][(prestep + step) % max(1,d*2*4)];
			G[Pos[i].first][Pos[i].second]++;
		}
		for (int i = 0; i < M; i++) {
			rmq.push(i, dist(Pos[VM[i][0]], Pos[VM[i][1]]) / COST[VM[i][0]][VM[i][1]]);
		}
		rmq.querymax(0);
		rmq.querymin(0);
	}
	void rotate3() {
		//cerr << loop << endl;
		int move = rx.rand() % (SIZE - 2) + 1;
		//int move = 7;
		for (int i = 0; i < N; i++) {
			int d = max(abs(Pos[i].first - SIZE / 2), abs(Pos[i].second - SIZE / 2));
			int step = move*d / (SIZE / 2);
			//int step = move;

			int prestep = Step[Pos[i].first][Pos[i].second];
			G[Pos[i].first][Pos[i].second]--;
			Pos[i] = Stepped[d][(prestep + step) % max(1, d * 2 * 4)];
			G[Pos[i].first][Pos[i].second]++;
		}
		for (int i = 0; i < M; i++) {
			rmq.push(i, dist(Pos[VM[i][0]], Pos[VM[i][1]]) / COST[VM[i][0]][VM[i][1]]);
		}
		rmq.querymax(0);
		rmq.querymin(0);
	}
	

	void rotate2() {
		//cerr << loop << endl;
		for (int i = 0; i<N; i++)G[Pos[i].first][Pos[i].second]--;
		int degree = pow(-1, rx.rand() % 2) * (rx.rand() % 10 + 1) / 10.;
		//int degree = pow(-1, rx.rand() % 2) * (rx.rand() % 450 + 1) / 10.;
		double t = tmr.getTime() / LIMIT;
		//if (t > 0.95)degree = 0;
		//else if (t > 0.8)degree = pow(-1, rx.rand() % 2) * (rx.rand() % 10 + 1) / 10.;
		//else if (t > 0.7)degree = pow(-1, rx.rand() % 2) * (rx.rand() % 5 + 1);
		//else if (t > 0.5)degree = pow(-1, rx.rand() % 2) * (rx.rand() % 10 + 1);
		//else if (t > 0.1)degree = pow(-1, rx.rand() % 2) * (rx.rand() % 100 + 1) / 10.;
		//else degree = pow(-1, rx.rand() % 2) * (rx.rand() % 45 + 1);
		for (int i = 0; i < N; i++) {
			int x2 = round((maxPos[i].first - 350)*cos(degree * Pi / 180.0) - (maxPos[i].second - 350)*sin(degree * Pi / 180.0));
			int y2 = round((maxPos[i].first - 350)*sin(degree * Pi / 180.0) + (maxPos[i].second - 350)*cos(degree * Pi / 180.0));
			if (abs(x2) <= 350 && abs(y2) <= 350) {
				Pos[i] = make_pair(x2 + 350, y2 + 350);
			}
			if (G[Pos[i].first][Pos[i].second]) {
				do {
					Pos[i] = make_pair(rx.rand() % SIZE, rx.rand() % SIZE);
				} while (G[Pos[i].first][Pos[i].second]);
			}
			G[Pos[i].first][Pos[i].second]++;
		}
		for (int i = 0; i < M; i++) {
			rmq.push(i, dist(Pos[VM[i][0]], Pos[VM[i][1]]) / COST[VM[i][0]][VM[i][1]]);
		}
		rmq.querymax(0);
		rmq.querymin(0);
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
