// C++11
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <queue>
#include <functional>
#include <cmath>
#include <map>
#include <cstdlib>
#include <tuple>
#include <fstream>
using namespace std;

#define MAX_TURN 2000
#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2500000000ULL
#define SIZE 600
#define LIMIT 9.99
#define INF 1e7
ofstream foo,foo2;
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

struct TROOP {
	int x, y;
	int size;
	int owner;
	double strength;
	int from, to, target;
	int depTime;
	int arrivalTime;
	int time;
	map<pair<int, int>, int>cnt;
};

struct BASE {
	int x, y;
	int owner;
	int growth;
	int size;

	vector<pair<int, int>>attacked;
};

struct TestCase {
	const int PERSON_CAP = 1000;
	const int SIMULATION_TIME = 2000;
	vector<double>powers;
	int speed, NOpp, B;
};
TestCase tc;

class World {
public:
	int curStep = -1;
	vector<TROOP> troops;
	vector<BASE>bases;
	double playerScore = 0;
	int totalUnits;
	vector<int>playerUnits;
	bool simComplete = false;

	void init(int nowStep, vector<BASE>nowBases, vector<TROOP>nowTroops, double nowPlayerScore) {
		curStep = nowStep;
		bases = nowBases;
		for (int i = 0; i < nowTroops.size(); i++) {
			if (nowTroops[i].to == -1)continue;
			else troops.push_back(nowTroops[i]);
		}
		playerScore = nowPlayerScore;
	}

	void updateScore() {
		totalUnits = 0;
		playerUnits.resize(tc.NOpp + 1);
		for (int i = 0; i < tc.B; i++) {
			totalUnits += bases[i].size;
			playerUnits[bases[i].owner] += bases[i].size;
		}
		for (int i = 0; i < troops.size(); i++) {
			totalUnits += troops[i].size;
			playerUnits[troops[i].owner] += troops[i].size;
		}

		if (totalUnits > 0) {
			playerScore += playerUnits[0] * 1.0 / totalUnits;
		}
		if (playerUnits[0] == totalUnits || playerUnits[0] == 0) {
			simComplete = true;
		}
	}

	void updateTroopDepartures(int owner, vector<int>attacks) {
		for (int i = 0; i < attacks.size(); i++) {
			int from = attacks[2 * i];
			int to = attacks[2 * i + 1];

			TROOP t;
			t.owner = owner;
			t.size = bases[from].size / 2;
			t.x = bases[from].x;
			t.y = bases[from].y;
			t.from = from;
			t.to = to;
			t.depTime = curStep;
			int moveT = (int)ceil(sqrt(pow(t.x - bases[to].x, 2) + pow(t.y - bases[to].y, 2)) / tc.speed);
			t.arrivalTime = t.depTime + moveT;
			troops.push_back(t);

			bases[from].size -= t.size;
		}
	}

	void updateTroopArrivals() {
		for (int i = 0; i < troops.size();) {
			if (troops[i].arrivalTime != curStep) {
				i++;
				continue;
			}

			int town = troops[i].owner;
			int tsize = troops[i].size;
			int ttarget = troops[i].to;

			if (town == bases[ttarget].owner) {
				bases[ttarget].size += tsize;
			}
			else {
				double pTroop = tsize * tc.powers[town];
				double pBase = bases[ttarget].size * tc.powers[bases[ttarget].owner];
				if (pBase >= pTroop) {
					bases[ttarget].size = max(0, bases[ttarget].size - (int)ceil(pTroop / tc.powers[bases[ttarget].owner]));
				}
				else {
					bases[ttarget].size = max(0, tsize - (int)ceil(pBase / tc.powers[town]));
					bases[ttarget].owner = town;
				}

				if (bases[ttarget].size > tc.PERSON_CAP) {
					bases[ttarget].size = tc.PERSON_CAP;
				}

				troops.erase(troops.begin() + i);
			}
		}
	}

	void startNewStep() {
		curStep++;

		for (int i = 0; i < tc.B; i++) {
			if (bases[i].size > 0) {
				bases[i].size += bases[i].growth + bases[i].size / 100;
			}
			if (bases[i].size > tc.PERSON_CAP) {
				bases[i].size = tc.PERSON_CAP;
			}
		}

		for (int i = 0; i < troops.size(); i++) {
			if (troops[i].arrivalTime == curStep) {
				troops[i].x = bases[troops[i].to].x;
				troops[i].y = bases[troops[i].to].y;
				continue;
			}

			double pathMoved = (curStep - troops[i].depTime) * 1.0 / (troops[i].arrivalTime - troops[i].depTime);
			double x = bases[troops[i].to].x + (bases[troops[i].to].x - bases[troops[i].to].x) * pathMoved;
			double y = bases[troops[i].to].y + (bases[troops[i].to].y - bases[troops[i].to].y) * pathMoved;
			troops[i].x = (int)x;
			troops[i].y = (int)y;
		}
	}
};

struct RealAI {
	RandomXor rx;
	int attackT;
	double locality;
	int B;
	int owner;
	vector<int>baseX, baseY;

	int init(vector<int>baseLocations, int speed) {
		B = baseLocations.size() / 2;
		baseX.resize(B);
		baseY.resize(B);
		for (int i = 0; i < B; i++) {
			baseX[i] = baseLocations[2 * i];
			baseY[i] = baseLocations[2 * i + 1];
		}
		return 0;
	}

	vector<int>others;
	int getRandomBase(int sourceInd) {
		vector<double>probs(others.size());
		double sp = 0;
		for (int i = 0; i < others.size(); i++) {
			int ind = others[i];
			probs[i] = pow(1.0 / (pow(baseX[sourceInd] - baseX[ind], 2) + pow(baseY[sourceInd] - baseY[ind], 2)), locality);
			sp += probs[i];
		}

		double r = (double)rx.rand() / UINT32_MAX * sp;
		double s = 0;
		for (int i = 0; i < others.size(); i++) {
			s += probs[i];
			if (s >= r)return others[i];
		}
		return others[others.size() - 1];
	}

	vector<int> sendTroops(vector<int>bases, vector<int>troops) {
		others.clear();
		for (int i = 0; i < B; i++) {
			if (bases[2 * i] != owner)others.push_back(i);
		}
		if (others.size() == 0)return vector<int>(0);

		vector<int>att;
		for (int i = 0; i < B; i++) {
			if (bases[2 * i] == owner && bases[2 * i + 1] > attackT) {
				att.push_back(i);
				att.push_back(getRandomBase(i));
			}
		}
		vector<int>ret(att.size());
		for (int i = 0; i < att.size(); i++) {
			ret[i] = att[i];
		}
		return ret;
	}
};
vector<RealAI>realAIs;

struct PlayerAI {
	vector<int> sendTroops(vector<int>bases, vector<int>troops) {
	}
};


struct PLAYER {
	vector<int>own;
	vector<TROOP>trs;

	int sum_T_N;
};

struct GAME {
	vector<PLAYER> Player;
	vector<BASE> Base;
	vector<TROOP>Troop;
};

class AbstractWars {
public:
	RandomXor rx;
	static int t;
	int B_N,SPEED,P_N;
	int exist_P;
	double playerScore;
	bool first_turn,second_turn;
	vector<GAME>G;
	vector<TROOP>Troop;
	vector<int>goingT;
	map<tuple<int, int, int>, vector<pair<int,int>>>T_co;
	vector<vector<vector<pair<int, int>>>>DIR;
	vector<vector<double>>DIST;
	vector<pair<double,int>>strength;
	pair<int,int> max_str, min_str;
	vector<double>kakomare;
	vector<vector<pair<double, int>>>nearby;
	vector<int>attackT;
	// ----------------------------------------------
	int init(vector <int> baseLocations, int speed) {
		first_turn = second_turn = true;
		P_N = 1;
		SPEED = speed;
		B_N = baseLocations.size() / 2;

		tc.B = B_N;
		tc.speed = speed;

		G.resize(MAX_TURN);
		for (int g = 0; g < MAX_TURN; g++) {
			G[g].Base.resize(B_N);
			for (int i = 0; i < B_N; ++i) {
				G[g].Base[i].x = G[g].Base[i].x = baseLocations[2 * i];
				G[g].Base[i].y = G[g].Base[i].y = baseLocations[2 * i + 1];
				G[g].Base[i].growth = 3;
			}
		}

		DIST.resize(B_N);
		for (int i = 0; i < B_N; i++)DIST[i].resize(B_N);
		nearby.resize(B_N);
		for (int i = 0; i < B_N; ++i) {
			for (int j = 0; j < B_N; ++j) {
				//if (i == j)DIST[i][j] = INF;
				DIST[i][j] = dist(G[0].Base[i].x, G[0].Base[i].y, G[0].Base[j].x, G[0].Base[j].y);
				nearby[i].push_back(make_pair(DIST[i][j], j));
			}
			sort(nearby[i].begin(), nearby[i].end());
		}

		DIR.resize(B_N);
		for (int i = 0; i < B_N; i++) {
			DIR[i].resize(B_N);
			for (int j = 0; j < B_N; j++) {
				int time = (int)ceil(DIST[i][j] / SPEED);
				for (int k = 1; k < time; k++) {
					double partMoved = k * 1.0 / time;
					int x = G[0].Base[i].x + (G[0].Base[j].x - G[0].Base[i].x) * partMoved;
					int y = G[0].Base[i].y + (G[0].Base[j].y - G[0].Base[i].y) * partMoved;
					T_co[make_tuple(x, y, k)].push_back(make_pair(i, j));
					DIR[i][j].push_back(make_pair(x,y));
				}
			}
		}
		//for (auto itr = T_co.begin(); itr != T_co.end(); itr++)if (itr->second.size() > 1)cerr << itr->second.size() << endl;

		kakomare.resize(B_N);

		return 0;
	}
	// ----------------------------------------------

	int targetEnemyBase(int sourceInd) {
		int target = -1;
		double mind = INF;
		vector<pair<double, int>>sorted;
		for (int i = 1; i<P_N; ++i) {
			//if (exist_P != 1 && i == priority[P_N - exist_P].second)continue;

			/*
			vector<pair<int, int>>cnt(B_N);
			for (int l = 0; l < B_N; l++)cnt[l].second = l;
			for (int k = 0; k < B_N; k++) {
				if (G[t].Base[k].owner != i) {
					for (int r = 0; r < 100; r++) {
						int ran = getRandomBase(k, G[t].Base[k].owner);
						if (ran != -1)cnt[ran].first++;
					}
				}
			}
			sort(cnt.begin(), cnt.end());
			reverse(cnt.begin(), cnt.end());
			*/

			for (int j = 0; j < G[t].Player[i].own.size(); j++) {
				int ind = G[t].Player[i].own[j];

				if (ind == sourceInd)continue;

				/*
				bool flag5 = false;
				for (int k = 0; k < B_N; k++) {
					if (G[t].Base[k].owner == 0) {
						if (DIST[k][ind] < DIST[sourceInd][ind])flag5=true;
					}
				}
				if (flag5)continue;
				*/
				/*
				double extra = -1000;
				if(i==0){
					int a=0,sum=0;
					for (int k = 0; k < B_N; k++) {
						if (cnt[0].second == ind)a = cnt[0].first;
						sum += cnt[0].first;
					}
					if(sum)extra = -double(a) / sum * 1000;
				}*/

				/*
				int goT = 0;
				int time = (int)ceil(DIST[sourceInd][ind] / SPEED);
				for (int k = 0; k < G[t].Base[ind].attacked.size(); k++){
					if (G[t].Base[ind].attacked[k].first <= time)goT += G[t].Base[ind].attacked[k].second;
				}*/
				/*
				int anri = 0;
				int time = (int)ceil(DIST[sourceInd][ind] / SPEED);
				int tmp = G[t].Base[ind].tr.n;
				int p = G[t].Base[ind].growth;
				//int turn = 0;
				for (int k = 0; k < time; k++) {
					tmp += p + tmp / 100;
					if (tmp >= attackT[G[t].Base[ind].owner])tmp -= tmp / 2;
					//turn++;
				}
				//tmp = min(1000, tmp);


				int e_tr = G[t].Base[ind].tr.n;
				int c = G[t].Base[ind].attacked.size() - 1;
				for (int k = 0; k < time; k++) {
					e_tr += (e_tr > 0 ? 1 : -1)*G[t].Base[ind].growth + e_tr / 100;
					if (e_tr > attackT[G[t].Base[ind].owner])e_tr -= e_tr / 2;
					while(c >= 0 && c < G[t].Base[ind].attacked.size() && G[t].Base[ind].attacked[c].first == k) {
						e_tr += G[t].Base[ind].attacked[c].second;
						c++;
					}
				}
				if (e_tr < -1000)continue;

				int att = 0;
				int help = 0;
				for (int k = 0; k < G[t].Base[ind].attacked.size(); k++) {
					if (G[t].Base[ind].attacked[k].second < 0)help -= G[t].Base[ind].attacked[k].second;
					else att -= G[t].Base[ind].attacked[k].second;
				}
				*/
				//if (goingT[ind] > 3000 - help)continue;
				
				//cerr << -(i != 0) * 1000 << " " << (i == 0 ? 2.0 : 1.0)*DIST[sourceInd][ind] << " " << endl;
				//sorted.push_back(make_pair(-(i != 0) * 1000 + (i == 0 ? 2.0 : 1.0)*DIST[sourceInd][ind], ind));
				sorted.push_back(make_pair(DIST[sourceInd][ind],ind));

				/*
				if (mind > DIST[sourceInd][ind] / G[t].Base[ind].growth) {
					//if (G[t].Base[ind].owner != max_str.first)continue;
					//if (goingT[ind] < 2000) {
					target = ind;
					mind = DIST[sourceInd][ind] / G[t].Base[ind].growth;
					//}
				}
				*/
			}
		}
		sort(sorted.begin(), sorted.end());
		if(sorted.size())target = sorted[0].second;
		/*if ((double)rx.rand() / UINT32_MAX < 0.1) {
			target = sorted[min(1,(int)sorted.size()-1)].second;
		}*/

		return target;
	}

	int waruagaki(int sourceInd) {
		double maxd = 0;
		int target = -1;
		for (int i = 0; i < B_N; i++) {
			if (maxd < DIST[sourceInd][i]) {
				maxd = DIST[sourceInd][i];
				target = i;
			}
		}

		return target;
	}

	// ----------------------------------------------
	vector <int> sendTroops(vector <int> bases, vector <int> troops) {
		if (first_turn) {
			for (int i = 0; i < B_N; ++i)P_N = max(P_N, bases[2 * i] + 1);
			for (int g = 0; g < MAX_TURN; g++)G[g].Player.resize(P_N);

			strength.resize(P_N);
			attackT.resize(P_N, 1000);
			//cerr << "P_N " << P_N << endl;

			tc.NOpp = P_N - 1;
			tc.powers.resize(P_N, 1.0);

			first_turn = false;
		}
		else if (second_turn) {
			for (int i = 0; i < B_N; i++) {
				int growth = bases[2 * i + 1] - G[t - 1].Base[i].size;
				for (int g = 0; g < MAX_TURN; g++)G[g].Base[i].growth = growth;
			}

			second_turn = false;
		}

		begin_proc();

		vector<vector<int>>cnt(P_N);
		for (int i = 0; i < P_N; i++)cnt[i].resize(4);

		vector<int>pt(P_N);
		// compile the list of bases owned by other players
		for (int i = 0; i < B_N; ++i) {
			G[t].Player[bases[2 * i]].own.push_back(i);
			G[t].Base[i].size = bases[2 * i + 1];
			G[t].Base[i].owner = bases[2 * i];

			G[t].Player[bases[2 * i]].sum_T_N += G[t].Base[i].size;
			cnt[bases[2 * i]][G[t].Base[i].growth]++;

			pt[G[t].Base[i].owner] += G[t].Base[i].size;
		}

		int T_N = troops.size() / 4;
		for (int i = 0; i < T_N; i++) {
			pt[troops[4 * i]] += troops[4 * i + 1];
			attackT[troops[4 * i]] = min(attackT[troops[4 * i]], troops[4 * i + 1] * 2);
		}

		bool flagt = true;
		while (flagt) {
			for (int j = 0; j < G[t].Troop.size(); j++) {
				if (j >= T_N) {
					G[t].Troop.erase(G[t].Troop.begin() + j);
					j--;
					continue;
				}
				if (G[t].Troop[j].to == -1) {
					bool flag = true;
					if (G[t].Troop[j].size != troops[4 * j + 1] || G[t].Troop[j].owner != troops[4 * j])flag = false;
					if (T_co[make_tuple(troops[4 * j + 2], troops[4 * j + 3], G[t].Troop[j].time)].size() == 0) {
						flag = false;
					}
					bool flag2 = false;
					for (auto itr = G[t].Troop[j].cnt.begin(); itr != G[t].Troop[j].cnt.end(); itr++) {
						if (DIR[itr->first.first][itr->first.second].size() >= G[t].Troop[j].time) {
							if (DIR[itr->first.first][itr->first.second][G[t].Troop[j].time - 1] == make_pair(troops[4 * j + 2], troops[4 * j + 3])) {
								flag2 = true;
							}
						}
					}
					if (flag2 == false)flag = flag2;

					if (flag == false) {
						G[t].Troop.erase(G[t].Troop.begin() + j);
						j--;
					}
				}
				else {
					bool flag = true;

					if (G[t].Troop[j].size != troops[4 * j + 1] || G[t].Troop[j].owner != troops[4 * j])flag = false;
					if (T_co[make_tuple(troops[4 * j + 2], troops[4 * j + 3], G[t].Troop[j].time)].size() == 0)flag = false;
					if (DIR[G[t].Troop[j].from][G[t].Troop[j].to][G[t].Troop[j].time - 1] != make_pair(troops[4 * j + 2], troops[4 * j + 3]))flag = false;

					if (flag == false) {
						G[t].Troop.erase(G[t].Troop.begin() + j - 1);
						j -= 2;
					}
				}
			}

			if(G[t].Troop.size())for (int j = 0; j < G[t].Troop.size(); j++) {
				if (j == G[t].Troop.size() - 1) {
					for (int k = 0; k <= j; k++) {
						G[t].Troop[k].x = troops[4 * k + 2];
						G[t].Troop[k].y = troops[4 * k + 3];
					}
					for (int k = j + 1; k < T_N; k++) {
						TROOP tr;
						tr.owner = troops[4 * k];
						tr.x = troops[4 * k + 2];
						tr.y = troops[4 * k + 3];
						tr.size = troops[4 * k + 1];
						tr.time = 1;
						tr.strength = 1;
						G[t].Troop.push_back(tr);
					}
					flagt = false;
					break;
				}
			}
			else {
				for (int k = 0; k < T_N; k++) {
					TROOP tr;
					tr.owner = troops[4 * k];
					tr.x = troops[4 * k + 2];
					tr.y = troops[4 * k + 3];
					tr.size = troops[4 * k + 1];
					tr.time = 1;
					tr.strength = 1;
					G[t].Troop.push_back(tr);
				}
				flagt = false;
			}
		}

		for (int i = 0; i < G[t].Troop.size(); i++) {
			if (G[t].Troop[i].owner == 0)continue;
			vector<pair<int, int>>kouho = T_co[make_tuple(G[t].Troop[i].x, G[t].Troop[i].y, G[t].Troop[i].time)];
			//cerr << kouho.size() << endl;
			for (int k = 0; k < kouho.size(); k++) {
				if (G[t].Troop[i].cnt.find(make_pair(kouho[k].first, kouho[k].second)) == G[t].Troop[i].cnt.end()) {
					G[t].Troop[i].cnt[make_pair(kouho[k].first, kouho[k].second)] = 1;
				}
				else G[t].Troop[i].cnt[make_pair(kouho[k].first, kouho[k].second)]++;
			}
			int max_cnt = 0;

			for (auto itr = G[t].Troop[i].cnt.begin(); itr != G[t].Troop[i].cnt.end(); itr++) {
				if (max_cnt < itr->second) {
					//if (G[t - 1].Base[itr->first.second].owner == G[t - 1].Base[itr->first.first].owner)continue;
					max_cnt = itr->second;
					G[t].Troop[i].from = itr->first.first;
					G[t].Troop[i].to = G[t].Troop[i].target = itr->first.second;
				}
				else if (max_cnt == itr->second) {
					G[t].Troop[i].from = -1;
					G[t].Troop[i].to = G[t].Troop[i].target = -1;
				}
			}
		}

		int pt_sum = 0;
		for (int i = 0; i < P_N; i++)pt_sum += pt[i];
		for (int i = 0; i < P_N; i++)strength[i] = make_pair(double(pt[i]) / (pt_sum-pt[i]), i);

		int p_base = 0;
		for (int i = 0; i < B_N; i++)if (G[t].Base[i].owner == 0)p_base++;
		vector<double>minD(B_N,INF);
		for (int i = 0; i < B_N; i++) {
			//if (G[t].Base[i].owner == 0)continue;
			kakomare[i] = 0;
			for (int j = 0; j < B_N; j++) {
				int ind = nearby[i][j].second;
				if (i == ind)continue;
				//if (G[t].Base[ind].owner == 0)break;
				//kakomare[i]++;
				if (G[t].Base[i].owner == 0) {
					if (G[t].Base[ind].owner == 0)kakomare[i] += 1. / (DIST[i][ind] * DIST[i][ind]);
					else kakomare[i] -= 1. / (DIST[i][ind] * DIST[i][ind]);
				}
				else {
					if (G[t].Base[ind].owner == 0)kakomare[i] -= 1. / (DIST[i][ind] * DIST[i][ind]);
					else if(G[t].Base[ind].owner == G[t].Base[i].owner)kakomare[i] += 1. / (DIST[i][ind] * DIST[i][ind]);
				}
				/*if (G[t].Base[j].owner == 0) {
					kakomare[i] += DIST[i][j] * DIST[i][j];
					minD[i] = min(minD[i], DIST[i][j]);
				}*/
			}
			kakomare[i] *= 700 * 500;
			//if(p_base)kakomare[i] /= p_base;
		}

		for (int i = 0; i < G[t].Troop.size(); i++) {
			int from = G[t].Troop[i].from;
			int to = G[t].Troop[i].to;
			if (to == -1)continue;
			
			if (G[t].Troop[i].owner == 0) {
				int time = (int)ceil(DIST[from][to] / SPEED);
				G[t].Base[to].attacked.push_back(make_pair(time - G[t].Troop[i].time, -G[t].Troop[i].size));
				goingT[G[t].Troop[i].target] += G[t].Troop[i].size;
			}
			else {
				int time = (int)ceil(DIST[from][to] / SPEED);
				G[t].Base[to].attacked.push_back(make_pair(time - G[t].Troop[i].time, G[t].Troop[i].size));
			}
		}
		for (int i = 0; i < B_N; i++)sort(G[t].Base[i].attacked.begin(), G[t].Base[i].attacked.end());
		/*
		for (int i = 0; i < B_N; i++) {
			if (G[t].Base[i].owner) {
				vector<double>probs;
				double sp = 0;
				for (int j = 0; j < B_N; j++) {
					if (G[t].Base[j].owner == 0) {
						double prob = pow(1.0 / (pow(G[t].Base[i].x - G[t].Base[j].x, 2) + pow(G[t].Base[i].y - G[t].Base[j].y, 2)));
					}
				}
			}
		}
		*/

		if (G[t].Player[0].own.size() == B_N) {
			// noone to fight!
			end_proc();
			return vector<int>(0);
		}

		vector<pair<double,int>>order(B_N);
		for (int i = 0; i < B_N; i++)order[i] = make_pair(G[t].Base[i].size, i);
		sort(order.begin(),order.end());
		reverse(order.begin(), order.end());
		//for (int i = 0; i < B_N; i++)order.push_back(make_pair((kakomare[i] + minD[i])/G[t].Base[i].growth,i));
		//sort(order.begin(), order.end());
		vector<int> att;
		if (G[t].Player[0].own.size() != 1) {
			for (int i = 0; i < B_N; i++) {
				//int target = order[i].second;
				int source = order[i].second;
				//if (G[t].Base[target].owner == 0)continue;
				if (G[t].Base[source].owner != 0)continue;
				//if (G[t].Base[source].size < 800)continue;
				
				/*
				if (G[t].Base[source].attacked.size()) {
					int time = G[t].Base[source].attacked[0].first;
					int e_tr = G[t].Base[source].attacked[0].second;

					int p_tr = G[t].Base[source].tr.n + G[t].Base[source].growth + G[t].Base[source].tr.n / 100;
					p_tr -= p_tr / 2;
					for (int j = 0; j < time; j++) {
						p_tr += G[t].Base[source].growth + p_tr / 100;
					}

					if (p_tr < e_tr)continue;
				}
				*/

				int target = targetEnemyBase(source);

				/*
				for (int j = B_N-1; j >= 0; j--) {
					int tmp = nearby[source][j].second;
					int time = (int)ceil(DIST[source][tmp] / SPEED);
					for (int k = 0; k < G[t].Base[tmp].attacked.size(); k++) {
						if (time == G[t].Base[tmp].attacked[k].first) {
							target = tmp;
						}
					}
				}
				*/

				if (target == -1)continue;
				/*if (G[t].Base[source].attacked.size()) {
					if (G[t].Base[source].attacked[0].first < 70 - 10 * G[t].Base[source].growth) {
						continue;
					}
				}*/
				//vector<pair<int, int>>eva;
				//for (int j = 0; j < B_N; j++) {
					//int source = nearby[target][j].second;
					//int target = nearby[source][j].second;
					//if (target == source)continue;
					//if (G[t].Base[source].owner != 0)continue;
					//if (G[t].Base[target].owner == 0)continue;

					//if (goingT[target] > 2000)continue;

					/*
					int time = (int)ceil(DIST[source][target] / SPEED);
					int e_tr = G[t].Base[target].tr.n;
					for (int k = 0; k < time; k++) {
						e_tr += G[t].Base[target].growth + e_tr / 100;
					}
					e_tr = min(800, e_tr);
					if (G[t].Base[target].owner == 0)e_tr *= 2;
					eva.push_back(make_pair(DIST[source][target] , target));


					bool flag = false;
					int time = (int)ceil(DIST[source][target] / SPEED);

					int e_tr = G[t].Base[target].tr.n;
					for (int k = 0; k < time; k++) {
						e_tr += G[t].Base[target].growth + e_tr / 100;
					}
					if (e_tr*2 < G[t].Base[source].tr.n / 2 && G[t].Base[target].growth > G[t].Base[source].growth)flag = true;
					*/

					//}

					//sort(eva.begin(), eva.end());
					//if (eva.size()) {
					//int target = eva[0].second;
				/*bool flag3 = false;
				int time = (int)ceil(DIST[source][target] / SPEED);

				bool flag4 = false;
				int e_tr = G[t].Base[target].tr.n;
				int c = G[t].Base[target].attacked.size()-1;
				for (int k = 0; k < time; k++) {
					e_tr += G[t].Base[target].growth + e_tr / 100;
				}
				if (e_tr * 1.2 < G[t].Base[source].tr.n / 2 && kakomare[target]<0 && kakomare[source]<0)flag3 = true;
				*/
				//if (kakomare[target] < 0)flag3 = true;
				/*if (G[t].Base[target].attacked.size()) {
					if (G[t].Base[target].attacked[0].second < 0)continue;
					if (G[t].Base[target].attacked[0].first < (int)ceil(DIST[source][target] / SPEED)) {
						if (G[t].Base[target].attacked[0].second > G[t].Base[target].tr.n) {
							flag3 = true;
						}
					}
				}*/
					if (G[t].Base[source].size > 1000 - G[t].Base[source].growth - 9 - 4) {
						double mind = INF;
						int to = target;
						
						if(G[t].Base[source].size / 2 < 900)for (int k = 0; k < B_N; k++) {
							if (k == to || k == source)continue;
							if (G[t].Base[k].growth < G[t].Base[source].growth && DIST[source][k] < DIST[source][target] && DIST[k][target] < DIST[source][target] && (DIST[source][k] + DIST[k][target]) <= DIST[source][target] * 1.2) {
								if (mind > DIST[source][k]) {
									mind = DIST[source][k];
									to = k;
								}
							}
						}
						

						att.push_back(source);
						att.push_back(to);

						TROOP tr;
						tr.x = G[t].Base[source].x;
						tr.y = G[t].Base[source].y;
						tr.size = G[t].Base[source].size / 2;
						tr.from = source;
						tr.to = to;
						tr.target = target;
						tr.time = 0;
						tr.owner = 0;
						tr.strength = 1;
						tr.depTime = t;
						tr.arrivalTime = t + (int)ceil(DIST[tr.from][tr.to] / tc.speed);
						tr.cnt[make_pair(tr.from, tr.to)] = 1;
						G[t].Troop.push_back(tr);

						goingT[target] += tr.size / 2;
						G[t].Base[source].size -= G[t].Base[source].size / 2;
						G[t].Base[target].attacked.push_back(make_pair((int)ceil(DIST[source][target]/SPEED),-tr.size));
						sort(G[t].Base[target].attacked.begin(),G[t].Base[target].attacked.end());
					}
				//}
			}
		}
		else {
			int source = G[t].Player[0].own[0];

			bool nige = false;
			if (G[t].Base[source].attacked.size()) {
				if (G[t].Base[source].attacked[0].first < 9)nige = true;
			}

			if (nige && G[t].Base[source].size / 2 != 0) {

				att.push_back(source);
				int target = waruagaki(G[t].Player[0].own[0]);
				att.push_back(target);

				TROOP tr;
				tr.x = G[t].Base[source].x;
				tr.y = G[t].Base[source].y;
				tr.size = G[t].Base[source].size / 2;
				tr.from = source;
				tr.to = tr.target = target;
				tr.time = 0;
				tr.owner = 0;
				tr.strength = 1;
				tr.cnt[make_pair(tr.from, tr.to)] = 1;
				G[t].Troop.push_back(tr);

				goingT[target] += tr.size / 2;
				G[t].Base[source].size -= G[t].Base[source].size / 2;
				G[t].Base[target].attacked.push_back(make_pair((int)ceil(DIST[source][target] / SPEED), -tr.size));
				sort(G[t].Base[target].attacked.begin(), G[t].Base[target].attacked.end());
			}
		}

		end_proc();
		return att;
	}

	void begin_proc() {
		exist_P = 0;

		for (int i = 0; i < G[t].Troop.size(); i++) {
			G[t].Troop[i].time++;
		}

		goingT.assign(B_N, 0);
		//goingTroop();
	}

	void end_proc() {
		for (int i = 0; i < G[t].Troop.size(); i++) {
			if(G[t].Troop[i].to!=-1 && G[t].Troop[i].from != -1)if (G[t].Troop[i].time+1 == (int)ceil(DIST[G[t].Troop[i].from][G[t].Troop[i].to] / SPEED)) {
				continue;
			}
			if(t+1<MAX_TURN)G[t + 1].Troop.push_back(G[t].Troop[i]);
		}

		if (t)for (int i = 0; i < B_N; i++) {
			if (G[t].Base[i].owner != G[t - 1].Base[i].owner) {
				foo << "TURN " << t << endl;
				foo << "	Owner " << G[t].Base[i].owner << endl;
				foo << "	Num " << G[t].Base[i].size << endl;
				foo << "	Growth " << G[t].Base[i].growth << endl;
				foo << endl;
			}
		}

		t++;
	}

	inline double dist(int x1,int y1, int x2, int y2) {
		return sqrt(pow(x1 - x2,2) + pow(y1 - y2,2));
	}

	void goingTroop() {
		goingT.assign(B_N, 0);
	}

	double runTest(int nowStep) {
		World world;

		world.init(nowStep, G[nowStep].Base, G[nowStep].Troop, playerScore);

		for (int step = nowStep; step < 2000; step++) {
			world.startNewStep();
			world.updateTroopArrivals();

			vector<int>baseArg(tc.B * 2);
			for (int i = 0; i < tc.B; i++) {
				baseArg[2 * i] = world.bases[i].owner;
				baseArg[2 * i + 1] = world.bases[i].size;
			}

			vector<int>troopsArg(world.troops.size() * 4);
			for (int i = 0; i < world.troops.size(); i++) {
				troopsArg[4 * i] = world.troops[i].owner;
				troopsArg[4 * i + 1] = world.troops[i].size;
				troopsArg[4 * i + 2] = world.troops[i].x;
				troopsArg[4 * i + 3] = world.troops[i].y;
			}

			for (int owner = 0; owner < P_N; owner++) {
				vector<int>attacks = callSendTroops(owner, baseArg, troopsArg);

				world.updateTroopDepartures(owner, attacks);
			}

			world.updateScore();

			if (world.simComplete) {
				world.playerScore += (tc.SIMULATION_TIME - step - 1) * world.playerUnits[0] * 1.0 / world.totalUnits;
				break;
			}
		}

		return world.playerScore;
	}

	vector<int> callSendTroops(int owner, vector<int>bases, vector<int>troops) {
		if (owner > 0)return realAIs[owner - 1].sendTroops(bases, troops);
		else return realAIs[0].sendTroops(bases, troops);
	}
};
int AbstractWars::t;
// -------8<------- end of solution submitted to the website -------8<-------



template<class T> void getVector(vector<T>& v) {
	foo2 << v.size() << endl;
	for (int i = 0; i < v.size(); ++i) {
		cin >> v[i];
		if (i)foo2 << " ";
		foo2 << v[i];
	}
	foo2 << endl;
}

int main() {
	foo.open("change.txt", ios::out);
	foo2.open("test.txt", ios::out);
	AbstractWars aw;
	int N;
	cin >> N;
	vector<int> baseLoc(N);
	getVector(baseLoc);
	int S;
	cin >> S;
	foo2 << S << endl;

	int retInit = aw.init(baseLoc, S);
	cout << retInit << endl;
	cout.flush();

	for (int st = 0; st < MAX_TURN; ++st) {
		int B;
		cin >> B;
		vector<int> bases(B);
		getVector(bases);
		int T;
		cin >> T;
		vector<int> troops(T);
		getVector(troops);

		vector<int> ret = aw.sendTroops(bases, troops);
		cout << ret.size() << endl;
		for (int i = 0; i < (int)ret.size(); ++i) {
			cout << ret[i] << endl;
		}
		cout.flush();
	}
}
