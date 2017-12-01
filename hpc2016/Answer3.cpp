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
	int laserNum;
	bool goal=false;
	Vector2 TargetVector;
	vector<bool>targetAsteroid;
	vector<pair<int, int>> AllLaser;
	bool update = true;
	bool online = false;
	Vector2 projectedVector;
	Vector2 astprojectedVector;
	Vector2 moveVector;
	Vector2 astmoveVector;
	struct astLaser {
		Vector2 laser, ast;
	};
	vector<astLaser> astlaser;
	int AsteroidCnt;
	int exsistAsteroidCnt;
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
	AsteroidCnt = aStage.asteroidCount();
	exsistAsteroidCnt = aStage.asteroidCount();
	targetAsteroid.resize(AsteroidCnt);
	update = true;
	online = false;
	//cout<<"initialized"<<endl;
}

float DistEva(const Stage& aStage) {
	float sumDist = 0;
	for (int i = 0; i < AsteroidCnt; i++) {
		if (aStage.asteroid(i).exists() && !targetAsteroid[i]) {
			for (int j = 0; j < AsteroidCnt; j++) {
				if (aStage.asteroid(j).exists() && !targetAsteroid[j]) {
					sumDist += aStage.asteroid(i).pos().dist(aStage.asteroid(j).pos());
				}
			}
		}
	}
	return sumDist;
}

//------------------------------------------------------------------------------
/// 各ターンでの行動を返します。
///
/// @param[in] aStage 現在ステージの情報。
///
/// @return これから行う行動を表す Action クラス。
Action Answer::getNextAction(const Stage& aStage)
{
	int cnt = 0;
	/*for (int i = 0; i < AsteroidCnt; i++) {
		if (aStage.asteroid(i).exists()) {
			cnt++;
		}
	}
	if (cnt != exsistAsteroidCnt) {
		exsistAsteroidCnt = cnt;
		update = true;
	}*/

	Ship ship = aStage.ship();

	cnt = 0;
	//for (int i = 0; i < AsteroidCnt; i++)if (aStage.asteroid(i).exists())cnt++;
	//cout << cnt << endl;

	if (update) {
		for (int k = 0; k < AsteroidCnt; k++) {
			if (aStage.asteroid(k).exists()) {
				targetAsteroid[k] = false;
			}
		}
		goal = false;
		AllLaser.clear();
		astlaser.clear();
		int index = 0;
		for (int i = 0; i < AsteroidCnt; i++) {
			if (aStage.asteroid(i).exists()) {
				Vector2 laser = Vector2(0, 1000);
				Vector2 astPos = aStage.asteroid(i).pos();
				float rad1 = Math::DegToRad(1);
				for (int j = 0; j < 180; j++) {
					int cnt = 0;
					laser.rotateRad(rad1);
					vector<bool>astDes(AsteroidCnt);
					bool flag = false;
					for (int k = 0; k < AsteroidCnt; k++) {
						if (i == k)continue;
						if (aStage.asteroid(k).exists()) {
							bool isDestroyed = Util::CanShootAsteroid(astPos, astPos + laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius()) || Util::CanShootAsteroid(astPos, astPos - laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius());
							if (astDes[k] != isDestroyed) {
								flag = true;
								if (isDestroyed) {
									cnt++;
								}
								else {
									cnt--;
								}
								astDes[k] = isDestroyed;
							}
						}
					}
					if (flag && cnt) {
						AllLaser.push_back(make_pair(cnt, index));
						astlaser.push_back({ laser,astPos });
						index++;
					}
				}
			}
		}

	}

	bool last = true;
	for (int k = 0; k < AsteroidCnt; k++) {
		if (!aStage.asteroid(k).exists()) {
			targetAsteroid[k] = true;
		}
		else if (!targetAsteroid[k])last=false;
	}
	if (last)for (int k = 0; k < AsteroidCnt; k++)targetAsteroid[k] = false;

	if (aStage.ship().canShoot()) {
		//cout << "shooted" << endl;
		if (AllLaser.size() == 0)return Action::Shoot(TargetVector);

		if (online) {
			update = true;
			return Action::Shoot(ship.pos() + TargetVector);
		}
		else {
			float rad1 = Math::DegToRad(1);
			Vector2 laser = Vector2(0, 1000);
			vector<pair<int, int>> x;
			for (int j = 0; j < 360; j++) {
				int cnt = 0;
				laser.rotateRad(rad1);
				vector<bool>astDes(AsteroidCnt);
				bool flag = false;
				float dist = 0;
				for (int k = 0; k < AsteroidCnt; k++) {
					if (aStage.asteroid(k).exists() && !targetAsteroid[k]) {
						bool isDestroyed = Util::CanShootAsteroid(ship.pos(), ship.pos() + laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius());
						if (astDes[k] != isDestroyed) {
							flag = true;
							if (isDestroyed) {
								dist = max(dist, ship.pos().squareDist(aStage.asteroid(k).pos()));
								cnt++;
							}
							else {
								cnt--;
							}
							astDes[k] = isDestroyed;
						}
					}
				}
				if (flag && cnt) {
					x.push_back(make_pair(cnt+dist, j));
				}
			}
			sort(x.begin(), x.end());
			reverse(x.begin(), x.end());

			float ang = Math::DegToRad(x[0].second + 1);
			laser.rotateRad(ang);

			return Action::Shoot(ship.pos() + laser);
		}
	}
	else {
		//cout << "moved" << endl;
		if (AllLaser.size() == 0) {
			for (int i = 0; i < AsteroidCnt; i++) {
				if (aStage.asteroid(i).exists()) {
					TargetVector = aStage.asteroid(i).pos();
					update = false;
					return Action::Move(TargetVector);
				}
			}
		}
		if (update) {
			vector<pair<float, int>>cntDist;
			for (int i = 0; i < AllLaser.size(); i++) {
				projectedVector = ship.pos().getProjected(astlaser[AllLaser[i].second].laser);
				astprojectedVector = astlaser[AllLaser[i].second].ast.getProjected(astlaser[AllLaser[i].second].laser);
				moveVector = projectedVector - ship.pos();
				astmoveVector = astprojectedVector - astlaser[AllLaser[i].second].ast;
				moveVector -= astmoveVector;
				int cnt1 = 0, cnt2 = 0;
				for (int k = 0; k < AsteroidCnt; k++) {
					if (aStage.asteroid(k).exists()) {
						targetAsteroid[k] = false;
						bool isDestroyed1 = Util::CanShootAsteroid(ship.pos() + moveVector, ship.pos() + moveVector + astlaser[AllLaser[i].second].laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius());
						if (isDestroyed1) {
							cnt1++;
							targetAsteroid[k] = true;
						}
					}
				}
				for (int k = 0; k < AsteroidCnt; k++) {
					if (aStage.asteroid(k).exists()) {
						targetAsteroid[k] = false;
						bool isDestroyed2 = Util::CanShootAsteroid(ship.pos() + moveVector, ship.pos() + moveVector - astlaser[AllLaser[i].second].laser, aStage.asteroid(k).pos(), aStage.asteroid(k).radius());
						if (isDestroyed2) {
							cnt2++;
							targetAsteroid[k] = true;
						}
					}
				}
				cntDist.push_back(make_pair(max(cnt1,cnt2) - moveVector.length() / 80, (i+1)*pow(-1,cnt2>cnt1)));
			}
			sort(cntDist.begin(), cntDist.end());
			reverse(cntDist.begin(), cntDist.end());

			laserNum = cntDist[0].second*pow(-1, cntDist[0].second < 0) - 1;
			projectedVector = ship.pos().getProjected(astlaser[AllLaser[laserNum].second].laser);
			astprojectedVector = astlaser[AllLaser[laserNum].second].ast.getProjected(astlaser[AllLaser[laserNum].second].laser);
			astmoveVector = astprojectedVector - astlaser[AllLaser[laserNum].second].ast;
			projectedVector -= astmoveVector;
			TargetVector = astlaser[AllLaser[laserNum].second].laser*pow(-1, cntDist[0].second < 0);

			moveVector = projectedVector - ship.pos();
			for (int k = 0; k < AsteroidCnt; k++) {
				if (aStage.asteroid(k).exists()) {
					targetAsteroid[k] = false;
					if (Util::CanShootAsteroid(ship.pos() + moveVector, ship.pos() + moveVector + astlaser[AllLaser[laserNum].second].laser*pow(-1, cntDist[0].second < 0), aStage.asteroid(k).pos(), aStage.asteroid(k).radius())) {
						targetAsteroid[k] = true;
					}
				}
			}

			//cout << "updated" << endl;
			update = false;
		}

		moveVector = projectedVector - ship.pos();
		//cout << moveVector.length() << endl;

		if (moveVector.isZeroStrict() || goal) {
			goal = true;
			online = true;
			int cnt = 0;
			for (int k = 0; k < AsteroidCnt; k++) {
				if (aStage.asteroid(k).exists()) {
					if (Util::CanShootAsteroid(ship.pos(), ship.pos() - TargetVector, aStage.asteroid(k).pos(), aStage.asteroid(k).radius()))cnt++;
				}
			}
			if(cnt)return Action::Move(ship.pos() - TargetVector);
			else return Action::Move(ship.pos() + TargetVector);
		}
		else {
			online = false;
			return Action::Move(ship.pos() + moveVector);
		}
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
}

} // namespace
// EOF
