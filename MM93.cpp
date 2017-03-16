// MM93.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

// C++11
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <map>
#include <chrono>
#include <time.h>
#define all(a) a.begin(),a.end()
#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2500000000ULL
#define BATCH 30

using namespace std;

#ifdef _DEBUG
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#define LIMIT 3000
#else
#define LIMIT 9.7
#endif

int R[] = { 0,0,1,1 };
int C[] = { 0,1,1,0 };

int dist(int a, int b, int S) {
	int ay = a / S, ax = a%S;
	int by = b / S, bx = b%S;

	return (ax - bx)*(ax - bx) + (ay - by)*(ay - by);
}

int dist2(int ar, int ac, int br, int bc) {
	return (ar - br)*(ar - br) + (ac - bc)*(ac - bc);
}

int dist3(int a, int b, int S) {
	int r = a / S, c = a%S;
	int nr = b / S, nc = b%S;
	return dist2(r > nr ? nr + 1 : nr, c > nc ? nc + 1 : nc, nr > r ? r + 1 : r, nc > c ? c + 1 : c);
}

int posR(int r, int n){
	return r + R[n / 2];
}
int posC(int c, int n) {
	return c + C[n / 2];
}

int sum_Dist(vector<int>& pos, vector<int>&order, int S) {
	int sum = 0;
	for (int i = 1;i < (int)order.size();i++) {
		sum += dist(pos[order[i]], pos[order[i - 1]], S);
	}

	return sum;
}

unsigned int randxor()
{
	static unsigned int x = rand(), y = 362436069, z = 521288629, w = 88675123;
	unsigned int t;
	t = (x ^ (x << 11));x = y;y = z;z = w;
	return(w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

class Timer{
	double start, end;
	double limit;

	double getTime() {
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
		start = getTime();
	}

	void setTime() {
		end = getTime();
	}
	double passTime() {
		return end - start;
	}

	bool overTime() {
		if (passTime() > limit) {
			cerr << "LIMIT:" << limit << "s" << " " << "TIME:" << end - start << "s" << endl;
			cerr << "OVER:" << end - start - limit << "s" << endl;
			return true;
		}
		return false;
	}

	void setLimit(double l) {
		limit = l;
	}
};

class CrossStitch {
public:
	vector<string> embroider(vector<string> pattern) {
		vector<string> ret;
		int S = pattern.size();

		srand((unsigned)time(NULL));

		vector<double>limit(26);
		vector<vector<int>>Pos(26);
		int n_alp = 0;
		int n_alp2 = 0;
		for (int r = 0;r < S;++r) {
			for (int c = 0;c < S;++c) {
				char alp = pattern[r][c];
				if (alp != '.') {
					Pos[alp - 'a'].push_back(r*S + c);
					n_alp++;
				}
			}
		}
		for (int i = 0;i < 26;i++)n_alp2 += Pos[i].size()*Pos[i].size();
		for (int i = 0;i < 26;i++) {
			limit[i] = (double)Pos[i].size()*Pos[i].size() * LIMIT / n_alp2;
		}

		double passedTIME = 0;
		// for each color, for each cell (r, c) do two stitches (r+1, c)-(r, c+1)-(r+1, c+1)-(r, c)
		for (char col = 'a'; col <= 'z'; ++col) {
			int n = col - 'a';
			if (Pos[n].size() == 0)continue;
			cerr << "SIZE:" << Pos[n].size() << endl;

			Timer tmr;
			tmr.setLimit(limit[n]);

			vector<int>order;
			vector<vector<pair<int, int>>>Dist(Pos[n].size());

			for (int i = 0;i < (int)Pos[n].size();i++) {
				int pos;
				if (i) pos = Dist[i - 1][0].second;
				else pos = randxor() % (Pos[n].size());

				order.push_back(pos);

				for (int j = (i!=0);j < Pos[n].size() - i;j++) {
					int ij_dist;
					if (i) {
						ij_dist = dist(Pos[n][pos], Pos[n][Dist[i - 1][j].second], S);
						Dist[i].push_back(make_pair(ij_dist, Dist[i - 1][j].second));
					}
					else {
						if (pos == j)continue;
						ij_dist = dist(Pos[n][pos], Pos[n][j], S);
						Dist[i].push_back(make_pair(ij_dist, j));
					}
				}
				sort(all(Dist[i]));
			}

			ret.push_back(string(1, col));

			tmr.setTime();
			cerr << "input:" << tmr.passTime() << "s" << endl;

			bool looping = Pos[n].size() > 1;
			int loop = 0;
			int maxscore, score;
			maxscore = score = sum_Dist(Pos[n], order, S);
			vector<int>res(order);
			while (looping) {
				for(int i=0;looping && i<BATCH;i++){
					if (loop){
						tmr.setTime();
						if (tmr.overTime()) {
							looping = false;
							break;
						}
					}

					int r1 = randxor() % order.size();

					int minusDist = 0, plusDist = 0;
					if (r1 > 0)minusDist += dist(Pos[n][order[r1]], Pos[n][order[r1 - 1]], S);
					if (r1 < (int)order.size() - 1) minusDist += dist(Pos[n][order[r1]], Pos[n][order[r1 + 1]], S);
					if (r1 > 0 && r1 < (int)order.size() - 1)plusDist += dist(Pos[n][order[r1 - 1]], Pos[n][order[r1 + 1]], S);

					int order_r1 = order[r1], index = r1;
					order.erase(order.begin() + r1);
					
					vector<pair<int, int>>ins;

					for(int j =0;j<BATCH;j++){
						int r2 = randxor() % (order.size()+1);

						int mDist = 0, pDist = 0;
						if (r2 > 0)pDist += dist(Pos[n][order_r1], Pos[n][order[r2 - 1]], S);
						if (r2 < (int)order.size()) pDist += dist(Pos[n][order_r1], Pos[n][order[r2]], S);
						if (r2 > 0 && r2 < (int)order.size())mDist += dist(Pos[n][order[r2 - 1]], Pos[n][order[r2]], S);

						ins.push_back(make_pair(pDist - mDist, r2));
					}

					sort(all(ins));

					double s = min(20,score + plusDist - minusDist + ins[0].first - maxscore);
					double e = 1.0 / exp(s);
					bool change = (limit[n] - tmr.passTime()) / limit[n] * e > (double)randxor() / UINT32_MAX;

					if (plusDist - minusDist + ins[0].first < 0 || change) {
						order.insert(order.begin() + ins[0].second, order_r1);

						score += plusDist - minusDist + ins[0].first;

						if (ins[0].second < index)i++;
					}
					else {
						order.insert(order.begin() + index, order_r1);
					}

					if (maxscore > score) {
						maxscore = score;
						res = order;
					}
				}
				
				for(int i=0;looping && i<BATCH;i++){
					if (loop) {
						tmr.setTime();
						if (tmr.overTime()) {
							looping = false;
							break;
						}
					}

					int minusDist = 0, plusDist = 0;
					vector<int>move;
					pair<int, int>lr;

					int r1 = randxor() % order.size(), r2 = randxor() % order.size();
					if (r1 > r2)swap(r1,r2);
					if (r1 > 0)minusDist += dist3(Pos[n][order[r1 - 1]], Pos[n][order[r1]], S);
					if (r2 < (int)order.size() - 1) minusDist += dist3(Pos[n][order[r2]], Pos[n][order[r2 + 1]], S);
					lr = make_pair(r1, r2);

					if (lr.first > 0 && lr.second < (int)order.size() - 1) {
						plusDist += dist3(Pos[n][order[lr.first - 1]], Pos[n][order[lr.second + 1]], S);
					}

					for (int j = lr.second;j >= lr.first;j--) {
						move.push_back(order[j]);
						order.erase(order.begin() + j);
					}

					
					vector<pair<int, int>>ins;

					for(int j=0;j<BATCH;j++){
						int r3 = randxor() % (order.size() + 1);

						int mDist = 0, pDist = 0;
						if (r3 > 0) pDist += dist3(Pos[n][move[move.size() - 1]], Pos[n][order[r3 - 1]], S);
						if (r3 < (int)order.size())pDist += dist3(Pos[n][move[0]], Pos[n][order[r3]], S);
						if (r3 > 0 && r3 < (int)order.size())mDist += dist3(Pos[n][order[r3 - 1]], Pos[n][order[r3]], S);
						ins.push_back(make_pair(pDist - mDist, r3));
					}
					for (int j = 0;j < BATCH;j++) {
						int r3 = randxor() % (order.size() + 1);

						int mDist = 0, pDist = 0;
						if (r3 > 0) pDist += dist3(Pos[n][move[0]], Pos[n][order[r3 - 1]], S);
						if (r3 < (int)order.size())pDist += dist3(Pos[n][move[move.size() - 1]], Pos[n][order[r3]], S);
						if (r3 > 0 && r3 < (int)order.size())mDist += dist3(Pos[n][order[r3 - 1]], Pos[n][order[r3]], S);
						ins.push_back(make_pair(pDist - mDist, -r3-1));
					}

					sort(all(ins));

					double s = min(20, score + plusDist - minusDist + ins[0].first - maxscore);
					double e = 1.0 / exp(s);
					bool change = (limit[n] - tmr.passTime()) / limit[n] * e > (double)randxor() / UINT32_MAX;

					if (plusDist - minusDist + ins[0].first < 0 || change) {
						if (ins[0].second >= 0) {
							for (int j = 0;j < move.size();j++) {
								order.insert(order.begin() + ins[0].second, move[j]);
							}
						}
						else {
							ins[0].second = -(ins[0].second + 1);
							for (int j = move.size()-1;j >= 0;j--) {
								order.insert(order.begin() + ins[0].second, move[j]);
							}
						}

						score += plusDist - minusDist + ins[0].first;
					}
					else {
						for (int j = 0;j < move.size();j++) {
							order.insert(order.begin() + lr.first, move[j]);
						}
					}

					if (maxscore > score) {
						maxscore = score;
						res = order;
					}
				}
				
				for (int i = 0;looping && i < BATCH;i++) {
					if (loop) {
						tmr.setTime();
						if (tmr.overTime()) {
							looping = false;
							break;
						}
					}

					int r1, r2;
					r1 = randxor() % order.size(), r2 = randxor() % order.size();

					int m1 = 0, p1 = 0, m2 = 0, p2 = 0;
					if (r1 > 0 && r1 - 1 != r2) {
						m1 += dist3(Pos[n][order[r1]], Pos[n][order[r1 - 1]], S);
						p2 += dist3(Pos[n][order[r2]], Pos[n][order[r1 - 1]], S);
					}
					if (r1 < (int)order.size() - 1 && r1 + 1 != r2) {
						m1 += dist3(Pos[n][order[r1]], Pos[n][order[r1 + 1]], S);
						p2 += dist3(Pos[n][order[r2]], Pos[n][order[r1 + 1]], S);
					}
					if (r2 > 0 && r2 - 1 != r1) {
						p1 += dist3(Pos[n][order[r1]], Pos[n][order[r2 - 1]], S);
						m2 += dist3(Pos[n][order[r2]], Pos[n][order[r2 - 1]], S);
					}
					if (r2 < (int)order.size() - 1 && r2 + 1 != r1) {
						p1 += dist3(Pos[n][order[r1]], Pos[n][order[r2 + 1]], S);
						m2 += dist3(Pos[n][order[r2]], Pos[n][order[r2 + 1]], S);
					}

					double s = min(20,score + p1 - m1 + p2 - m2 - maxscore);
					double e = 1.0 / exp(s);
					bool change = (limit[n] - tmr.passTime()) / limit[n] * e >(double)randxor() / UINT32_MAX;

					if (p1 + p2 - m1 - m2 < 0 || change) {
						swap(order[r1], order[r2]);

						score += p1 + p2 - m1 - m2;
					}

					if (maxscore > score) {
						maxscore = score;
						res = order;
					}
				}
				
				if(looping)loop++;
			}
			order = res;
			cerr << char('a' + n) << " loop:" << loop << "times" << endl;

			Timer push;

			vector<int>from(order.size());
			vector<int>to(order.size());
			vector<pair<int, int>>pndists(order.size());
			pndists[0].first = -1, pndists[order.size() - 1].second = -1;
			from[0] = -1, to[order.size() - 1] = -1;

			for (int i = (int)order.size() - 1;i >= 0;i--) {
				int r = Pos[n][order[i]] / S, c = Pos[n][order[i]] % S;

				if (i < order.size() - 1) {
					int nr = Pos[n][order[i + 1]] / S, nc = Pos[n][order[i + 1]] % S;
					int ndist = dist2(r > nr ? nr + 1 : nr, c > nc ? nc + 1 : nc, nr > r ? r + 1 : r, nc > c ? c + 1 : c);
					pndists[i].second = ndist;

					if (r > nr && c > nc) {
						to[i] = 0;
					}
					else if (r > nr && c == nc) {
						if (from[i + 1] % 2 == 0) {
							if (ndist == 0)to[i] = from[i + 1] % 4;
							else to[i] = (from[i + 1] + 2) % 4;
						}
						else to[i] = 1;
					}
					else if (r > nr && c < nc) {
						to[i] = 2;
					}
					else if (r == nr && c < nc) {
						if (from[i + 1] % 2 == 0) {
							if (ndist == 0)to[i] = (from[i + 1] + 2) % 4 + 2;
							else to[i] = from[i + 1] % 4 + 2;
						}
						else to[i] = 3;
					}
					else if (r < nr && c < nc) {
						to[i] = 4;
					}
					else if (r < nr && c == nc) {
						if (from[i + 1] % 2 == 0) {
							if (ndist == 0)to[i] = from[i + 1] % 4 + 4;
							else to[i] = (from[i + 1] + 2) % 4 + 4;
						}
						else to[i] = 5;
					}
					else if (r < nr && c > nc) {
						to[i] = 6;
					}
					else if (r == nr && c > nc) {
						if (from[i + 1] % 2 == 0) {
							if (ndist == 0)to[i] = ((from[i + 1] + 2) % 4 + 6) % 8;
							else to[i] = (from[i + 1] % 4 + 6) % 8;
						}
						else to[i] = 7;
					}
				}

				if (i > 0) {
					int pr = Pos[n][order[i - 1]] / S, pc = Pos[n][order[i - 1]] % S;
					int pdist = dist2(r > pr ? pr + 1 : pr, c > pc ? pc + 1 : pc, pr > r ? r + 1 : r, pc > c ? c + 1 : c);
					pndists[i].first = pdist;

					if (r > pr && c > pc) {
						from[i] = 0;
					}
					else if (r > pr && c == pc) {
						if (to[i] % 2 == 0) from[i] = (to[i] + 2) % 4;
						else from[i] = 1;
					}
					else if (r > pr && c < pc) {
						from[i] = 2;
					}
					else if (r == pr && c < pc) {
						if (to[i] % 2 == 0) from[i] = to[i] % 4 + 2;
						else from[i] = 3;
					}
					else if (r < pr && c < pc) {
						from[i] = 4;
					}
					else if (r < pr && c == pc) {
						if (to[i] % 2 == 0) from[i] = (to[i] + 2) % 4 + 4;
						else from[i] = 5;
					}
					else if (r < pr && c > pc) {
						from[i] = 6;
					}
					else if (r == pr && c > pc) {
						if (to[i] % 2 == 0) from[i] = (to[i] % 4 + 6) % 8;
						else from[i] = 7;
					}
				}
			}

			if (pndists[0].second != -1) {
				int r = Pos[n][order[0]] / S, c = Pos[n][order[0]] % S;
				int nr = Pos[n][order[1]] / S, nc = Pos[n][order[1]] % S;
				if (pndists[0].second == 0) {
					if (from[1] % 2) {
						from[1] = (from[1] - 1) % 8;
					}
					if (r + R[((from[1] + 2) % 8) / 2] == nr + R[from[1] / 2] && c + C[((from[1] + 2) % 8) / 2] == nc + C[from[1] / 2]) to[0] = (from[1] - 2 + 8) % 8;
					else to[0] = (from[1] + 2) % 8;
					pndists[0].second = 1, pndists[1].first = 1;
				}
				else if (from[1] % 2) {
					from[1] = (from[1] - 1) % 8;
					if (r + R[((from[1] + 2) % 8) / 2] == nr + R[from[1] / 2] && c + C[((from[1] + 2) % 8) / 2] == nc + C[from[1] / 2]) to[0] = (from[1] + 2) % 8;
					else to[0] = (from[1] - 2 + 8) % 8;
				}
			}
			else {
				from[0] = 0, to[0] = 2;
			}

			for (int i = 1;i < (int)order.size() - 1;i++) {
				int r = Pos[n][order[i]] / S, c = Pos[n][order[i]] % S;
				int pr = Pos[n][order[i - 1]] / S, pc = Pos[n][order[i - 1]] % S;
				int nr = Pos[n][order[i + 1]] / S, nc = Pos[n][order[i + 1]] % S;

				if (to[i] % 2) {
					to[i] = ((from[i] + 2 * (to[i] % 4 == 1)) % 4 + (to[i] - to[i] % 2)) % 8;
				}
				if (from[i + 1] % 2) {
					if (pndists[i].second) from[i + 1] = ((to[i] + 2 * (from[i + 1] % 4 == 1)) % 4 + (from[i + 1] - from[i + 1] % 2)) % 8;
					else from[i + 1] = ((to[i] + 2 * (from[i + 1] % 4 == 3)) % 4 + (from[i + 1] - from[i + 1] % 2)) % 8;

					to[i + 1] = ((from[i + 1] + 2 * (to[i + 1] % 4 == 1)) % 4 + (to[i + 1] - to[i + 1] % 2)) % 8;
				}

				if (pndists[i].second > 0 && pndists[i].first > 0) {
					if (from[i] != (to[i] + 2) % 8 && from[i] != (to[i] - 2 + 8) % 8) {
						int prdist = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, (from[i] + 2) % 8), posC(c, (from[i] + 2) % 8));
						int pldist = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, (from[i] - 2 + 8) % 8), posC(c, (from[i] - 2 + 8) % 8));
						int nrdist = dist2(posR(nr, from[i + 1]), posC(nc, from[i + 1]), posR(r, (to[i] + 2) % 8), posC(c, (to[i] + 2) % 8));
						int nldist = dist2(posR(nr, from[i + 1]), posC(nc, from[i + 1]), posR(r, (to[i] - 2 + 8) % 8), posC(c, (to[i] - 2 + 8) % 8));
						int pdist = pndists[i].first;
						int ndist = pndists[i].second;

						prdist += (prdist == 0) * 100000;pldist += (pldist == 0) * 100000;
						nrdist += (nrdist == 0) * 100000;nldist += (nldist == 0) * 100000;

						if (min(pldist, prdist) + ndist <= pdist + min(nldist, nrdist)) {
							if (pldist <= prdist)from[i] = (from[i] - 2 + 8) % 8;
							else from[i] = (from[i] + 2) % 8;
							pndists[i - 1].second = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, from[i]), posC(c, from[i]));
							pndists[i].first = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, from[i]), posC(c, from[i]));
						}
						else {
							if (nldist <= nrdist)to[i] = (to[i] - 2 + 8) % 8;
							else to[i] = (to[i] + 2) % 8;
							pndists[i].second = dist2(posR(nr, from[i + 1]), posC(nc, from[i + 1]), posR(r, to[i]), posC(c, to[i]));
							pndists[i + 1].first = dist2(posR(nr, from[i + 1]), posC(nc, from[i + 1]), posR(r, to[i]), posC(c, to[i]));
						}
					}
				}
				else if (pndists[i].first > 0) {
					int ndist = dist2(Pos[n][order[i]] / S, Pos[n][order[i]] % S, Pos[n][order[i + 1]] / S, Pos[n][order[i + 1]] % S);

					if (ndist == 2) {
						if (from[i] == (to[i] + 2) % 8 || from[i] == (to[i] - 2 + 8) % 8) {
							if (pndists[i + 1].second > 0) {
								int prdist = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, (from[i] + 2) % 8), posC(c, (from[i] + 2) % 8));
								int pldist = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, (from[i] - 2 + 8) % 8), posC(c, (from[i] - 2 + 8) % 8));
								int pdist = pndists[i].first;

								prdist += (prdist == 0) * 100000;pldist += (pldist == 0) * 100000;

								if (min(pldist, prdist) + 1 <= pdist + 1 + 1 * (i < order.size() - 2)) {
									if (pldist <= prdist)from[i] = (from[i] - 2 + 8) % 8;
									else from[i] = (from[i] + 2) % 8;
									to[i] = (from[i] + 2) % 8;

									pndists[i - 1].second = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, from[i]), posC(c, from[i]));
									pndists[i].first = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, from[i]), posC(c, from[i]));
									pndists[i].second = 1;
									pndists[i + 1].first = 1;
								}
								else {
									from[i + 1] = (from[i + 1] + 2 + 2 * (i < (order.size() - 2))) % 8;
									pndists[i].second = 1 + 1 * (i < order.size() - 2);
									pndists[i + 1].first = 1 + 1 * (i < order.size() - 2);
								}
							}
							else {
								from[i + 1] = (from[i + 1] + 2) % 8;
								pndists[i].second = 1;
								pndists[i + 1].first = 1;
							}
						}
						else {
							pndists[i].second = 1;
							pndists[i + 1].first = 1;
							to[i] = (from[i] + 2) % 8;
						}
					}
					else {
						int prdist = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, (from[i] + 2) % 8), posC(c, (from[i] + 2) % 8));
						int pldist = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, (from[i] - 2 + 8) % 8), posC(c, (from[i] - 2 + 8) % 8));
						int pdist = pndists[i].first;
						prdist += (prdist == 0) * 100000;pldist += (pldist == 0) * 100000;
						if (min(pldist, prdist) < pdist) {
							if (pldist <= prdist)from[i] = (from[i] - 2 + 8) % 8;
							else from[i] = (from[i] + 2) % 8;
							pndists[i - 1].second = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, from[i]), posC(c, from[i]));
							pndists[i].first = dist2(posR(pr, to[i - 1]), posC(pc, to[i - 1]), posR(r, from[i]), posC(c, from[i]));
						}

						if (to[i] == from[i] || to[i] == (from[i] + 4) % 8) {
							int tmp = to[i];
							if (r + R[((to[i] + 2) % 8) / 2] == nr + R[from[i + 1] / 2] && c + C[((to[i] + 2) % 8) / 2] == nc + C[from[i + 1] / 2]) to[i] = (to[i] + 2) % 8;
							else to[i] = (to[i] - 2 + 8) % 8;
							if (r + R[tmp / 2] == nr + R[((from[i + 1] + 2) % 8) / 2] && c + C[tmp / 2] == nc + C[((from[i + 1] + 2) % 8) / 2]) from[i + 1] = (from[i + 1] + 2) % 8;
							else from[i + 1] = (from[i + 1] - 2 + 8) % 8;
							pndists[i].second = 1;
							pndists[i + 1].first = 1;
						}
						else {
							pndists[i].second = 1;
							pndists[i + 1].first = 1;
						}
					}
				}
			}

			from[0] = (to[0] + 2) % 8;
			to[order.size() - 1] = (from[order.size() - 1] + 2) % 8;

			for (int i = 0;i < (int)order.size();i++) {
				if (from[i] == to[i] || from[i] == (to[i] + 4) % 8) {
					cerr << char('a' + n) << " " << i << " " << "miss" << endl;
					cerr << from[i] << " " << to[i] << endl;
				}

				int r = Pos[n][order[i]] / S, c = Pos[n][order[i]] % S;

				ret.push_back(to_string(r + R[from[i]/2]) + " " + to_string(c + C[from[i] / 2]));
				ret.push_back(to_string(r + R[((from[i] + 4) % 8) / 2]) + " " + to_string(c + C[((from[i] + 4) % 8) / 2]));
				ret.push_back(to_string(r + R[((to[i] + 4) % 8) / 2]) + " " + to_string(c + C[((to[i] + 4) % 8) / 2]));
				ret.push_back(to_string(r + R[to[i] / 2]) + " " + to_string(c + C[to[i] / 2]));
			}

			push.setTime();
			cerr << "output:" << push.passTime() << "s" << endl << endl;

			tmr.setTime();
			passedTIME += tmr.passTime();
		}

		cerr << "TIME:" << passedTIME << "s" << endl;
		return ret;
	}
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
	for (int i = 0; i < (int)v.size(); ++i)
		cin >> v[i];
}

int main() {
	int S;
	cin >> S;
	vector<string> pattern(S);
	getVector(pattern);

	CrossStitch cs;
	vector<string> ret = cs.embroider(pattern);
	
	cout << ret.size() << endl;
	for (int i = 0; i < (int)ret.size(); ++i)
		cout << ret[i] << endl;
	cout.flush();
}



