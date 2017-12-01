/**
 * コンパイル： g++ -std=c++11 Main.cpp
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

/** ブロックが置かれていない場所の値 */
static const int EMPTY_BLOCK = 0;

/** お邪魔ブロックの値 */
int OBSTACLE_BLOCK = -1;

mt19937 MT(8410325);

/**
 * from以上、to未満の乱数を返します。
 */
int randInt(int from, int to) {
	uniform_int_distribution<int> rand(from, to - 1);
	return rand(MT);
}

class Field {
public:
	int W, H;
	vector<vector<int>> blocks;

	Field() {}
	Field(int W, int H) :
		W(W),
		H(H),
		blocks(vector<vector<int>>(H, vector<int>(W, 0))) {}

	/**
	 * フィールド1つ分を入力します。
	 */
	void input() {
		blocks.clear();
		for (int i = 0; i < H; i++) {
			vector<int> row;
			for (int j = 0; j < W; j++) {
				int block;
				cin >> block;
				row.push_back(block);
			}
			blocks.push_back(row);
		}
		string endStr;
		cin >> endStr;
	}

	/**
	 * フィールドの状態を標準エラー出力します。
	 */
	void show() {
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				cerr << blocks[i][j] << " ";
			}
			cerr << endl;
		}
		cerr << "====" << endl;
		cerr.flush();
	}

	//区間内のブロック最少位置
	int min_point(int packWidth, vector<int>packHeights) {
		vector<int>num_blocks(W);

		for (int i = 0; i < W; i++) {
			if (blocks[H - 1][i] == 0) {
				num_blocks[i] = 0;
				continue;
			}
			for (int j = 0; j < H; j++) {
				if (blocks[j][i] != 0) {
					num_blocks[i] = H - j;
					break;
				}
			}
		}
		int min_pos = 0;
		int min_num_block = 100;
		for (int i = 0; i < W - (packWidth - 1); i++) {
			int num_block = 0;
			bool isDanger = false;
			for (int j = 0; j < packWidth; j++) {
				num_block += num_blocks[i + j];
				//isDanger = IsDanger(i+j,packHeights[j]);
			}
			if (min_num_block > num_block) {
				min_num_block = num_block;
				min_pos = i;
			}
		}
		return min_pos;
	}

	//一番上のブロックナンバー
	vector<int>TopBlocks() {
		vector<int>top_blocks(W);
		for (int i = 0; i < W; i++) {
			for (int j = 0; j < H; j++) {
				if (blocks[j][i] != 0) {
					top_blocks[i] = blocks[j][i];
					break;
				}
			}
		}
		return top_blocks;
	}

	//列の高さ
	int getHeight(int pos) {
		int height = H;
		for (int j = 0; j < H; j++) {
			if (blocks[H - 1 - j][pos] == 0) {
				height = j;
				break;
			}
		}
		return height;
	}

	pair<int, int> getInsertPos(int pos) {
		pair<int, int>insertPos;
		for (int j = 0; j < H; j++) {
			if (blocks[H - 1 - j][pos] == 0) {
				insertPos = make_pair(H - 1 - j, pos);
				break;
			}
		}
		return insertPos;
	}

	//デンジャーラインを超えないか
	bool IsDanger(int pos, int packHeight) {
		return packHeight + getHeight(pos) > H;
	}

	int ChainCnt(vector<vector<int> >nextBlock, vector<pair<int, int> >pos) {
		int chainCnt = 0;

		while (1) {
			vector<vector<bool> >block_erase(H);
			for (int i = 0; i < H; i++)block_erase[i].resize(W);

			for (int i = 0; i < pos.size(); i++) {
				for (int j = -1; j < 2; j++)for (int k = -1; k < 2; k++) {
					if (k == 0 && j == 0)continue;
					int cnt = 1, sum = nextBlock[pos[i].first][pos[i].second];
					while (1) {
						if (pos[i].second + j*cnt < 0 || pos[i].second + j*cnt >= W || pos[i].first + k*cnt >= H || pos[i].first + k*cnt < 0)break;
						if (nextBlock[pos[i].first + k*cnt][pos[i].second + j*cnt] == 0)break;
						sum += nextBlock[pos[i].first + k*cnt][pos[i].second + j*cnt];
						if (sum > 10)break;
						else if (sum == 10) {
							for (int l = 0; l < cnt + 1; l++) {
								block_erase[pos[i].first + k*l][pos[i].second + j*l] = true;
							}
							break;
						}
						else cnt++;
					}
				}
			}

			bool have_Chain = false;
			for (int i = 0; i < H; i++)for (int j = 0; j < W; j++)if (block_erase[i][j])have_Chain = true;
			if (!have_Chain) return chainCnt;

			for (int i = 0; i < H; i++)for (int j = 0; j < W; j++)if (block_erase[i][j])nextBlock[i][j] = 0;

			for (int i = 0; i < W; i++) {
				int bottom = -1;
				for (int j = 0; j < H; j++) {
					if (bottom == -1 && nextBlock[H - 1 - j][i] == 0) bottom = H - 1 - j;
					if (bottom != -1 && nextBlock[H - 1 - j][i] != 0) {
						nextBlock[bottom][i] = nextBlock[H - 1 - j][i];
						nextBlock[H - 1 - j][i] = 0;
						bottom--;
					}
				}
			}
			chainCnt++;
		}
	}
};

class Pack {
public:
	int T;
	vector<vector<int>> blocks;
	int rotateCnt;

	Pack(int T, vector<vector<int>> blocks) :
		T(T),
		blocks(blocks),
		rotateCnt(0) {}

	/**
	 * T*Tのサイズのパックを1つ入力します。Packを生成するときは必ず呼び出してください。
	 */
	static Pack inputFirst(int T) {
		vector<vector<int>> blocks;
		for (int i = 0; i < T; i++) {
			vector<int> row;
			for (int j = 0; j < T; j++) {
				int block;
				cin >> block;
				row.push_back(block);
			}
			blocks.push_back(row);
		}
		string endStr;
		cin >> endStr;
		return Pack(T, blocks);
	}

	/**
	 * お邪魔ブロックでパックを埋めます。回転後の状態で呼び出さないでください。
	 */
	int fillWithObstacle(int obstacleCnt) {
		int fillCnt = 0;
		for (int i = 0; i < T; i++) {
			for (int j = 0; j < T; j++) {
				if (fillCnt < obstacleCnt && blocks[i][j] == EMPTY_BLOCK) {
					blocks[i][j] = OBSTACLE_BLOCK;
					fillCnt++;
				}
				if (fillCnt >= obstacleCnt) {
					return fillCnt;
				}
			}
		}
		return fillCnt;
	}

	/**
	 * このパックを90*rot度、回転させます。
	 */
	void rotate(int rot) {
		rotateCnt += rot;
		rotateCnt %= 4;

		for (int r = 0; r < rot; r++) {
			vector<vector<int>> tmp = blocks;
			for (int i = 0; i < T; i++) {
				for (int j = 0; j < T; j++) {
					blocks[j][T - 1 - i] = tmp[i][j];
				}
			}
		}
	}

	/**
	 * このパックの両端を削れるだけ削った、本来の両端のインデックスをpairで返します。
	 * firstが左端、secondが右端のインデックス。
	 */
	pair<int, int> getSides() {
		int minSide = T;
		int maxSide = 0;
		for (int i = 0; i < T; i++) {
			for (int j = 0; j < T; j++) {
				if (blocks[i][j] != EMPTY_BLOCK) {
					minSide = min(minSide, j);
					maxSide = max(maxSide, j);
				}
			}
		}
		return pair<int, int>(minSide, maxSide);
	}

	/**
	 * このパックの状態を標準エラー出力します
	 */
	void show() {
		for (int i = 0; i < T; i++) {
			for (int j = 0; j < T; j++) {
				cerr << blocks[i][j] << " ";
			}
			cerr << endl;
		}
		cerr.flush();
	}

	//下側に寄せたパックを返す
	vector<vector<int> > getNumber() {
		vector<vector<int> > bottom_blocks(T);
		for (int i = 0; i < T; i++)bottom_blocks[i].resize(T);

		for (int i = 0; i < T; i++) {
			int cnt = 0;
			for (int j = 0; j < T; j++) {
				if (blocks[T - 1 - j][i] != 0) {
					bottom_blocks[T - cnt - 1][i] = blocks[T - 1 - j][i];
					cnt++;
				}
			}
		}
		return bottom_blocks;
	}

	int getHeight(int pos) {
		int cnt = 0;
		for (int j = 0; j < T; j++) {
			if (blocks[j][pos] != 0)cnt++;
		}
		return cnt;
	}

	bool IsIncludingOjama() {
		for (int i = 0; i < T; i++) {
			for (int j = 0; j < T; j++) {
				if (blocks[i][j] == 11)return true;
			}
		}
		return false;
	}
};

class State {
public:
	int W, H, T, S, N;
	int turn;
	int remTime;
	vector<Pack> packs;
	Field myField;
	Field enemyField;
	int myObstacle;
	int enemyObstacle;

	State() {}

	/**
	 * Stateを入力します。Stateを生成するときは必ず呼び出してください。
	 */
	static State inputFirst() {
		State st;
		cin >> st.W >> st.H >> st.T >> st.S >> st.N;
		st.packs.clear();
		for (int i = 0; i < st.N; i++) {
			Pack pack = Pack::inputFirst(st.T);
			st.packs.push_back(pack);
		}
		st.myField = Field(st.W, st.H);
		st.enemyField = Field(st.W, st.H);
		return st;
	}

	/**
	 * 1ターン分の入力を行います。
	 */
	bool inputTurn() {
		if (!(cin >> turn >> remTime)) return false;
		cin >> myObstacle;
		myField.input();
		cin >> enemyObstacle;
		enemyField.input();
		return true;
	}

	/**
	 * 現在のターンのパックをどこに落とすか決定して、標準出力します。
	 * 落とす位置の決め方
	 * - 回転角度はランダムで決める
	 * - 落下位置はランダムで決める
	 * - ただし、落下位置は、左端・右端に詰められる場合は、それも考慮する(-2, -1, 8, 9にも落とせる場合は落とす)
	 */
	void executeTurn() {
		int pos = -3;
		myObstacle -= packs[turn].fillWithObstacle(myObstacle);

		int rot = 0;
		int sumHeight = 0;
		for (int i = 0; i < W; i++)sumHeight += myField.getHeight(i);
		int average_Height = sumHeight / W;
		//vector<int>top_blocks = myField.TopBlocks();
		int chain0Pos = W, chain0Ang = 0;
		int insertPos = 0, insertAng = 0;
		vector<pair<pair<int,int>,int>>chain;
		for (int i = 0; i < 4; i++) {
			pair<int, int> sides = packs[turn].getSides();
			int packWidth = sides.second - sides.first + 1;
			vector<vector<int> > pack_number = packs[turn].getNumber();
			for (int j = 0; j < W-packWidth+1; j++) {
				bool isDanger = false;
				for (int k = 0; !isDanger&&k < packWidth; k++) isDanger = myField.IsDanger(j + k, packs[turn].getHeight(sides.first + k));
				vector<vector<int> >next_field(myField.blocks);
				vector<pair<int, int> >next_packPos;

				for (int k = 0; k < packWidth; k++) {
					int cnt = 0;
					for (int l = 0; l < H; l++) {
						if (next_field[H - l - 1][j + k] == 0) {
							next_field[H - l - 1][j + k] += pack_number[T - cnt - 1][sides.first + k];
							if (pack_number[T - cnt - 1][sides.first + k] > 0)next_packPos.push_back(make_pair(H - l - 1, j + k));
							cnt++;
							if (cnt == T)break;
						}
					}
				}
				//評価
        int chainCnt=myField.ChainCnt(next_field, next_packPos);
				if (!isDanger && chainCnt >= max(1, ((H - average_Height) / 3))) {
					rot = i;
					pos = j;
					chain.push_back(make_pair(make_pair(pos,rot),sides.first));
					//cout << pos << " " << rot << endl;
					//cout.flush();
					//return;
				}
				if (!isDanger && myField.ChainCnt(next_field, next_packPos) == 0 && chain0Pos > j - sides.first) {
					chain0Pos = j - sides.first;
					chain0Ang = i;
				}
				if (!isDanger) {
					insertPos = j - sides.first;
					insertAng = i;
				}
			}
			packs[turn].rotate(1);
		}

		pair<int, int> sides = packs[turn].getSides();
		int packWidth = sides.second - sides.first + 1;
		//vector<int>packHeights;
		//for (int i = 0; i < packWidth; i++)packHeights.push_back(packs[turn].getHeight(sides.first + i));
		if (pos == -3) {
			pos = chain0Pos;
			rot = chain0Ang;
			if (pos == W) {
				pos = insertPos;
				rot = insertAng;
			}
		}else{
			sort(chain.begin(),chain.end());
			pos=chain[0].first.first-chain[0].second;
			rot=chain[0].first.second;
		}
		packs[turn].rotate(rot);
		cout << pos << " " << packs[turn].rotateCnt << endl;
		cout.flush();
	}
};

int main() {
	cout << "SampleAI.cpp" << endl;
	cout.flush();

	State state = State::inputFirst();

	OBSTACLE_BLOCK = state.S + 1;

	for (int i = 0; i < state.N; i++) {
		bool inputSuccess = state.inputTurn();

		if (!inputSuccess) {
			cerr << "input Failed" << endl;
			cerr.flush();
			break;
		}

		state.executeTurn();
	}
}
