//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください
//------------------------------------------------------------------------------

#include "Answer.hpp"
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
using namespace std;

#define INF (int)1e7

/// プロコン問題環境を表します。
namespace hpc {
	vector<int>goal;
	int limit;
	int mincnt;
	int target;
	bool update;
	vector<bool>targetAsteroid;
	Vector2 moveVector;
	int AsteroidCnt;
	int exsistAsteroidCnt;
	vector<vector<float>>E;
	int num;


	float DistEva(const Stage& aStage) {
		float sumDist = 0;

		for (int j = 0; j < AsteroidCnt; j++) {
			if (aStage.asteroid(j).exists() && !targetAsteroid[j]) {
				sumDist += aStage.ship().pos().dist(aStage.asteroid(j).pos());
			}
		}
		return sumDist;
	}

	float DenseEva(const Stage& aStage, int n) {
		float sumDense = 0;

		for (int j = 0; j < AsteroidCnt; j++) {
			if (aStage.asteroid(j).exists() && !targetAsteroid[j]) {
				sumDense += aStage.asteroid(n).pos().dist(aStage.asteroid(j).pos());
			}
		}
		sumDense *= aStage.ship().pos().dist(aStage.asteroid(n).pos());

		return sumDense;
	}

	float MinDist(const Stage& aStage) {

		int cnt = 1;
		float minDist = 1000;
		int n = -1;
		for (int i = 0; i < AsteroidCnt; i++) {
			if (aStage.asteroid(i).exists() && !targetAsteroid[i]) {
				float dist = aStage.ship().pos().dist(aStage.asteroid(i).pos());
				if (minDist > dist) {
					minDist = dist;
					n = i;
				}
			}
		}
		if (n == -1)return 0;
		while (1) {
			float mindist = 1000;
			int astNum = -1;
			targetAsteroid[n] = true;
			for (int i = 0; i < AsteroidCnt; i++) {
				if (aStage.asteroid(i).exists() && !targetAsteroid[i]) {
					if (mindist > E[n][i]) {
						mindist = E[n][i];
						astNum = i;
					}
				}
			}
			if (astNum == -1)break;
			n = astNum;
			minDist += mindist;

			cnt++;
		}

		return minDist;
	}

	void solve(const Stage& aStage, vector<bool>astExsist, Vector2 ship, int cnt, vector<int>path) {
		if (limit > 100)return;
		if (cnt >= mincnt) return;
		limit++;
		int astcnt = 0;
		for (int i = 0; i < AsteroidCnt; i++) {
			if (!astExsist[i]) {
				astcnt++;
				targetAsteroid[i] = false;
			}else{
				targetAsteroid[i] = true;
			}
		}

		if (astcnt == 0) {
			mincnt = min(cnt, mincnt);
			//cout << mincnt << endl;
			goal.clear();
			goal = path;
			return;
		}

		int rest = 40;

		while (rest) {
			int tar = -1;
			float minDist = 1000;
			for (int i = 0; i < AsteroidCnt; i++) {
				if (!astExsist[i]) {
					float dist = ship.dist(aStage.asteroid(i).pos()) - aStage.asteroid(i).radius();
					if (dist < minDist) {
						minDist = dist;
						tar = i;
					}
				}
			}
			if (tar == -1) {
				mincnt = min(cnt, mincnt);
				//cout << mincnt << endl;
				goal.clear();
				goal = path;
				return;
			}

			if (minDist > rest * 2) {
				ship += (aStage.asteroid(tar).pos() - ship).unit(rest * 2);
				break;
			}
			else {
				ship += (aStage.asteroid(tar).pos() - ship).unit((int)ceil(minDist / 2) * 2);
				rest -= (int)ceil(minDist / 2);
				astExsist[tar] = true;
				targetAsteroid[tar] = true;
			}
		}

		//cout << ship.x << " " << ship.y << endl;

		float rad1 = Math::DegToRad(1);
		Vector2 laser = Vector2(0, 1000);
		vector<pair<float, int>> x;
		int tcnt = 0;
		vector<bool>astDes(AsteroidCnt);
		vector<vector<bool>>astex(360);
		for (int j = 0; j < 360; j++) {
			bool flag = false;

			for (int k = 0; k < AsteroidCnt; k++) {
				if (!astExsist[k]) {
					bool isDestroyed = Util::CanShootAsteroid(ship, ship + laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius());
					if (astDes[k] != isDestroyed) {
						flag = true;
						if (isDestroyed) {
							targetAsteroid[k] = true;
							tcnt++;
						}
						else {
							tcnt--;
							targetAsteroid[k] = false;
						}
						astDes[k] = isDestroyed;
					}
				}
			}
			if (flag && tcnt) {
				astex[j].resize(AsteroidCnt);
				for (int i = 0; i < AsteroidCnt; i++)astex[j][i] = targetAsteroid[i];
				x.push_back(make_pair(DistEva(aStage) + MinDist(aStage), j));
				for (int i = 0; i < AsteroidCnt; i++)targetAsteroid[i] = astex[j][i];
			}
			laser.rotateRad(rad1);
		}
		sort(x.begin(), x.end());
		for (int i = 0; i < 4 && i < (int)x.size(); i++) {
			path.push_back(x[i].second);
			solve(aStage, astex[x[i].second], ship, cnt + 1, path);
			path.pop_back();
		}
		return;
	}

	//------------------------------------------------------------------------------
	/// Answer クラスのコンストラクタです。
	///
	/// @note ここにインスタンス生成時の処理を書くことができますが、何も書かなくても構いません。
	Answer::Answer()
	{

	}

	//------------------------------------------------------------------------------
	/// Answer クラスのデストラクタです。
	///
	/// @note ここにインスタンス破棄時の処理を書くことができますが、何も書かなくても構いません。
	Answer::~Answer()
	{
	}

	//------------------------------------------------------------------------------
	/// 各ステージ開始時に呼び出されます。
	///
	/// @note ここで、各ステージに対して初期処理を行うことができます。
	///
	/// @param[in] aStage 現在のステージ。
	void Answer::init(const Stage& aStage)
	{
		goal.clear();
		mincnt = 100;
		num = 0;
		update = true;
		target = -1;
		AsteroidCnt = aStage.asteroidCount();
		exsistAsteroidCnt = aStage.asteroidCount();
		targetAsteroid.resize(AsteroidCnt);
		E.resize(AsteroidCnt);
		for (int i = 0; i < AsteroidCnt; i++) {
			E[i].resize(AsteroidCnt);
			for (int j = 0; j < AsteroidCnt; j++) {
				if (i == j)E[i][j] = INF;
				else {
					E[i][j] = aStage.asteroid(i).pos().dist(aStage.asteroid(j).pos());
				}
			}
		}

		Vector2 ship = aStage.ship().pos();
		vector<bool>astinit(AsteroidCnt);
		int rest = 40;
		while (rest) {
			int tar = -1;
			float minDist = 1000;
			for (int i = 0; i < AsteroidCnt; i++) {
				if (!astinit[i]) {
					float dist = ship.dist(aStage.asteroid(i).pos()) - aStage.asteroid(i).radius();
					if (dist < minDist) {
						minDist = dist;
						tar = i;
					}
				}
			}

			if (minDist > rest * 2) {
				ship += (aStage.asteroid(tar).pos() - ship).unit(rest * 2);
				break;
			}
			else {
				ship += (aStage.asteroid(tar).pos() - ship).unit((int)ceil(minDist / 2) * 2);
				rest -= (int)ceil(minDist / 2);
				astinit[tar] = true;
			}
		}
		for (int i = 0; i < AsteroidCnt; i++) {
			if (!astinit[i]) {
				targetAsteroid[i] = false;
			}
			else {
				targetAsteroid[i] = true;
			}
		}

		//cout << ship.x << " "<<ship.y << endl;
		float rad1 = Math::DegToRad(1);
		Vector2 laser = Vector2(0, 1000);
		vector<pair<float, int>> x;
		int cnt = 0;
		vector<bool>astDes(AsteroidCnt);
		vector<vector<bool>>astex(360);
		for (int j = 0; j < 360; j++) {
			bool flag = false;
			for (int k = 0; k < AsteroidCnt; k++) {
				if (!astinit[k]) {
					bool isDestroyed = Util::CanShootAsteroid(ship, ship + laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius());
					if (astDes[k] != isDestroyed) {
						flag = true;
						if (isDestroyed) {
							targetAsteroid[k] = true;
							cnt++;
						}
						else {
							cnt--;
							targetAsteroid[k] = false;
						}
						astDes[k] = isDestroyed;
					}
				}
			}
			if (flag && cnt) {
				astex[j].resize(AsteroidCnt);
				for (int i = 0; i < AsteroidCnt; i++)astex[j][i] = targetAsteroid[i];
				x.push_back(make_pair(DistEva(aStage) + MinDist(aStage), j));
				for (int i = 0; i < AsteroidCnt; i++)targetAsteroid[i] = astex[j][i];
			}
			laser.rotateRad(rad1);
		}
		sort(x.begin(), x.end());
		//vector<pair<float, int>>z;
		vector<int>path;
		for (int i = 0; i < 4 && i < (int)x.size(); i++) {
			limit = 0;
			path.push_back(x[i].second);
			solve(aStage, astex[x[i].second], ship, 1, path);
			path.pop_back();
			//z.push_back(make_pair(x[i].first*astcnt, x[i].second));
		}
		//sort(z.begin(), z.end());
	}

	//------------------------------------------------------------------------------
	/// 各ターンでの行動を返します。
	///
	/// @param[in] aStage 現在ステージの情報。
	///
	/// @return これから行う行動を表す Action クラス。
	Action Answer::getNextAction(const Stage& aStage)
	{
		//cout << aStage.turn() << " " << aStage.existingAsteroidCount() << endl;

		int cnt = 0;
		for (int i = 0; i < AsteroidCnt; i++) {
			if (aStage.asteroid(i).exists()) {
				targetAsteroid[i] = false;
				cnt++;
			}
			else {
				targetAsteroid[i] = true;
			}
		}
		if (exsistAsteroidCnt != cnt) {
			exsistAsteroidCnt = cnt;
			update = true;
		}

		Ship ship = aStage.ship();

		if (aStage.ship().canShoot()) {
			//cout << ship.pos().x << " " << ship.pos().y << endl;

			Vector2 laser = Vector2(0, 1000);
			float ang = Math::DegToRad(goal[num]);
			laser.rotateRad(ang);
			num++;
			
			return Action::Shoot(ship.pos()+laser);
		}
		else {
			if (update) {
				float minDist = 1000;

				for (int j = 0; j < AsteroidCnt; j++) {
					if (aStage.asteroid(j).exists()) {

						float dist = ship.pos().dist(aStage.asteroid(j).pos()) - aStage.asteroid(j).radius();
						if (dist < minDist) {
							minDist = dist;
							target = j;
						}

					}
				}
				moveVector = aStage.asteroid(target).pos() - ship.pos();
				moveVector.unitAssign(2.0f);

				update = false;
			}
			return Action::Move(ship.pos()+moveVector);
		}
	}

	//------------------------------------------------------------------------------
	/// 各ステージ終了時に呼び出されます。
	///
	/// @param[in] aStage 現在ステージの情報。
	///
	/// @note ここにステージ終了時の処理を書くことができますが、何も書かなくても構いません。
	void Answer::finalize(const Stage& aStage)
	{
		//cout << aStage.turn() << endl;
	}

} // namespace
  // EOF