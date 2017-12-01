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
	int target;
	bool update = true;
	vector<bool>targetAsteroid;
	Vector2 moveVector;
	int AsteroidCnt;
	int exsistAsteroidCnt;
	vector<vector<float>>E;
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
	target = -1;
	update = true;
	moveVector = Vector2::Zero();
	AsteroidCnt = aStage.asteroidCount();
	exsistAsteroidCnt = aStage.asteroidCount();
	targetAsteroid.resize(AsteroidCnt);
	E.resize(AsteroidCnt);
	for (int i = 0; i < AsteroidCnt; i++) {
		E[i].resize(AsteroidCnt);
		for (int j = 0; j < AsteroidCnt; j++) {
			if(i==j)E[i][j] = INF;
			else {
				E[i][j] = aStage.asteroid(i).pos().dist(aStage.asteroid(j).pos());
			}
		}
	}
}

float DistEva(const Stage& aStage,Vector2 moveVec) {
	float sumDist = 0;

	for (int j = 0; j < AsteroidCnt; j++) {
		if (aStage.asteroid(j).exists() && !targetAsteroid[j]) {
			sumDist += (aStage.ship().pos()+moveVec).dist(aStage.asteroid(j).pos());
		}
	}
	return sumDist;
}

float DenseEva(const Stage& aStage , int n) {
	float sumDense = 0;

	for (int j = 0; j < AsteroidCnt; j++) {
		if (aStage.asteroid(j).exists() && !targetAsteroid[j]) {
			sumDense += aStage.asteroid(n).pos().dist(aStage.asteroid(j).pos());
		}
	}
	sumDense *= aStage.ship().pos().dist(aStage.asteroid(n).pos());

	return sumDense;
}

float MinDist(const Stage& aStage,Vector2 moveVec) {
	//int maxCnt = aStage.existingAsteroidCount();
	int cnt = 1;
	float minDist = 1000;
	int n=-1;
	for (int i = 0; i < AsteroidCnt; i++) {
		if (aStage.asteroid(i).exists() && !targetAsteroid[i]) {
			float dist = (aStage.ship().pos()+moveVec).dist(aStage.asteroid(i).pos());
			if (minDist > dist) {
				minDist = dist;
				n = i;
			}
		}
	}
	if (n == -1)return 0;
	while (1) {
		float mindist=1000;
		int astNum=-1;
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

//------------------------------------------------------------------------------
/// 各ターンでの行動を返します。
///
/// @param[in] aStage 現在ステージの情報。
///
/// @return これから行う行動を表す Action クラス。
Action Answer::getNextAction(const Stage& aStage)
{
	//cout <<aStage.turn()<<" "<< aStage.existingAsteroidCount() << endl;
	
	int cnt = 0;
	for (int i = 0; i < AsteroidCnt; i++) {
		if (aStage.asteroid(i).exists()) {
			targetAsteroid[i] = false;
			cnt++;
		}
	}
	if (exsistAsteroidCnt != cnt) {
		exsistAsteroidCnt = cnt;
		update = true;
	}

	Ship ship = aStage.ship();

	if (aStage.ship().canShoot()) {
		float rad1 = Math::DegToRad(1);
		Vector2 laser = Vector2(0, 1000);
		vector<pair<float, int>> x;
		for (int j = 0; j < 360; j++) {
			cnt = 0;
			for (int k = 0; k < AsteroidCnt; k++) {
				if (aStage.asteroid(k).exists()) {
					targetAsteroid[k] = false;
					if (Util::CanShootAsteroid(ship.pos(), ship.pos() + laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius())) {
						cnt++;
						targetAsteroid[k] = true;
					}
				}
			}
			x.push_back(make_pair(DistEva(aStage,Vector2::Zero()) + MinDist(aStage,Vector2::Zero()), j));
			laser.rotateRad(rad1);
		}
		sort(x.begin(), x.end());
		//reverse(x.begin(), x.end());

		float ang = Math::DegToRad(x[0].second);
		laser.rotateRad(ang);

		return Action::Shoot(ship.pos() + laser);
	}

	else {
		float minDist = 1000;
		if (update) {
			update = false;

			for (int j = 0; j < AsteroidCnt; j++) {
				if (aStage.asteroid(j).exists()) {

					float dist = ship.pos().dist(aStage.asteroid(j).pos());
					if (dist < minDist) {
						minDist = dist;
						target = j;
					}
				}
			}
			if (minDist / 2 < aStage.ship().restReloadTurn()) {
				return Action::Move(aStage.asteroid(target).pos());
			}

			moveVector = aStage.asteroid(target).pos() - ship.pos();
			moveVector.unitAssign(2.0f*aStage.ship().restReloadTurn());
			//moveVector.rotateRad(-Math::PI / 4);

			float rad1 = Math::DegToRad(1);
			vector<pair<float, int>> y;
			for (int i = 0; i < 360; i++) {
				Vector2 laser = Vector2(0, 1000);
				vector<pair<float, int>> x;
				for (int k = 0; k < AsteroidCnt; k++) if (aStage.asteroid(k).exists()) targetAsteroid[k] = false;
				cnt = 0;
				vector<bool>astDes(AsteroidCnt);
				for (int j = 0; j < 360; j++) {

					bool flag = false;
					for (int k = 0; k < AsteroidCnt; k++) {
						if (aStage.asteroid(k).exists()) {
							bool isDestroyed = Util::CanShootAsteroid(ship.pos() + moveVector, ship.pos() + moveVector + laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius());
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
					if (flag && cnt)x.push_back(make_pair((ship.pos()+moveVector).dist(aStage.asteroid(target).pos())+DistEva(aStage,moveVector) + MinDist(aStage, moveVector), j));
					laser.rotateRad(rad1);
				}
				sort(x.begin(), x.end());
				y.push_back(make_pair(x[0].first, i));

				moveVector.rotateRad(rad1);
			}
			sort(y.begin(), y.end());
			//moveVector.rotateRad(-Math::PI/2);
			moveVector.rotateRad(Math::DegToRad(y[0].second));

			target = -1;

			return Action::Move(ship.pos() + moveVector);			
		}

		if (target != -1) return Action::Move(aStage.asteroid(target).pos());
		else return Action::Move(ship.pos() + moveVector);
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
