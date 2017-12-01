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
#include <float.h>
#include <iomanip>
using namespace std;

#define INF (int)1e7
#define LIMIT 30

/// プロコン問題環境を表します。
namespace hpc {
	vector<int>lasers;					//レーザーを放つ角度
	int num;							//lasersの要素番号
	int limit;							//探索回数
	int mincnt;							//レーザーを放つ最小の数
	int target;							//一番近いアステロイドの番号
	bool update;						//更新が必要かどうか
	vector<bool>isAsteroids;			//探索時に使うアステロイドの存在
	int AsteroidCnt;					//アステロイドの数
	int exsistAsteroidCnt;				//残っているアステロイドの数
	vector<vector<float>>E;				//各アステロイド間の距離

	//宇宙船と残っているアステロイドとの距離の和
	float DistEva(const Stage& aStage, Vector2 ship) {
		float sumDist = 0;

		for (int j = 0; j < AsteroidCnt; j++) {
			if (aStage.asteroid(j).exists() && isAsteroids[j]) {
				sumDist += ship.dist(aStage.asteroid(j).pos());
			}
		}
		return sumDist;
	}

	//一番近いアステロイドを通って行った時の距離
	float MinDist(const Stage& aStage, Vector2 ship) {
		int cnt = 1;
		float minDist = INF;
		int n = -1;
		for (int i = 0; i < AsteroidCnt; i++) {
			if (aStage.asteroid(i).exists() && isAsteroids[i]) {
				float dist = ship.dist(aStage.asteroid(i).pos());
				if (minDist > dist) {
					minDist = dist*2;
					n = i;
				}
			}
		}
		if (n == -1)return 0;
		while (1) {
			float mindist = INF;
			int astNum = -1;
			isAsteroids[n] = false;
			for (int i = 0; i < AsteroidCnt; i++) {
				if (aStage.asteroid(i).exists() && isAsteroids[i]) {
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

	//どの向きにレーザーを放つか探索
	void solve(const Stage& aStage, vector<bool>astExsist, Vector2 ship, int cnt, vector<int>tmp_lasers) {
		if (cnt <= 3)limit = 0;
		if (limit > LIMIT)return;
		if (cnt*41 >= mincnt)return;
		limit++;

		//cout << mincnt << endl;

		//残っているアステロイドの数をカウント
		int astcnt = 0;
		for (int i = 0; i < AsteroidCnt; i++) {
			if (astExsist[i]) {
				astcnt++;
				isAsteroids[i] = true;
			}
			else {
				isAsteroids[i] = false;
			}
		}
		//残っていなかったらlasersに設定
		if (astcnt == 0) {
			mincnt = min(cnt*41, mincnt);
			lasers.clear();
			lasers = tmp_lasers;
			return;
		}

		int rest = 40;			//残りの移動回数
		while (rest >= 0) {
			//一番近いアステロイド
			int tar = -1;
			float minDist = INF;
			for (int i = 0; i < AsteroidCnt; i++) {
				if (astExsist[i]) {
					float dist = ship.dist(aStage.asteroid(i).pos());
					if (dist < minDist) {
						minDist = dist;
						tar = i;
					}
				}
			}

			//残っていなかったらlasersに設定
			if (tar == -1) {
				if (cnt * 41 + 40 - rest < mincnt) {
					mincnt = cnt * 41 + 40 - rest;
					lasers.clear();
					lasers = tmp_lasers;
				}
				return;
			}

			//一番近いアステロイドに残りの移動回数で行けるかどうか
			if (minDist - aStage.asteroid(tar).radius() >= rest * 2) {
				for (int i = 0; i < rest; i++) {
					Vector2 dir = aStage.asteroid(tar).pos() - ship;

					if (dir.length() > Parameter::ShipMaxSpeed()) {
						dir = dir / dir.length() * Parameter::ShipMaxSpeed();
					}
					ship += dir;
				}
				break;
			}
			else {
				int a = 0;

				while (1) {
					Vector2 dir = aStage.asteroid(tar).pos() - ship;

					if (dir.length() > Parameter::ShipMaxSpeed()) {
						dir = dir / dir.length() * Parameter::ShipMaxSpeed();
					}
					ship += dir;
					a++;
					Vector2 e = aStage.asteroid(tar).pos() - ship;
					float r = aStage.asteroid(tar).radius();
					if (e.length() < r) break;
				}

				rest -= a;
				astExsist[tar] = false;
				isAsteroids[tar] = false;
			}
		}

		if (cnt*41 + 40 >= mincnt)return;

		//レーザーを360度撃つ
		float rad1 = Math::DegToRad(1);
		Vector2 laser = Vector2(0, 1000);
		vector<pair<float, int>> laserEva;
		int tcnt = 0;
		vector<bool>astDes(AsteroidCnt);
		vector<vector<bool>>astex(360);
		for (int j = 0; j < 360; j++) {
			bool flag = false;

			for (int k = 0; k < AsteroidCnt; k++) {
				if (astExsist[k]) {
					bool isDestroyed = Util::CanShootAsteroid(ship, ship + laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius());
					if (astDes[k] != isDestroyed) {
						flag = true;
						if (isDestroyed) {
							isAsteroids[k] = false;
							tcnt++;
						}
						else {
							tcnt--;
							isAsteroids[k] = true;
						}
						astDes[k] = isDestroyed;
					}
				}
			}
			if (flag && tcnt) {
				astex[j].resize(AsteroidCnt);
				for (int i = 0; i < AsteroidCnt; i++)astex[j][i] = isAsteroids[i];

				//DistEvaとMinDistの和で評価
				laserEva.push_back(make_pair(DistEva(aStage, ship) + MinDist(aStage, ship), j));

				for (int i = 0; i < AsteroidCnt; i++)isAsteroids[i] = astex[j][i];
			}
			laser.rotateRad(rad1);
		}

		//小さいやつをいくつか探索続行
		sort(laserEva.begin(), laserEva.end());
		for (int i = 0; i < astcnt / 4.0f && i < (int)laserEva.size(); i++) {
		//for (int i = 0; i < 1 && i < (int)laserEva.size(); i++) {
			tmp_lasers.push_back(laserEva[i].second);
			solve(aStage, astex[laserEva[i].second], ship, cnt + 1, tmp_lasers);
			tmp_lasers.pop_back();
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
		//初期化処理
		lasers.clear();
		mincnt = 3000;
		num = 0;
		update = true;
		target = -1;
		AsteroidCnt = aStage.asteroidCount();
		exsistAsteroidCnt = aStage.asteroidCount();
		isAsteroids.resize(AsteroidCnt);
		for (int i = 0; i < AsteroidCnt; i++) isAsteroids[i] = true;
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

		//探索開始
		limit = 0;
		vector<int>tmp_lasers;
		solve(aStage, isAsteroids, aStage.ship().pos(), 1, tmp_lasers);
	}

	//------------------------------------------------------------------------------
	/// 各ターンでの行動を返します。
	///
	/// @param[in] aStage 現在ステージの情報。
	///
	/// @return これから行う行動を表す Action クラス。
	Action Answer::getNextAction(const Stage& aStage)
	{
		//アステロイドの数が変わっていたら更新
		int cnt = 0;
		for (int i = 0; i < AsteroidCnt; i++) {
			if (aStage.asteroid(i).exists()) {
				cnt++;
			}
		}
		if (exsistAsteroidCnt != cnt) {
			exsistAsteroidCnt = cnt;
			update = true;
		}

		Vector2 ship = aStage.ship().pos();

		if (aStage.ship().canShoot()) {
			//探索した向きにレーザーを放つ
			Vector2 laser = Vector2(0, 1000);
			float ang = Math::DegToRad(lasers[num]);
			laser.rotateRad(ang);
			num++;

			return Action::Shoot(ship + laser);
		}
		else {
			if (update) {

				float minDist = 1000;

				//一番近いアステロイドに向かう
				for (int j = 0; j < AsteroidCnt; j++) {
					if (aStage.asteroid(j).exists()) {

						float dist = ship.dist(aStage.asteroid(j).pos());
						if (dist < minDist) {
							minDist = dist;
							target = j;
						}
					}
				}

				update = false;
			}
			return Action::Move(aStage.asteroid(target).pos());
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