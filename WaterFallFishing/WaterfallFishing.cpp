#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <random>
#include <time.h>
using namespace std;

#define all(a) a.begin(),a.end()

class RandomXor {
	unsigned static int x, y, z, w;
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
unsigned int RandomXor::x, RandomXor::y, RandomXor::z, RandomXor::w;

inline int convertCharToCount(char c) {
	if (c >= 'A')return c - 'A' + 10;
	return c - '0';
}

class Simulation {
	RandomXor rx;
	mt19937 mt;
	int width, days;
	vector<vector<int>>data;
	vector<double>ret;
public:
	void init(vector<vector<int>>data_) {
		width = data_[0].size() - 2;
		days = data_.size();
		data = data_;
		mt.seed(rx.rand());
		ret.resize(width);
	}

	void getDailyFish(vector<int>& DailyFish) {
		int numFish = 5 + rx.rand() % 96;
		int mean = rx.rand() % width;
		int stDev = 2 + rx.rand() % (width / 4 - 1);

		normal_distribution<> norm(mean, stDev);
		for (int i = 0; i < numFish; i++) {
			int x = (int)round(norm(mt));
			if (x < 0 || x > width - 1) {
				i--;
				continue;
			}
			DailyFish[x]++;
		}
	}

	void analyze() {
		for (int i = 0; i < days; i++) {
			double mean = 0, std = 0;
			vector<int>pos;
			int n_fish = 0;
			int l = 0, r = width + 1;

			for (int j = 1; j <= width; j++) {
				if (data[i][j]) {
					for (int k = 0; k < data[i][j]; k++)pos.push_back(j);
					mean += j*data[i][j];
					n_fish += data[i][j];

					if (l == 0)l = j;
					r = j;
				}
			}
			mean /= n_fish;

			for (int j = 0; j < n_fish; j++) {
				std += pow((pos[j] - mean), 2);
			}
			std /= n_fish;
			std = sqrt(std);
			std = max(0.01, std);

			normal_distribution<> norm(mean, std);

			vector<double>E(width);
			int cnt = 0;
			while (1) {
				int x = (int)round(norm(mt));
				if (x < 1 || x > width) {
					continue;
				}
				x--;
				E[x]++;

				cnt++;
				if (cnt == 100000)break;
			}

			for (int j = 0; j < width; j++) {
				E[j] = (E[j] / cnt)*n_fish;
			}

			for (int j = l; j <= r; j++) {
				ret[j - 1] += (data[i][j] - E[j - 1]);
			}
		}
	}

	vector<int>setTraps() {
		vector<int>traps;

		vector<pair<int, int>>fish(width);
		for (int j = 0; j < width; j++)fish[j].second = j;

		for (int i = 0; i < width*10; i++) {
			vector<int>DailyFish(width);

			for (int k = 0; k < 3; k++) {
				getDailyFish(DailyFish);
			}

			double maxFish = 0;
			int target = 0;
			for (int j = 0; j < width; j++) {
				DailyFish[j] *= ret[j];
				if (maxFish < DailyFish[j]) {
					maxFish = DailyFish[j];
					target = j;
				}
			}

			fish[target].first++;
		}

		sort(all(fish));
		reverse(all(fish));

		traps.push_back(fish[0].second);

		return traps;
	}
};

class WaterfallFishing {
public:
	vector<int> placeTraps(vector<string> data_) {
		int W = data_[0].size();

		vector<vector<int>>data(data_.size());
		for (int i = 0; i < data.size(); i++) {
			data[i].resize(W + 2);
			for (int j = 1; j < W + 1; j++)data[i][j] = convertCharToCount(data_[i][j - 1]);
		}

		Simulation sim;
		sim.init(data);
		sim.analyze();

		// put traps in all locations on the river
		vector<int> traps;
		traps = sim.setTraps();
		return traps;
	}
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
	for (int i = 0; i < v.size(); ++i) {
		cin >> v[i];
		//cerr << v[i] << endl;
	}
}

int main() {
	WaterfallFishing wf;
	int S;
	cin >> S;
	//cerr << S << endl;
	vector<string> data(S);
	getVector(data);

	vector<int> ret = wf.placeTraps(data);
	cout << ret.size() << endl;
	for (int i : ret)
		cout << i << endl;
	cout.flush();
}
