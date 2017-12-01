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
	/*int min_point(int packWidth, vector<int>packHeights) {
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
			//bool isDanger = false;
			for (int j = 0; j < packWidth; j++) {
				if (num_blocks[i + j] + packHeights[j] < 0)break;
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
	}*/

	//列の高さ
	int getHeight(int pos) {
		int height = H;
		//下から見ていく
		for (int j = 0; j < H; j++) {
			if (blocks[H - 1 - j][pos] == 0) {
				height = j;
				break;
			}
		}
		return height;
	}

	/*
	pair<int, int> getInsertPos(int pos) {
		pair<int, int>insertPos;
		for (int j = 0; j < H; j++) {
			if (blocks[H - 1 - j][pos] == 0) {
				insertPos = make_pair(H - 1 - j, pos);
				break;
			}
		}
		return insertPos;
	}*/

	//デンジャーラインを超えないか
	bool IsDanger(int pos, int packHeight) {
		int height = H;
		//下から見ていく
		for (int j = 0; j < H; j++) {
			if (blocks[H - 1 - j][pos] == 0) {
				height = j;
				break;
			}
		}
		return packHeight + height > H;
	}

	//チェーン数を数え上げる
	//戻り値：チェーン数、できるお邪魔の数、後に残るフィールド
	//引数：パックが落ちたフィールド、落ちたブロックの位置、繋げようとしているチェーン数
	pair<pair<int,int>,vector<vector<int>>> ChainCnt(vector<vector<int> >nextBlock, vector<pair<int, int> >pos, int chainCnt) {
		int score = floor(pow(1.3,chainCnt));
		int E = 0;

		//消されるブロックのマップ
		vector<vector<bool> >block_erase(H);
		for (int i = 0; i < H; i++)block_erase[i].resize(W);

		//チェーンを消す
		for (int i = 0; i < pos.size(); i++) {
			//全方位
			for (int j = -1; j < 2; j++)for (int k = -1; k < 2; k++) {
				if (k == 0 && j == 0)continue;

				//移動回数、累積和
				int cnt = 1, sum = nextBlock[pos[i].first][pos[i].second];

				while (1) {
					if (pos[i].second + j*cnt < 0 || pos[i].second + j*cnt >= W || pos[i].first + k*cnt >= H || pos[i].first + k*cnt < 0)break;
					if (nextBlock[pos[i].first + k*cnt][pos[i].second + j*cnt] == 0)break;

					sum += nextBlock[pos[i].first + k*cnt][pos[i].second + j*cnt];
					if (sum > 10)break;
					else if (sum == 10) {
						for (int l = 0; l < cnt + 1; l++) {
							block_erase[pos[i].first + k*l][pos[i].second + j*l] = true;
							E++;
						}
						break;
					}
					else {
						cnt++;
						int cnt2 = 1,raw_sum=sum;//現在の累積和を保存

						while (1) {
							if (pos[i].second - j*cnt2 < 0 || pos[i].second - j*cnt2 >= W || pos[i].first - k*cnt2 >= H || pos[i].first - k*cnt2 < 0)break;
							if (nextBlock[pos[i].first - k*cnt2][pos[i].second - j*cnt2] == 0)break;

							sum += nextBlock[pos[i].first - k*cnt2][pos[i].second - j*cnt2];
							if (sum > 10)break;
							else if (sum == 10) {
								for (int l = 0; l < cnt; l++) {
									block_erase[pos[i].first + k*l][pos[i].second + j*l] = true;
									E++;
								}for (int l = 0; l < cnt2 + 1; l++) {
									block_erase[pos[i].first - k*l][pos[i].second - j*l] = true;
									E++;
								}
								break;
							}
							else cnt2++;
						}
						//累積和を元に戻す
						sum = raw_sum;
					}
				}
			}
		}

		E = floor(E / 2);
		//チェーンがあるかどうか
		bool have_Chain = false;
		for (int i = 0; i < H; i++)for (int j = 0; j < W; j++)if (block_erase[i][j])have_Chain = true;
		//チェーンがないなら戻る
		if (!have_Chain) return make_pair(make_pair(chainCnt-1,0),nextBlock);
		//チェーンを作るブロックを消す
		for (int i = 0; i < H; i++)for (int j = 0; j < W; j++)if (block_erase[i][j])nextBlock[i][j] = 0;

		//落ちてきたブロックの位置
		vector<pair<int, int>>moved_blocks;

		for (int i = 0; i < W; i++) {
			int bottom = -1;
			for (int j = 0; j < H; j++) {
				//ブロックが落ちる位置
				if (bottom == -1 && nextBlock[H - 1 - j][i] == 0) bottom = H - 1 - j;
				//ブロックを落とす
				if (bottom != -1 && nextBlock[H - 1 - j][i] != 0) {
					nextBlock[bottom][i] = nextBlock[H - 1 - j][i];
					//落ちてきたブロックの位置を格納
					moved_blocks.push_back(make_pair(bottom, i));
					//落としたブロックがあった位置を0
					nextBlock[H - 1 - j][i] = 0;
					//落ちる位置を一つ上に
					bottom--;
				}
			}
		}

		score *= E;
		pair<pair<int, int>,vector<vector<int>>> chainInfo;
		chainInfo= ChainCnt(nextBlock, moved_blocks, chainCnt + 1);
		score += chainInfo.first.second;
		return make_pair(make_pair(chainInfo.first.first,score),chainInfo.second);
	}

	/*
	vector<vector<int> >NextField(vector<vector<int>>nowField) {

	}
	*/
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
	vector<vector<int> > FallPack() {
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

	/*
	bool IsIncludingOjama() {
		for (int i = 0; i < T; i++) {
			for (int j = 0; j < T; j++) {
				if (blocks[i][j] == 11)return true;
			}
		}
		return false;
	}
	*/
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
	int chainNumber = -1;
	int burstNumber = 5;

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

	//チェーンの先読み
	pair<int, int> chainMode(vector<vector<int> >field, int turn_num, int cnt) {
		if (turn_num == 500)return make_pair(0, 0);
		if (cnt > 2)return make_pair(0, 0);

		int pos = -3;
		int rot = 0;

		pair<int, int> chain;
		vector<pair<int, int>>chainInfo;

		packs[turn_num].rotate((4 - packs[turn_num].rotateCnt % 4));
		for (int i = 0; i < 4; i++) {
			pair<int, int> sides = packs[turn_num].getSides();
			int packWidth = sides.second - sides.first + 1;
			vector<vector<int> > pack = packs[turn_num].FallPack();

			for (int j = 0; j < W - packWidth + 1; j++) {
				bool isDanger = false;
				for (int k = 0; !isDanger&&k < packWidth; k++) isDanger = myField.IsDanger(j + k, packs[turn_num].getHeight(sides.first + k));

				vector<vector<int> >next_field(field);
				vector<pair<int, int> >next_packPos;

				//パックを落とす
				for (int k = 0; k < packWidth; k++) {
					int cnt = 0;
					for (int l = 0; l < H; l++) {
						if (next_field[H - l - 1][j + k] == 0) {
							next_field[H - l - 1][j + k] += pack[T - cnt - 1][sides.first + k];
							if (pack[T - cnt - 1][sides.first + k] > 0)next_packPos.push_back(make_pair(H - l - 1, j + k));
							cnt++;
							if (cnt == T)break;
						}
					}
				}

				//評価
				pair<pair<int, int>,vector<vector<int>>> chainCnt = myField.ChainCnt(next_field, next_packPos, 1);
				if (!isDanger && chainCnt.first.first<=1) {
					int now_rotateCnt = packs[turn_num].rotateCnt;

					chain = chainMode(chainCnt.second, turn_num + 1, cnt + 1);
					chainInfo.push_back(chain);

					packs[turn_num].rotate((now_rotateCnt + 4 - packs[turn_num].rotateCnt) % 4);
				}
				else if (!isDanger) {
					chainInfo.push_back(chainCnt.first);
				}

			}
			packs[turn_num].rotate(1);
		}
		sort(chainInfo.begin(), chainInfo.end());
		if (chainInfo.size())return chainInfo.back();

		return make_pair(0, 0);
	}

	struct CHAIN {
		int num, ojama, pos, rot, side_first;

		bool operator<(const CHAIN& right)const {
			if (num != right.num)return num > right.num;
			if (ojama != right.ojama)return ojama>right.ojama;
			return pos-side_first < right.pos-right.side_first;
		}
	};

	void executeTurn() {
		myObstacle -= packs[turn].fillWithObstacle(myObstacle);

		int pos = -3;
		int rot = 0;

		int sumHeight = 0;
		for (int i = 0; i < W; i++)sumHeight += myField.getHeight(i);
		int average_Height = sumHeight / W;

		int insertPos = 0, insertAng = 0;
		vector<CHAIN>chainBurst;
		vector<CHAIN>chain;
		vector<CHAIN>chain_insert;

		burstNumber = max(1, H - average_Height);
		chainNumber = -1;

		packs[turn].rotate((4 - packs[turn].rotateCnt % 4));
		for (int i = 0; i < 4; i++) {
			pair<int, int> sides = packs[turn].getSides();
			int packWidth = sides.second - sides.first + 1;
			vector<vector<int> > fallingPack = packs[turn].FallPack();

			for (int j = 0; j < W - packWidth + 1; j++) {
				bool isDanger = false;
				for (int k = 0; !isDanger&&k < packWidth; k++) isDanger = myField.IsDanger(j + k, packs[turn].getHeight(sides.first + k));

				vector<vector<int> >next_field(myField.blocks);
				vector<pair<int, int> >next_packPos;

				for (int k = 0; k < packWidth; k++) {
					int cnt = 0;
					for (int l = 0; l < H; l++) {
						if (next_field[H - l - 1][j + k] == 0) {
							next_field[H - l - 1][j + k] += fallingPack[T - cnt - 1][sides.first + k];
							if (fallingPack[T - cnt - 1][sides.first + k] > 0)next_packPos.push_back(make_pair(H - l - 1, j + k));
							cnt++;
							if (cnt == T)break;
						}
					}
				}

				//評価
				pair<pair<int, int>,vector<vector<int>>> chainCnt = myField.ChainCnt(next_field, next_packPos, 1);
				if (!isDanger && chainCnt.first.first >= burstNumber) {
					rot = i;
					pos = j;
					chain.push_back({ chainCnt.first.first,chainCnt.first.second/5,pos, rot, sides.first });
				}
				else if (!isDanger&&chainCnt.first.first <=1) {
					int now_rotateCnt = packs[turn].rotateCnt;

					pair<int, int> flag = chainMode(chainCnt.second, turn + 1, 1);

					packs[turn].rotate((now_rotateCnt + 4 - packs[turn].rotateCnt) % 4);

					if (flag.first > chainNumber) {
						chainNumber = flag.first;
						rot = i;
						pos = j;
						chain.push_back({ flag.first,flag.second/5,pos, rot, sides.first });
					}
				}
				else if (!isDanger) {
					insertPos = j;
					insertAng = i;
					chain_insert.push_back({ chainCnt.first.first,chainCnt.first.second/5,insertPos,insertAng,sides.first });
				}
			}
			packs[turn].rotate(1);
		}

		if (pos == -3) {
			sort(chain_insert.begin(), chain_insert.end());
			if (chain_insert.size()) {
				pos = chain_insert[0].pos - chain_insert[0].side_first;
				rot = chain_insert[0].rot;
			}
			else {
				pos = 0; rot = 0;
			}
		}
		else {
			if (chainBurst.size()) {
				sort(chainBurst.begin(), chainBurst.end());
				pos = chainBurst[0].pos - chainBurst[0].side_first;
				rot = chainBurst[0].rot;
			}
			else {
				sort(chain.begin(), chain.end());
				pos = chain[0].pos - chain[0].side_first;
				rot = chain[0].rot;
			}
		}

		packs[turn].rotate(rot);
		cout << pos << " " << packs[turn].rotateCnt << endl;
		cout.flush();
	}
};

int main() {
	cout << "KURENAI-chan" << endl;
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
