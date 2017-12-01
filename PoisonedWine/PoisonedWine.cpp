#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <time.h>
#include <tuple>

using namespace std;

class PoisonTest {
public:
	static vector<int> useTestStrips(vector<string> tests) {
		cout << "?" << endl;
		cout << tests.size() << endl;
		for (int i = 0; i < (int)tests.size(); ++i) {
			cout << tests[i] << endl;
		}

		int n;
		cin >> n;

		vector<int> res(n);
		for (int i = 0; i < n; ++i) {
			cin >> res[i];
		}
		return res;
	}
};

// -------8<------- start of solution submitted to the website -----8<-------
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include <chrono>
#include <functional>
#include <queue>
#include <map>

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

	void setLimit(double l) { limit = l; }
	void setStart() { start = getCycle(); }
};


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


struct SegTree {
	int N;
	vector<int>dat;

	SegTree(int n) {
		N = 1;
		while (N < n)N *= 2;
		dat.resize(2 * N - 1);
	}

	void update(int i, int x) {
		i = N + i - 1;
		dat[i] = x;
		while (i > 0) {
			i = (i - 1) / 2;
			dat[i] = merge(dat[i * 2 + 1], dat[i * 2 + 2]);
		}
	}

	int query(int a, int b, int k, int l, int r) {
		if (r <= a || b <= l)return 0;
		if (a <= l && r <= b)return dat[k];
		else {
			int ls = query(a, b, 2 * k + 1, l, (l + r) / 2);
			int rs = query(a, b, 2 * k + 2, (l + r) / 2, r);
			return merge(ls, rs);
		}
	}

	int merge(int l, int r) {
		return l + r;
	}

	int getNum(int i) {
		i = N + i - 1;
		return dat[i];
	}
};


struct STATUS {
	int k, loss, score;
	int minloss;

	STATUS() {}
	STATUS(int k_, int loss_, int score_, int minloss_) {
		k = k_, loss = loss_, score = score_, minloss = minloss_;
	}

	bool operator<(const STATUS& r)const {
		return 1.*score < 1.*r.score;
	}
};


class PoisonedWine {
	vector<bool>Bottles;

	int oB, oS, oR, oP;
	int rB, rS, rP, rR;

	double LIMIT = 55.;

	Timer tmr;
	Xor128 xor128;

	double time;
	bool boost = false;
public:
	vector<int> testWine(int n_Bottles, int n_Strips, int n_Rounds, int n_Poison) {
		rB = oB = n_Bottles;
		rS = oS = n_Strips;
		rR = oR = n_Rounds;
		rP = oP = n_Poison;

		Bottles.resize(n_Bottles);

		vector<int>order(n_Bottles);
		for (int i = 0; i < n_Bottles; i++)order[i] = i;

		int sum = 0;
		for (int i = 1; i <= n_Rounds; i++)sum += i;
		time = 1.*LIMIT / sum;

		vector<int> bad, bad2;
		int tP2 = 0;

		for (int r = n_Rounds; r > 0; r--) {
			shuffle(order);


			int s, n;
			s = min(rS, rB);

			tmr.setStart();
			tmr.setLimit(rR*time);
			n = estimate(rB, rS + (rP - tP2 == 1), rR, rP - tP2);
			//cerr << tmr.getTime() << endl;
			//cerr << (boost ? "boosted" : "searched") << endl;
			if (boost) {
				n = easySolve(rB, rS + (rP - tP2 == 1), rR, rP - tP2);
			}



			vector<vector<int>>chosen(s);
			vector<string> test(s);

			int index = 0;
			for (int i = 0; i < s; i++) {
				for (int k = 0; k < n && index < n_Bottles; index++) {
					if (Bottles[order[index]])continue;

					chosen[i].push_back(order[index]);
					k++;
				}

				test[i] = testsInt_to_testsString(chosen[i]);
			}

			vector<int> testRes = PoisonTest::useTestStrips(test);


			vector<int>tmpbad;
			int tP = 0;
			for (int i = 0; i < (int)testRes.size(); ++i) {
				if (testRes[i] == 1) {
					if (chosen[i].size() == 1) {
						Bottles[chosen[i][0]] = true;
						bad.push_back(chosen[i][0]);
						rP--;
						rB--;
					}
					else {
						if (rP - tP2 != 1 && 1.*(rP - tP2) / rB > 1.*(rP - tP2 - 1) / (rB - tmpbad.size())) {
							for (int j = 0; j < (int)chosen[i].size(); j++) {
								Bottles[chosen[i][j]] = true;
								bad2.push_back(chosen[i][j]);
							}
							tP2++;
							rB -= chosen[i].size();
						}
						else {
							for (int j = 0; j < (int)chosen[i].size(); j++) {
								tmpbad.push_back(chosen[i][j]);
							}
							tP++;
						}

					}

					rS--;
				}
				else {
					for (int j = 0; j < (int)chosen[i].size(); j++) {
						Bottles[chosen[i][j]] = true;
					}
					rB -= chosen[i].size();
				}
			}


			bool flag = false;
			if (rP == 0)flag = true;
			else if (r == 1 && rP == tP)flag = true;
			else if (r == 1 || rS == 0 || rB == rP) {
				tmpbad.clear();
				for (int i = 0; i < n_Bottles; i++) {
					if (Bottles[i] == false)tmpbad.push_back(i);
				}
				flag = true;
			}

			if (flag) {
				int size = (int)tmpbad.size();

				for (int i = 0; i < size; i++) {
					bad.push_back(tmpbad[i]);
				}
				for (int i = 0; i < bad2.size(); i++) {
					bad.push_back(bad2[i]);
				}

				return bad;
			}



			if (rP == tP) {
				for (int i = index; i < n_Bottles; i++) {
					if (Bottles[order[i]] == false) {
						Bottles[order[i]] = true;
						rB--;
					}
				}
			}

			if (rP == tP + tP2) {
				for (int i = index; i < n_Bottles; i++) {
					if (Bottles[order[i]] == false) {
						Bottles[order[i]] = true;
						rB--;
					}
				}

				for (int i = 0; i < bad2.size(); i++) {
					Bottles[bad2[i]] = false;
				}
				rB += bad2.size();
				bad2.clear();
				tP2 = 0;
			}
			else if (rP - tP2 >= rB) {
				for (int i = 0; i < bad2.size(); i++) {
					Bottles[bad2[i]] = false;
				}
				rB += bad2.size();
				bad2.clear();
				tP2 = 0;
			}

			rR--;
		}
	}


	int easySolve(int n_Bottles, int n_Strips, int n_Rounds, int n_Poison) {
		int s = min(n_Strips, n_Bottles), n = 1;
		double E;
		double P = 1.*n_Poison / n_Bottles;
		for (int k = 1; s*k <= n_Bottles; k++) {
			E = 1.*s*k * P;
			if (E / s > (0.8 - 0.07*n_Rounds))break;
			n = k;
		}
		return n;
	}


	double calP(int n_Bottles, int n_Poison, int k) {
		if (k == 0)return 1.;
		return 1.*(n_Bottles - (k - 1) - n_Poison) / (n_Bottles - k - 1) * calP(n_Bottles, n_Poison, k - 1);
	}


	int estimate(int n_Bottles, int n_Strips, int n_Rounds, int n_Poison) {
		boost = true;
		int loop = 0;
		int ans = 0;
		while (1) {
			int n = search(n_Bottles, n_Strips, n_Rounds, n_Poison);
			ans += n;
			loop++;

			if (tmr.Over())break;

			boost = false;
		}
		if (loop <= 10)boost = true;

		return round(1.*ans / loop);
	}


	int search(int n_Bottles, int n_Strips, int n_Rounds, int n_Poison) {
		priority_queue<STATUS>Stats[11];
		Stats[n_Rounds].push(STATUS(0, 0, 0, 0));

		vector<vector<vector<int>>>covered(10001, vector<vector<int>>(21, vector<int>(11, -1)));

		int t = min(n_Bottles, n_Strips);

		vector<pair<int, int>>lists(n_Bottles / t);
		for (int i = 0; i < lists.size(); i++) {
			lists[i].second = i + 1;
		}

		int loop = 0;
		while (1) {
			if (tmr.Over()) {
				break;
			}

			loop++;

			bool owari = true;
			for (int r = n_Rounds; r >= 0; r--) {
				if (Stats[r].empty())continue;

				owari = false;
				STATUS st = Stats[r].top(); Stats[r].pop();

				if (covered[st.k][st.loss][r] >= st.score)continue;
				covered[st.k][st.loss][r] = st.score;


				int tB, tS;
				tB = n_Bottles - st.score;
				tS = n_Strips - st.loss;


				if (r == 0) {
					lists[st.k - 1].first = max(lists[st.k - 1].first, st.score);
					continue;
				}
				else if (tS == 0 || tB <= n_Poison)continue;


				int s = min(tB, tS);

				SegTree seg(tB);
				int remain = n_Poison;
				while (remain) {
					int x = xor128.rand() % tB;
					if (seg.getNum(x))continue;
					seg.update(x, 1);
					remain--;
				}

				vector<int>kB(tB / s);

				for (int k = 1; s*k <= tB; k++) {
					double P = calP(n_Bottles, n_Poison, k);

					if (P > 0.8) {
						lists[k - 1].first = 1;
						continue;
					}
					else if (P < 1.*r / (s + 1))break;

					int found = 0;
					for (int i = 0; i < s; i++) {
						if (seg.query(i*k, (i + 1)*k, 0, 0, seg.N))found++;
					}

					kB[k - 1] = found;
				}


				vector<bool>used(s + 1);
				for (int i = kB.size() - 1; i >= 0; i--) {
					if (kB[i] <= st.minloss)continue;

					if (used[kB[i]])continue;
					used[kB[i]] = true;

					if (r == n_Rounds)Stats[r - 1].push(STATUS(i + 1, kB[i], (s - kB[i])*(i + 1), kB[i]));
					else Stats[r - 1].push(STATUS(st.k, st.loss + kB[i], st.score + (s - kB[i])*(i + 1), kB[i]));
				}
			}

			if (owari) break;
		}

		sort(lists.begin(), lists.end(), greater<pair<double, int>>());

		return lists[0].second;
	}


	string testsInt_to_testsString(vector<int>&num) {
		string test = "";
		for (int i = 0; i < (int)num.size(); i++) {
			if (i)test += ",";
			test += to_string(num[i]);
		}

		return test;
	}


	void shuffle(vector<int>& vec) {
		for (int i = 0; i < vec.size(); i++) {
			int j = xor128.rand() % vec.size();
			swap(vec[i], vec[j]);
		}
	}
};
// -------8<------- end of solution submitted to the website -------8<-------

int main() {
	PoisonedWine pw;
	int numBottles, testStrips, testRounds, numPoison;
	cin >> numBottles >> testStrips >> testRounds >> numPoison;

	vector<int> ret = pw.testWine(numBottles, testStrips, testRounds, numPoison);
	cout << ret.size() << endl;
	for (int i = 0; i < (int)ret.size(); ++i) {
		cout << ret[i] << endl;
	}
}