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
#include <random>
#include <fstream>
using namespace std;

#define MAX_TURN 2000
#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2500000000ULL
#define SIZE 600
#define LIMIT 15.0
#define INF 1e7

//ofstream foo, foo2;

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
		end = start = getCycle();
	}

	double getTime() {
		end = getCycle();
		return end - start;
	}

	void setTime(double d) {
		end = getCycle();
		start = end - d;
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
	void setStart() { end = start = getCycle(); }
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


inline double dist(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}


vector<vector<double>>DIST;
vector<vector<pair<double, int>>>nearby;
vector<vector<pair<double, int>>>sortf;

struct TROOP {
	int x, y;
	int size;
	int owner;
	int sourceId, targetId;
	int depTime;
	int arrivalTime;
	int time;

	map<pair<int, int>, int>cnt;
};
struct TROOP2 {
	int x, y;
	int size;
	int owner;
	int sourceId, targetId;
	int depTime;
	int arrivalTime;
};

struct BASE {
	int x, y;
	int size;
	int owner;
	int growth;

	vector<pair<int, int>>attacked;
};
struct BASE2 {
	int x, y;
	int size;
	int owner;
	int growth;
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
	vector<TROOP2>troops;
	vector<BASE2>bases;
	double playerScore = 0;
	int totalUnits;
	int playerUnit;
	bool simComplete = false;

	void init(World& curWorld) {
		curStep = curWorld.curStep;
		bases = curWorld.bases;
		troops = curWorld.troops;
		playerScore = curWorld.playerScore;
	}

	void updateScore() {
		totalUnits = 0;
		playerUnit = 0;
		for (int i = 0; i < tc.B; i++) {
			totalUnits += bases[i].size;
			if(bases[i].owner==0)playerUnit += bases[i].size;
		}
		for (int i = 0; i < troops.size(); i++) {
			totalUnits += troops[i].size;
			if (troops[i].owner == 0)playerUnit += troops[i].size;
		}

		if (totalUnits > 0) {
			playerScore += playerUnit * 1.0 / totalUnits;
		}
		if (playerUnit == totalUnits || playerUnit == 0) {
			simComplete = true;
		}
	}

	void updateTroopDepartures(int owner, vector<int>&attacks) {
		int a_n = attacks.size() / 2;
		for (int i = 0; i < a_n; i++) {
			int sourceId = attacks[2 * i];
			int targetId = attacks[2 * i + 1];

			TROOP2 t;
			t.owner = owner;
			t.size = bases[sourceId].size / 2;
			t.x = bases[sourceId].x;
			t.y = bases[sourceId].y;
			t.sourceId = sourceId;
			t.targetId = targetId;
			t.depTime = curStep;
			int moveT = (int)ceil(dist(t.x, t.y, bases[targetId].x, bases[targetId].y) / tc.speed);
			t.arrivalTime = t.depTime + moveT;
			troops.push_back(t);

			bases[sourceId].size -= t.size;
		}
	}

	void updateTroopArrivals() {
		for (int i = 0; i < troops.size();) {
			if (troops[i].targetId == -1) {
				i++;
				continue;
			}
			if (troops[i].arrivalTime != curStep) {
				i++;
				continue;
			}

			int town = troops[i].owner;
			int tsize = troops[i].size;
			int ttarget = troops[i].targetId;

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
			}

			troops.erase(troops.begin() + i);
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
				troops[i].x = bases[troops[i].targetId].x;
				troops[i].y = bases[troops[i].targetId].y;
				continue;
			}

			double pathMoved = (curStep - troops[i].depTime) * 1.0 / (troops[i].arrivalTime - troops[i].depTime);
			double x = bases[troops[i].targetId].x + (bases[troops[i].targetId].x - bases[troops[i].targetId].x) * pathMoved;
			double y = bases[troops[i].targetId].y + (bases[troops[i].targetId].y - bases[troops[i].targetId].y) * pathMoved;
			troops[i].x = (int)x;
			troops[i].y = (int)y;
		}
	}
};


struct SortedWorld {
	double score;
	World world;

	SortedWorld(double score_, World& world_) {
		score = score_;
		Timer tmr;
		world = world_;
	}

	bool operator < (const SortedWorld& right) const {
		if (score > right.score)return true;
		else return false;
	}
};

class Simulation {
public:
	RandomXor rx;
	int t;
	vector<BASE2>curBases;
	vector<TROOP2>curTroops;
	World curWorld;
	int cnt, width;

	void init(World& world, int cnt_, int width_);

	int getTarget(int sourceInd);
	vector<int> getTargets();
	double getScore(int sourceInd);

	double simulation(World world, vector<int>&attacks);

	double runTest(int curStep, World world);

	double run(World& world);

	vector<int> callEnemySendTroops(int owner, vector<int>&bases, vector<int>&troops);

	double callPlayerSendTroops(int curStep, World& world);
};

struct RealAI {
	RandomXor rx;
	int attackT;
	double locality;
	int owner;
	vector<int>baseX, baseY;

	int init(vector<BASE2>&bases, int own) {
		baseX.resize(tc.B);
		baseY.resize(tc.B);
		for (int i = 0; i < tc.B; i++) {
			baseX[i] = bases[i].x;
			baseY[i] = bases[i].y;
		}

		locality = 1.5;
		attackT = 999;
		owner = own;

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

	vector<int> sendTroops(vector<int>&bases) {
		others.clear();
		for (int i = 0; i < tc.B; i++) {
			if (bases[2 * i] != owner)others.push_back(i);
		}
		if (others.size() == 0)return vector<int>(0);

		vector<int>att;
		for (int i = 0; i < tc.B; i++) {
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
	int owner = 0;
	vector<int>others;

	int t;
	World curWorld;
	void init(int curStep, World world) {
		t = curStep;
		curWorld = world;
	}

	double sendTroops(int cnt, int width) {
		others.clear();
		for (int i = 0; i < tc.B; i++) {
			if (curWorld.bases[i].owner != owner)others.push_back(i);
		}
		if (others.size() == 0)return -1;

		vector<pair<double, int>>order(tc.B);
		for (int i = 0; i < tc.B; i++)order[i] = make_pair(curWorld.bases[i].size, i);
		sort(order.begin(), order.end());
		reverse(order.begin(), order.end());

		vector<int> att;
		double score = -1;
		for (int i = 0; i < tc.B; i++) {
			int source = order[i].second;
			if (curWorld.bases[source].owner != 0)continue;

			if (curWorld.bases[source].size > 1000 - 4) {
				Simulation sim;
				sim.init(curWorld, cnt, width);
				score = sim.getScore(source);
				/*
				if (target == -1)continue;

				int targetId = target;

				att.push_back(source);
				att.push_back(targetId);

				TROOP tr;
				tr.x = curWorld.bases[source].x;
				tr.y = curWorld.bases[source].y;
				tr.size = curWorld.bases[source].size / 2;
				tr.sourceId = source;
				tr.targetId = targetId;
				tr.target = target;
				tr.time = 0;
				tr.owner = 0;
				tr.strength = 1;
				tr.depTime = t;
				tr.arrivalTime = t + (int)ceil(DIST[tr.sourceId][tr.targetId] / tc.speed);
				tr.cnt[make_pair(tr.sourceId, tr.targetId)] = 1;
				sim.curWorld.troops.push_back(tr);

				sim.curWorld.bases[source].size -= sim.curWorld.bases[source].size / 2;
				*/
				break;
			}
		}

		return score;
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
	int P_N;
	double playerScore;
	bool first_turn, second_turn;
	vector<GAME>G;
	map<tuple<int, int, int>, vector<pair<int, int>>>T_co;
	vector<vector<vector<pair<int, int>>>>DIR;
	vector<int>attackT;
	World nowWorld;
	int defWidth;
	Timer tmr;
	bool yabai;
	// ----------------------------------------------
	int init(vector<int>& baseLocations, int speed) {
		tmr.setStart();
#ifndef _DEBUG
		int remain = Server::getRemainingTime(0);
		tmr.setTime(15.0 - remain/1000.);
#endif

		yabai = false;
		first_turn = second_turn = true;
		P_N = 1;

		tc.B = baseLocations.size() / 2;
		tc.speed = speed;

		nowWorld.bases.resize(tc.B);
		for (int i = 0; i < tc.B; ++i) {
			nowWorld.bases[i].x = baseLocations[2 * i];
			nowWorld.bases[i].y = baseLocations[2 * i + 1];
			nowWorld.bases[i].growth = 3;
		}

		G.resize(MAX_TURN);
		for (int g = 0; g < MAX_TURN; g++) {
			G[g].Base.resize(tc.B);
		}

		DIST.resize(tc.B);
		for (int i = 0; i < tc.B; i++)DIST[i].resize(tc.B);
		nearby.resize(tc.B);
		for (int i = 0; i < tc.B; ++i) {
			for (int j = 0; j < tc.B; ++j) {
				DIST[i][j] = dist(nowWorld.bases[i].x, nowWorld.bases[i].y, nowWorld.bases[j].x, nowWorld.bases[j].y);
				nearby[i].push_back(make_pair(DIST[i][j], j));
			}
			sort(nearby[i].begin(), nearby[i].end());
		}

		DIR.resize(tc.B);
		for (int i = 0; i < tc.B; i++) {
			DIR[i].resize(tc.B);
			for (int j = 0; j < tc.B; j++) {
				int time = (int)ceil(DIST[i][j] / tc.speed);
				for (int k = 1; k < time; k++) {
					double partMoved = k * 1.0 / time;
					int x = nowWorld.bases[i].x + (nowWorld.bases[j].x - nowWorld.bases[i].x) * partMoved;
					int y = nowWorld.bases[i].y + (nowWorld.bases[j].y - nowWorld.bases[i].y) * partMoved;
					T_co[make_tuple(x, y, k)].push_back(make_pair(i, j));
					DIR[i][j].push_back(make_pair(x, y));
				}
			}
		}

		//for (auto itr = T_co.begin(); itr != T_co.end(); itr++)if (itr->second.size() > 1)cerr << itr->second.size() << endl;

		sortf.resize(tc.B);
		for (int i = 0; i < tc.B; i++) {
			sortf[i].resize(tc.B);
			for (int j = 0; j < tc.B; j++) {
				sortf[i][j].second = j;
			}
		}

		defWidth = 5;

		return 0;
	}
	// ----------------------------------------------

	int targetEnemyBase(int sourceInd) {
		int target = -1;
		//double mind = INF;
		vector<pair<double, int>>sorted;
		for (int i = 1; i<P_N; ++i) {

			for (int j = 0; j < G[t].Player[i].own.size(); j++) {
				int ind = G[t].Player[i].own[j];

				if (ind == sourceInd)continue;
				sorted.push_back(make_pair(DIST[sourceInd][ind], ind));
			}
		}
		sort(sorted.begin(), sorted.end());
		if (sorted.size())target = sorted[0].second;

		return target;
	}

	int waruagaki(int sourceInd) {
		double maxd = 0;
		int target = -1;
		for (int i = 0; i < tc.B; i++) {
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
			for (int i = 0; i < tc.B; ++i)P_N = max(P_N, bases[2 * i] + 1);
			for (int g = 0; g < MAX_TURN; g++)G[g].Player.resize(P_N);

			attackT.resize(P_N, 999);

			tc.NOpp = P_N - 1;
			tc.powers.resize(P_N, 1.21);
			tc.powers[0] = 1.0;
			realAIs.resize(P_N);
			for (int i = 0; i < P_N; i++)realAIs[i].init(nowWorld.bases, i);

			first_turn = false;
		}
		else if (second_turn) {
			for (int i = 0; i < tc.B; i++) {
				int growth = bases[2 * i + 1] - G[t - 1].Base[i].size;
				nowWorld.bases[i].growth = growth;
			}

			second_turn = false;
		}

		int playerUnit = 0;
		int totalUnits = 0;
		// compile the list of bases owned by other players
		for (int i = 0; i < tc.B; ++i) {
			G[t].Player[bases[2 * i]].own.push_back(i);
			G[t].Base[i].size = bases[2 * i + 1];
			G[t].Base[i].owner = bases[2 * i];

			totalUnits += bases[2 * i + 1];
			if (bases[2 * i] == 0)playerUnit += bases[2 * i + 1];
		}

		begin_proc();

		int T_N = troops.size() / 4;
		for (int i = 0; i < T_N; i++) {
			attackT[troops[4 * i]] = min(attackT[troops[4 * i]], troops[4 * i + 1] * 2);
			realAIs[troops[4 * i]].attackT = min(realAIs[troops[4 * i]].attackT, troops[4 * i + 1] * 2);

			totalUnits += troops[4 * i + 1];
			if (troops[4 * i] == 0)playerUnit += troops[4 * i + 1];
		}

		bool flagt = true;
		while (flagt) {
			for (int j = 0; j < G[t].Troop.size(); j++) {
				if (j >= T_N) {
					G[t].Troop.erase(G[t].Troop.begin() + j);
					j--;
					continue;
				}
				if (G[t].Troop[j].targetId == -1) {
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
					if (DIR[G[t].Troop[j].sourceId][G[t].Troop[j].targetId][G[t].Troop[j].time - 1] != make_pair(troops[4 * j + 2], troops[4 * j + 3]))flag = false;

					if (flag == false) {
						G[t].Troop.erase(G[t].Troop.begin() + j - 1);
						j -= 2;
					}
				}
			}

			if (G[t].Troop.size())for (int j = 0; j < G[t].Troop.size(); j++) {
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
					G[t].Troop[i].sourceId = itr->first.first;
					G[t].Troop[i].targetId = itr->first.second;
				}
				else if (max_cnt == itr->second) {
					G[t].Troop[i].sourceId = -1;
					G[t].Troop[i].targetId = -1;
				}
			}
		}

		for (int i = 0; i < G[t].Troop.size(); i++) {
			int sourceId = G[t].Troop[i].sourceId;
			int targetId = G[t].Troop[i].targetId;
			if (targetId == -1)continue;

			G[t].Troop[i].depTime = t - G[t].Troop[i].time;
			G[t].Troop[i].arrivalTime = G[t].Troop[i].depTime + (int)ceil(DIST[sourceId][targetId] / tc.speed);

			if (G[t].Troop[i].owner == 0) {
				int time = (int)ceil(DIST[sourceId][targetId] / tc.speed);
				G[t].Base[targetId].attacked.push_back(make_pair(time - G[t].Troop[i].time, -G[t].Troop[i].size));
			}
			else {
				int time = (int)ceil(DIST[sourceId][targetId] / tc.speed);
				G[t].Base[targetId].attacked.push_back(make_pair(time - G[t].Troop[i].time, G[t].Troop[i].size));
			}
		}

		for (int i = 0; i < tc.B; i++)sort(G[t].Base[i].attacked.begin(), G[t].Base[i].attacked.end());

		if (G[t].Player[0].own.size() == tc.B) {
			// noone targetId fight!
			end_proc();
			return vector<int>(0);
		}

		vector<double>kakomare(tc.B);
		for (int i = 0; i < tc.B; i++) {
			int cnt = 0;
			//if (G[t].Base[i].owner != 0)continue;
			
			for (int j = 0; j < 5; j++) {
				int ind = nearby[i][j].second;
				if (i == ind)continue;
				if (G[t].Base[ind].owner == 0) {
					cnt++;
				}
			} 
			kakomare[i] = 1 - cnt/100.;
			
			/*
			double mind = INF;
			for (int j = 0; j < tc.B; j++) {
				if (i == j)continue;
				if(G[t].Base[j].owner!=0)mind = min(mind, DIST[i][j]);
			}
			kakomare[i] = 1 - (mind/10 + 1)/1000.;
			*/
		}
		vector<vector<int>>nokori(tc.B);
		for (int i = 0; i < tc.B; i++) {
			nokori[i].resize(tc.B);
			int cnt = 0;
			for (int j = 0; j < tc.B; j++) {
				int ind = nearby[i][j].second;
				if (G[t].Base[ind].owner == 0)continue;
				int time = (int)ceil(DIST[i][ind]/tc.speed);
				int owner = G[t].Base[ind].owner;
				int size = G[t].Base[ind].size;
				for (int k = 0; k < time; k++) {
					if (size > attackT[owner])size -= size / 2;
					size += G[t].Base[ind].growth + size / 100;
				}
				nokori[i][ind] = size;
				cnt++;
				if (cnt == 10)break;
			}
		}

		for (int i = 0; i < tc.B; i++) {
			for (int j = 0; j < tc.B; j++) {
				//cerr << (1 - kakomare[j] / 100.) <<" "<< (1 + attackT[G[t].Base[j].owner] / 10000.) <<" "<< tc.powers[G[t].Base[j].owner] <<" "<< (1 + G[t].Base[j].size / 10000.) <<" "<< (1. - G[t].Base[j].growth / 100.) << endl;
				//sortf[i][j].first = DIST[i][j] * (1 - kakomare[j] / 100.) * (1 + attackT[G[t].Base[j].owner] / 10000.) * (1 + 0.1*(tc.powers[G[t].Base[j].owner]-1)) * (1 + nokori[i][j] / 10000.) * (1. - G[t].Base[j].growth / 100.);
				sortf[i][j].first = DIST[i][j] * kakomare[j] * tc.powers[G[t].Base[j].owner] * (1 + nokori[i][j] / 1000.);
				sortf[i][j].second = j;
			}
			sort(sortf[i].begin(), sortf[i].end());
		}

		nowWorld.curStep = t;
		for (int i = 0; i < tc.B; i++) {
			nowWorld.bases[i].size = G[t].Base[i].size;
			nowWorld.bases[i].owner = G[t].Base[i].owner;
		}
		nowWorld.troops.clear();
		for (int i = 0; i < G[t].Troop.size(); i++) {
			//if (G[t].Troop[i].targetId == -1)continue;
			TROOP2 tr2;
			tr2.x = G[t].Troop[i].x;
			tr2.y = G[t].Troop[i].y;
			tr2.sourceId = G[t].Troop[i].sourceId;
			tr2.targetId = G[t].Troop[i].targetId;
			tr2.arrivalTime = G[t].Troop[i].arrivalTime;
			tr2.depTime = G[t].Troop[i].depTime;
			tr2.owner = G[t].Troop[i].owner;
			tr2.size = G[t].Troop[i].size;
			nowWorld.troops.push_back(tr2);
		}
		nowWorld.updateScore();
		//cerr << "score " << nowWorld.playerScore << endl;

#ifndef _DEBUG
		if (yabai == false && tmr.getTime() > 10.0) {
			int remain = Server::getRemainingTime(0);
			cerr << remain / 1000. << endl;
			cerr << tmr.getTime() << endl;
			tmr.setTime(15.0 - remain/1000.);
			if (tmr.getTime() > 10.0) {
				yabai = true;
			}
		}
#endif
		/*
		if (tmr.getTime() > 10.0) {
			yabai = true;
		}*/
		
		//cerr << tmr.getTime() << endl;
		vector<int> att;
		if ((double)playerUnit / totalUnits >= 0.1) {
			int width = 1, cnt = 2;

			if ((double)playerUnit / totalUnits > 0.9)width *= 1, cnt = 5;
			else if ((double)playerUnit / totalUnits > 0.7)width *= 1, cnt = 5;
			else if ((double)playerUnit / totalUnits > 0.5)width *= 1, cnt = 2;
			else if ((double)playerUnit / totalUnits < 0.1)width *= 0, cnt = 1;
			else if ((double)playerUnit / totalUnits < 0.2)width *= 1, cnt = 1;
			else if ((double)playerUnit / totalUnits < 0.3)width *= 1, cnt = 1;
			else width *= 2, cnt = 1;

			if (yabai || (double)playerUnit / totalUnits > 0.) {
				map<int, vector<int>>target;
				for (int j = 0; j < tc.B; j++) {
					int cnt2 = 0;
					if (G[t].Base[j].owner == 0 && G[t].Base[j].size > 999) {
						for (int j2 = 0; j2 < tc.B; j2++) {
							int ind = sortf[j][j2].second;
							if (G[t].Base[ind].owner != 0) {
								target[j].push_back(ind);
								cnt2++;
							}
							if (cnt2 == cnt)break;
						}
					}
				}

				vector<int>attacks;
				vector<pair<int, int>>st;

				for (auto itr = target.begin(); itr != target.end(); itr++) {
					int ind = rx.rand() % itr->second.size();
					st.push_back(make_pair(itr->first, itr->second[ind]));
				}

				for (int i = 0; i < st.size(); i++) {
					att.push_back(st[i].first);
					att.push_back(st[i].second);
				}
			}
			else {
				Simulation sim;
				sim.init(nowWorld, cnt, width);
				att = sim.getTargets();
			}

			for (int i = 0; i < att.size() / 2; i++) {
				TROOP tr;
				tr.x = nowWorld.bases[att[2 * i]].x;
				tr.y = nowWorld.bases[att[2 * i]].y;
				tr.size = G[t].Base[att[2 * i]].size / 2;
				tr.sourceId = att[2 * i];
				tr.targetId = att[2 * i + 1];
				vector<int>koho;
				koho.push_back(tr.targetId);
				for (int j = 0; j < tc.B; j++) {
					if (j == tr.sourceId)continue;
					if (G[t].Base[j].owner == 0) {
						if (DIST[tr.sourceId][tr.targetId] > DIST[tr.sourceId][j] && DIST[tr.sourceId][tr.targetId] > DIST[j][tr.targetId] && DIST[tr.sourceId][tr.targetId] * 1. > DIST[tr.sourceId][j] + DIST[j][tr.targetId]) {
							koho.push_back(j);
						}
					}
				}
				tr.targetId = koho[rx.rand()%koho.size()];
				att[2 * i + 1] = tr.targetId;
				tr.time = 0;
				tr.owner = 0;
				tr.depTime = t;
				tr.arrivalTime = t + (int)ceil(DIST[tr.sourceId][tr.targetId] / tc.speed);
				tr.cnt[make_pair(tr.sourceId, tr.targetId)] = 1;
				G[t].Troop.push_back(tr);

				G[t].Base[att[2 * i]].size -= G[t].Base[att[2 * i]].size / 2;

				G[t].Base[att[2 * i + 1]].attacked.push_back(make_pair((int)ceil(DIST[att[2 * i]][att[2 * i + 1]] / tc.speed), -tr.size));
				sort(G[t].Base[att[2 * i + 1]].attacked.begin(), G[t].Base[att[2 * i + 1]].attacked.end());
			}
		}
		else {
			for (int i = 0; i < tc.B; i++) {
				if (G[t].Base[i].owner != 0)continue;

				bool nige = false;
				if (G[t].Base[i].attacked.size()) {
					if (G[t].Base[i].attacked[0].second > 0 && G[t].Base[i].attacked[0].first < 9 && G[t].Base[i].size > pow(2, G[t].Base[i].attacked[0].first))nige = true;
				}

				if (nige && G[t].Base[i].size / 2 != 0 || G[t].Base[i].size > 999) {
					att.push_back(i);
					int target = waruagaki(i);
					att.push_back(target);

					TROOP tr;
					tr.x = nowWorld.bases[i].x;
					tr.y = nowWorld.bases[i].y;
					tr.size = G[t].Base[i].size / 2;
					tr.sourceId = i;
					tr.targetId = target;
					tr.time = 0;
					tr.owner = 0;
					tr.cnt[make_pair(tr.sourceId, tr.targetId)] = 1;
					G[t].Troop.push_back(tr);

					G[t].Base[i].size -= G[t].Base[i].size / 2;
					G[t].Base[target].attacked.push_back(make_pair((int)ceil(DIST[i][target] / tc.speed), -tr.size));
					sort(G[t].Base[target].attacked.begin(), G[t].Base[target].attacked.end());
				}
			}
		}

		end_proc();
		return att;
	}

	void begin_proc() {
		for (int i = 0; i < G[t].Troop.size(); i++) {
			G[t].Troop[i].time++;
			if (G[t].Troop[i].targetId != -1 && G[t].Troop[i].sourceId != -1)if (G[t].Troop[i].time == (int)ceil(DIST[G[t].Troop[i].sourceId][G[t].Troop[i].targetId] / tc.speed)) {
				double power;

				if (G[t].Base[G[t].Troop[i].targetId].owner == 0 && G[t].Base[G[t].Troop[i].targetId].owner == G[t - 1].Base[G[t].Troop[i].targetId].owner) {
					power = (G[t - 1].Base[G[t].Troop[i].targetId].size - G[t].Base[G[t].Troop[i].targetId].size) * 1.0 / G[t].Troop[i].size;
					if (power<1.0 || power > 1.2)power = 1.21;
					if (tc.powers[G[t].Troop[i].owner] == 1.21)tc.powers[G[t].Troop[i].owner] = power;
				}
				else if (G[t].Troop[i].owner == 0 && G[t].Base[G[t].Troop[i].targetId].owner == G[t - 1].Base[G[t].Troop[i].targetId].owner) {
					power = G[t].Troop[i].size * 1.0 / (G[t - 1].Base[G[t].Troop[i].targetId].size - G[t].Base[G[t].Troop[i].targetId].size);
					if (power<1.0 || power > 1.2)power = 1.21;
					if (tc.powers[G[t].Base[G[t].Troop[i].targetId].owner] == 1.21)tc.powers[G[t].Base[G[t].Troop[i].targetId].owner] = power;
				}
				else if (G[t - 1].Base[G[t].Troop[i].targetId].owner == 0 && G[t].Base[G[t].Troop[i].targetId].owner != G[t - 1].Base[G[t].Troop[i].targetId].owner) {
					power = G[t - 1].Base[G[t].Troop[i].targetId].size * 1.0 / (G[t].Troop[i].size - G[t].Base[G[t].Troop[i].targetId].size);
					if (power<1.0 || power > 1.2)power = 1.21;
					if (tc.powers[G[t].Troop[i].owner] == 1.21)tc.powers[G[t].Troop[i].owner] = power;
				}
				else if (G[t].Troop[i].owner == 0 && G[t].Base[G[t].Troop[i].targetId].owner != G[t - 1].Base[G[t].Troop[i].targetId].owner) {
					power = (G[t].Troop[i].size * 1.0 - G[t].Base[G[t].Troop[i].targetId].size * 1.0) / G[t - 1].Base[G[t].Troop[i].targetId].size;
					if (power<1.0 || power > 1.2)power = 1.21;
					if (tc.powers[G[t].Base[G[t].Troop[i].targetId].owner] == 1.21)tc.powers[G[t].Base[G[t].Troop[i].targetId].owner] = power;
				}

				G[t].Troop.erase(G[t].Troop.begin() + i);
				i--;
			}
		}
	}

	void end_proc() {
		for (int i = 0; i < G[t].Troop.size(); i++) {
			if (t + 1<MAX_TURN)G[t + 1].Troop.push_back(G[t].Troop[i]);
		}
		/*
		if (t)for (int i = 0; i < tc.B; i++) {
		if (G[t].Base[i].owner != G[t - 1].Base[i].owner) {
		foo << "TURN " << t << endl;
		foo << "	Owner " << G[t].Base[i].owner << endl;
		foo << "	Num " << G[t].Base[i].size << endl;
		foo << "	Growth " << nowWorld.bases[i].growth << endl;
		foo << endl;
		}
		}
		*/
		t++;
	}
};
int AbstractWars::t;


void Simulation::init(World& world, int cnt_, int width_) {
	Simulation::t = world.curStep + 1;
	Simulation::curWorld = world;
	Simulation::cnt = cnt_;
	Simulation::width = width_;

	for (int i = 0; i < Simulation::curWorld.troops.size();) {
		if (Simulation::curWorld.troops[i].targetId == -1) {
			Simulation::curWorld.troops.erase(Simulation::curWorld.troops.begin() + i);
		}
		else i++;
	}
}

vector<int> Simulation::getTargets() {
	vector<pair<double, vector<int>>>search;

	map<int, vector<int>>target;
	bool found = false;
	for (int j = 0; j < tc.B; j++) {
		int cnt = 0;
		if (curWorld.bases[j].owner == 0 && curWorld.bases[j].size > 999) {
			for (int j2 = 0; j2 < tc.B; j2++) {
				int ind = sortf[j][j2].second;
				if (j == ind)continue;
				if (curWorld.bases[ind].owner != 0) {
					target[j].push_back(ind);
					found = true;
					cnt++;
				}
				if (cnt == Simulation::cnt)break;
			}
		}
	}
	if (found == false)return vector<int>();

	for (int d = -1; d < Simulation::width*3; d++) {
		vector<int>attacks;
		vector<pair<int, int>>st;

		vector<int>cnts(tc.B);
		for (auto itr = target.begin(); itr != target.end(); itr++) {
			if (d==-1 || (double)rx.rand() / UINT32_MAX > 2./10.) {
				int ind = rx.rand() % itr->second.size();
				for(int r=0;cnts[ind] >= 2 && r<10;r++)ind = rx.rand() % itr->second.size();
				if (d == -1)ind = 0;
				st.push_back(make_pair(itr->first,itr->second[ind]));
				cnts[ind]++;
			}
		}

		for (int i = 0; i < st.size(); i++) {
			attacks.push_back(st[i].first);
			attacks.push_back(st[i].second);
		}

		double score;
		score = Simulation::simulation(curWorld, attacks);
		//cerr << score << endl;
		search.push_back(make_pair(score, attacks));
	}

	sort(search.begin(), search.end(), greater<pair<double, vector<int>>>());

	if (search.size()) {
		//cerr << search[0].first << endl;
		return search[0].second;
	}
	else return vector<int>();
}

double Simulation::getScore(int sourceInd) {
	vector<pair<double, int>>search;
	//search.push_back(make_pair(simulation(-1,-1),-1));
	for (int j = 0; j < tc.B; j++) {
		int ind = nearby[sourceInd][j].second;
		if (ind == sourceInd)continue;
		if (Simulation::curWorld.bases[ind].owner != 0) {
			World world(Simulation::curWorld);
			vector<int>attacks;
			attacks.push_back(sourceInd); attacks.push_back(ind);
			world.updateTroopDepartures(0, attacks);
			Simulation::init(world, Simulation::cnt, Simulation::width);
			double score;
			score = Simulation::simulation(world, attacks);
			//cerr << score << endl;
			search.push_back(make_pair(score, ind));
			Simulation::cnt++;
		}

		if (Simulation::cnt == Simulation::width)break;
	}
	sort(search.begin(), search.end(), greater<pair<double, int>>());

	return search[0].first;
}

double Simulation::simulation(World world, vector<int>&attacks) {
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
	for (int owner = 1; owner < tc.NOpp + 1; owner++) {
		vector<int>attacks = Simulation::callEnemySendTroops(owner, baseArg, troopsArg);
		world.updateTroopDepartures(owner, attacks);
	}

	world.updateTroopDepartures(0, attacks);

	world.updateScore();

	return run(world);
}

double Simulation::runTest(int curStep, World world) {
	for (int step = curStep; step < min(2000, curStep + 100); step++) {
		//cerr << step << endl;
		if (step != curStep) {
			world.startNewStep();
			world.updateTroopArrivals();
		}

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

		for (int owner = 0; owner < tc.NOpp + 1; owner++) {
			vector<int>attacks;
			if (owner)attacks = Simulation::callEnemySendTroops(owner, baseArg, troopsArg);
			else {
				//double score = Simulation::callPlayerSendTroops(step, world);
				//if (score != -1)return score;
				attacks = Simulation::callEnemySendTroops(owner, baseArg, troopsArg);
			}

			world.updateTroopDepartures(owner, attacks);
		}

		world.updateScore();

		if (world.simComplete) {
			world.playerScore += (tc.SIMULATION_TIME - step - 1) * world.playerUnit * 1.0 / world.totalUnits;
			break;
		}
	}

	return world.playerScore;
}

double Simulation::run(World& tmpworld) {
	vector<vector<SortedWorld>>holdWorld;
	holdWorld.resize(2001);
	holdWorld[Simulation::t].push_back(SortedWorld(tmpworld.playerScore, tmpworld));


	for (int step = Simulation::t; step < min(2000, Simulation::t + 500); step++) {
		shuffle(holdWorld[step].begin(), holdWorld[step].end(), mt19937());
		if ((step - Simulation::t + 1) % 10 == 0) {
			sort(holdWorld[step].begin(), holdWorld[step].end());
			reverse(holdWorld[step].begin(), holdWorld[step].end());
		}
		for (int k = 0; k < min(width + 1, (int)holdWorld[step].size()); k++) {
			World world = holdWorld[step][k].world;

			if (world.simComplete) {
				holdWorld[step + 1].push_back(SortedWorld(world.playerScore, world));
			}

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

			map<int, vector<int>>target;
			bool found = false;
			for (int j = 0; j < tc.B; j++) {
				int cnt = 0;
				if (curWorld.bases[j].owner == 0 && world.bases[j].size > 999) {
					for (int j2 = 0; j2 < tc.B; j2++) {
						int ind = sortf[j][j2].second;
						if (j == ind)continue;
						if (curWorld.bases[ind].owner != 0) {
							target[j].push_back(ind);
							found = true;
							cnt++;
						}
						if (cnt == 1)break;
					}
				}
			}

			for (int owner = 1; owner < tc.NOpp + 1; owner++) {
				vector<int>attacks = Simulation::callEnemySendTroops(owner, baseArg, troopsArg);
				world.updateTroopDepartures(owner, attacks);
			}

			for (int d = -1; d < 0; d++) {
				World newWorld(world);
				vector<int>attacks;
				vector<pair<int, int>>st;

				for (auto itr = target.begin(); itr != target.end(); itr++) {
					if (d==-1 || (double)rx.rand() / UINT32_MAX < curWorld.bases[itr->first].size / 1000.) {
						int ind = rx.rand() % itr->second.size();
						if (d == -1)ind = 0;
						st.push_back(make_pair(itr->first, itr->second[ind]));
					}
				}

				for (int i = 0; i < st.size(); i++) {
					attacks.push_back(st[i].first);
					attacks.push_back(st[i].second);
				}

				newWorld.updateTroopDepartures(0, attacks);

				holdWorld[step + 1].push_back(SortedWorld(newWorld.playerScore, newWorld));

				if (found == false)break;
			}
		}

		//sort(holdWorld[step + 1].begin(), holdWorld[step + 1].end());
		for (int k = 0; k < holdWorld[step + 1].size(); k++) {
			if (holdWorld[step + 1][k].world.simComplete)continue;
			holdWorld[step + 1][k].world.updateScore();
			
			if (holdWorld[step + 1][k].world.simComplete) {
				holdWorld[step + 1][k].world.playerScore += (min(tc.SIMULATION_TIME, Simulation::t + 500) - step - 1) * holdWorld[step + 1][k].world.playerUnit * 1.0 / holdWorld[step + 1][k].world.totalUnits;
				//return holdWorld[step + 1][k].world.playerScore;
			}

			holdWorld[step + 1][k].score = holdWorld[step + 1][k].world.playerScore;
			//holdWorld[step + 1][k].score = runTest(step + 1, holdWorld[step + 1][k].world);
		}

		//cerr << tmr.getTime() << endl;
	}
	sort(holdWorld[min(2000, Simulation::t + 500)].begin(), holdWorld[min(2000, Simulation::t + 500)].end());
	//cerr << tmr.getTime() << endl;
	return holdWorld[min(2000, Simulation::t + 500)][0].score;
}

vector<int> Simulation::callEnemySendTroops(int owner, vector<int>&bases, vector<int>&troops) {
	return realAIs[owner].sendTroops(bases);
}

double Simulation::callPlayerSendTroops(int curStep, World& world) {
	PlayerAI playerAI;
	playerAI.init(curStep, world);
	//Simulation::curWorld = world;
	//Simulation::t = curStep;
	return playerAI.sendTroops(cnt, width);
}
// -------8<------- end of solution submitted targetId the website -------8<-------


template<class T> void getVector(vector<T>& v) {
	//foo2 << v.size() << endl;
	for (int i = 0; i < v.size(); ++i) {
		cin >> v[i];
		//if (i)foo2 << " ";
		//foo2 << v[i];
	}
	//foo2 << endl;
}

int main() {
	//foo.open("change.txt", ios::out);
	//foo2.open("test.txt", ios::out);
	AbstractWars aw;
	int N;
	cin >> N;
	vector<int> baseLoc(N);
	getVector(baseLoc);
	int S;
	cin >> S;
	//foo2 << S << endl;

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
