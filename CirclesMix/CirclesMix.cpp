#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
#include <string>
#include <cmath>
#include <functional>

using namespace std;

#define REP(i,n) for(int i=0;i<n;i++)

#define LIMIT 19.8

#define LOCAL_SCALE 2500000000ULL
#define TC_SCALE 2800000000ULL
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
	Timer(double l) {
		start = getCycle();
		limit = l;
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
	double getLimit() { return limit; }
};

class Xor128 {
	unsigned static int r, c, z, w;
public:
	Xor128() {
		r = 31103110, c = 123456789, z = 521288629, w = 88675123;
	}

	unsigned int rand()
	{
		unsigned int t;
		t = (r ^ (r << 11)); r = c; c = z; z = w;
		return(w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
	}
};
unsigned int Xor128::r, Xor128::c, Xor128::z, Xor128::w;

struct POINT {
	int r, c;

	POINT(int r = 0, int c = 0) :r(r), c(c) {}

	POINT operator+(POINT& p) {
		return POINT(r + p.r, c + p.c);
	}
	POINT operator-(POINT& p) {
		return POINT(r - p.r, c - p.c);
	}
	void operator+=(POINT& p) {
		r += p.r;
		c += p.c;
	}
	void operator-=(POINT& p) {
		r -= p.r;
		c -= p.c;
	}
};

struct COLOR {
	int R, G, B;

	COLOR() :R(0), G(0), B(0) {}
	COLOR(int r, int g, int b) :R(r), G(g), B(b) {}

	COLOR operator + (COLOR& c) { return COLOR(R + c.R, G + c.G, B + c.B); }
	COLOR operator - (COLOR& c) { return COLOR(R - c.R, G - c.G, B - c.B); }
	COLOR operator * (double a) { return COLOR(R*a, G*a, B*a); }
	bool operator==(COLOR& c) { return R == c.R&&G == c.G&&B == c.B; }
};

struct ACTION {
	POINT pos;
	int radius;
	int color;

	ACTION(POINT pos=POINT(), int radius=0, int color = 0) :pos(pos), radius(radius), color(color) {}
};


COLOR color_decode(int color) {
	COLOR RGB;
	RGB.R = (color >> 16);
	RGB.G = (color >> 8) & 0xff;
	RGB.B = (color) & 0xff;

	return RGB;
}

int color_encode(COLOR RGB) {
	int color, R, G, B;
	R = (RGB.R << 16);
	G = (RGB.G << 8);
	B = (RGB.B);

	color = R | G | B;

	return color;
}

struct INFO {
	int score, scores_id;

	INFO(int score=0, int scores_id=0) :score(score), scores_id(scores_id) {}

	bool operator<(const INFO& r)const {
		return score < r.score;
	}
	bool operator>(const INFO& r)const {
		return score > r.score;
	}
};

struct MOVE {
	int score, dir;

	MOVE(int score = 0, int dir = 0) :score(score),dir(dir) {}

	bool operator<(const MOVE& r)const {
		return score < r.score;
	}
};

int rr[8] = { -1,-1,1,1,-1,0,1,0 };
int cc[8] = { -1,1,1,-1, 0,1,0,-1 };

int medC[256][256];
int diffC[256][256];
int addC[256][256];

class CirclesMix {
	int N, H, W;

	vector<vector<COLOR>>Image;
	vector<vector<COLOR>>targetImage;

	vector<ACTION>Action;

	vector<vector<POINT>>radiusPos;
	vector<vector<POINT>>endPos[8];

	vector<ACTION>nextActions;
	vector<INFO>ScoreInfo;

	Timer tmr;
	Xor128 xor128;

	int maxRadius = 1000;
	int maxDepth = 100000;

	int mode = 1;
public:
	vector<int> ret;
	void add(ACTION act) {
		ret.emplace_back(act.pos.r);
		ret.emplace_back(act.pos.c);
		ret.emplace_back(act.radius);
		ret.emplace_back(act.color);
	}
	void set_ret(vector<ACTION>& Action) {
		ret.clear();
		for (int i = 0; i < Action.size(); i++) {
			add(Action[i]);
		}
	}


	vector<int> drawImage(int H_, vector<int>& pixels, int N_) {
		init(H_, pixels, N_);

		draw();

		set_ret(Action);

		return ret;
	}


	void init(int H_, vector<int>& pixels, int N_) {
		tmr.setLimit(LIMIT);

		N = N_;
		H = H_;
		W = pixels.size() / H;

		ret.reserve(4 * N);

		Action.reserve(N);
		Image.resize(H, vector<COLOR>(W));
		targetImage.resize(H, vector<COLOR>(W));

		REP(i,H)REP(j,W) targetImage[i][j] = color_decode(pixels[W*i + j]);

		initPos();

		REP(i, 256)REP(j, 256) {
			medC[i][j] = (i + j) / 2;
			diffC[i][j] = abs(i - j);
			addC[i][j] = max(0, min(255, i + 2 * (j - i)));
		}
	}


	void draw() {
		REP(n_i,N){
			if (tmr.Over())break;

			Timer tmr2((tmr.getLimit() - tmr.getTime()) / (N - n_i));
			for (int d = 0; d < maxDepth; d++) {
				if (tmr2.Over())break;

				int r = xor128.rand() % H;
				int c = xor128.rand() % W;
				POINT pos(r, c);

				if (Image[r][c] == targetImage[r][c]) {
					d--;
					continue;
				}

				COLOR addedC;
				addedC.R = addC[Image[r][c].R][targetImage[r][c].R];
				addedC.G = addC[Image[r][c].G][targetImage[r][c].G];
				addedC.B = addC[Image[r][c].B][targetImage[r][c].B];

				ACTION act(pos, 0, color_encode(addedC));
				INFO info;

				bool pushed = false;
				int score = 0, pre_score = 0;
				REP(rad, maxRadius + 1) {
					REP(j, radiusPos[rad].size()) {
						POINT npos = act.pos + radiusPos[rad][j];
						if (is_OoR(npos))continue;

						int r, c;
						r = npos.r;
						c = npos.c;

						COLOR nextC = medColor(addedC, Image[r][c]);
						score -= colorDist(targetImage[r][c], Image[r][c]);
						score += colorDist(targetImage[r][c], nextC);
					}

					if (score > pre_score) {
						MOVE m;
						POINT tmpdir;
						do {
							m = moveEvaluation(act, mode);
							if (m.score < 0) {
								score += m.score;
								POINT dir(rr[m.dir], cc[m.dir]);
								act.pos += dir;
								tmpdir += dir;
							}
						} while (m.score < 0);

						if (score > pre_score) {
							act.pos -= tmpdir;

							info.scores_id = ScoreInfo.size();
							info.score = pre_score;
							ScoreInfo.emplace_back(info);

							act.radius--;
							nextActions.emplace_back(act);
							pushed = true;
							break;
						}
					}

					pre_score = score;
					act.radius++;
				}
				if (pushed == false) {
					info.scores_id = ScoreInfo.size();
					info.score = pre_score;
					ScoreInfo.emplace_back(info);

					act.radius--;
					nextActions.emplace_back(act);
				}
			}

			vector<INFO>sortedInfo(ScoreInfo);
			sort(sortedInfo.begin(), sortedInfo.end());
			int id = sortedInfo[0].scores_id;	

			ACTION act = nextActions[id];
			Action.emplace_back(act);

			updateImage(act);

			if (act.radius < 5)mode = 0;

			REP(i, nextActions.size()){
				if (dist(act.pos, nextActions[i].pos) <= act.radius + nextActions[i].radius) {
					swap(nextActions[i], nextActions[nextActions.size()-1]);
					swap(ScoreInfo[i], ScoreInfo[ScoreInfo.size() - 1]);

					ScoreInfo[i].scores_id = i;

					nextActions.pop_back();
					ScoreInfo.pop_back();

					i--;
				}
			}

			if (ScoreInfo.size() > N-n_i-1) {
				int score_border = sortedInfo[N-n_i-1].score;
				REP(i, nextActions.size()) {
					if (score_border < ScoreInfo[i].score) {
						swap(nextActions[i], nextActions[nextActions.size() - 1]);
						swap(ScoreInfo[i], ScoreInfo[ScoreInfo.size() - 1]);

						ScoreInfo[i].scores_id = i;

						nextActions.pop_back();
						ScoreInfo.pop_back();

						i--;
					}
				}
			}

		}

	}

	MOVE moveEvaluation(ACTION& act, int mode) {
		COLOR addedC = color_decode(act.color);
		vector<int>In(4), Out(4);
		vector<MOVE>m(4);
		REP(k2, 4) {
			int k = k2 + 4 * mode;
			REP(a, endPos[k][act.radius].size()) {
				POINT InP = act.pos + endPos[k][act.radius][a];
				POINT dir(rr[k], cc[k]);
				POINT OutP = InP + dir;

				bool Inflag, Outflag;
				Inflag = is_OoR(InP);
				Outflag = is_OoR(OutP);

				if (!Inflag) {
					COLOR nextC = medColor(addedC, Image[InP.r][InP.c]);
					In[k2] += colorDist(targetImage[InP.r][InP.c], Image[InP.r][InP.c]);
					In[k2] -= colorDist(targetImage[InP.r][InP.c], nextC);
				}
				if (!Outflag) {
					COLOR nextC = medColor(addedC, Image[OutP.r][OutP.c]);
					Out[k2] -= colorDist(targetImage[OutP.r][OutP.c], Image[OutP.r][OutP.c]);
					Out[k2] += colorDist(targetImage[OutP.r][OutP.c], nextC);
				}
			}
		}

		REP(k2, 4) {
			int k = k2 + 4 * mode;
			m[k2].dir = k;
			m[k2].score = Out[k2] + In[k2 ^ 2];
		}
		sort(m.begin(), m.end());

		return m[0];
	}

	void updateImage(ACTION& act) {
		COLOR addedC = color_decode(act.color);
		REP(rad, act.radius + 1)REP(j, radiusPos[rad].size()) {
			POINT p = act.pos + radiusPos[rad][j];

			if (is_OoR(p))continue;

			int r, c;
			r = p.r;
			c = p.c;

			Image[r][c] = medColor(Image[r][c],addedC);
		}
	}

	void initPos() {
		vector<vector<char>>used(2 * maxRadius + 3, vector<char>(2 * maxRadius + 3));

		radiusPos.resize(maxRadius+1);
		REP(k,8)endPos[k].resize(maxRadius+1);

		REP(rad, maxRadius+1){
			for (int r = -rad; r <= rad; r++) {
				int dc = (int)sqrt(pow2(rad) - pow2(r));
				if (pow2(r) + pow2(dc) > pow2(rad))continue;

				for (int c = -dc; c <= 0; c++) {
					if (used[maxRadius + r + 1][maxRadius + c + 1])continue;
					used[maxRadius + r + 1][maxRadius + c + 1] = true;
					used[maxRadius + r + 1][maxRadius - c + 1] = true;

					if (c) {
						radiusPos[rad].emplace_back(POINT(r, c));
						radiusPos[rad].emplace_back(POINT(r, -c));
					}
					else {
						radiusPos[rad].emplace_back(POINT(r, 0));
					}
				}
			}

			POINT p(-rad, 0);
			while (1) {
				for (int k = 0; k < 4; k++) {
					POINT v = POINT(rr[k], cc[k]);
					POINT np = p + v;

					if (used[maxRadius + np.r + 1][maxRadius + np.c + 1] == false) {
						endPos[k][rad].emplace_back(POINT(p.r, p.c));
						if (p.c != 0)endPos[k ^ 1][rad].emplace_back(POINT(p.r, -p.c));
						if (p.r != 0)endPos[k ^ 3][rad].emplace_back(POINT(-p.r, p.c));
						if (p.c != 0)if (p.r != 0)endPos[k ^ 2][rad].emplace_back(POINT(-p.r, -p.c));
					}
				}

				for (int k = 4; k < 8; k++) {
					POINT v = POINT(rr[k], cc[k]);
					POINT np = p + v;

					if (used[maxRadius + np.r + 1][maxRadius + np.c + 1] == false) {
						endPos[k][rad].emplace_back(POINT(p.r, p.c));
						if (p.c != 0) {
							if (k & 1)endPos[k ^ 2][rad].emplace_back(POINT(p.r, -p.c));
							else endPos[k][rad].emplace_back(POINT(p.r, -p.c));
						}
						if (p.r != 0) {
							if (k & 1)endPos[k][rad].emplace_back(POINT(-p.r, p.c));
							else endPos[k ^ 2][rad].emplace_back(POINT(-p.r, p.c));
						}
						if (p.c != 0)if (p.r != 0)endPos[k ^ 2][rad].emplace_back(POINT(-p.r, -p.c));
					}
				}

				if (used[maxRadius + p.r + 1][maxRadius + p.c - 1 + 1])p.c--;
				else if (used[maxRadius + p.r + 1 + 1][maxRadius + p.c + 1]) p.r++;
				else break;
			}
		}
	}

	int colorDist(COLOR& c1, COLOR& c2) {
		return diffC[c1.R][c2.R] + diffC[c1.G][c2.G] + diffC[c1.B][c2.B];
	}
	COLOR medColor(COLOR& c1, COLOR& c2) {
		return COLOR(medC[c1.R][c2.R], medC[c1.G][c2.G], medC[c1.B][c2.B]);
	}

	int pow2(int a) {
		return a*a;
	}
	double pow2(double a) {
		return a*a;
	}

	bool is_OoR(POINT& p) {
		return p.r < 0 || p.r >= H || p.c < 0 || p.c >= W;
		return false;
	}

	double dist(POINT& p1, POINT p2) {
		return sqrt(pow2(p1.r - p2.r) + pow2(p1.c - p2.c));
	}
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
	for (int i = 0; i < v.size(); ++i)
		cin >> v[i];
}

int main() {
	CirclesMix cm;
	int H;
	cin >> H;
	int S;
	cin >> S;
	vector<int> pixels(S);
	getVector(pixels);

	int N;
	cin >> N;// cerr << N << endl;

	vector<int> ret = cm.drawImage(H, pixels, N);
	cout << ret.size() << endl;
	for (int i = 0; i < (int)ret.size(); ++i)
		cout << ret[i] << endl;
	cout.flush();
}
