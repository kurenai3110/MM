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
	int V;
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

int Cost[50][50];
int cost[50][50];
bool used[50];
vector<int>path;
int order = 0;

int SumDist(const Stage& aStage) {
	fill(used,used+V,false);

	int n = 0;
	int sumDist = 0;
	path.clear();
	while (1) {
		int dist = INF;
		int pos=-1;
		for (int i = 0; i < aStage.asteroidCount(); i++) {
			if (aStage.asteroid(i).exists() && !used[i+1]) {
				if (dist > cost[n][i+1]) {
					dist = cost[n][i + 1];
					pos = i+1;
				}
			}
		}
		if (pos == -1)break;
		used[pos] = true;
		path.push_back(pos-1);
		sumDist += dist;
		n = pos;
	}

	return sumDist;
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
	V = aStage.asteroidCount()+1;

	for (int i = 0; i < 50; i++)for (int j = 0; j < 50; j++) {
		cost[i][j] = INF;
		Cost[i][j] = INF;
	}
	for (int i = 1; i < V; i++) {
		for (int j = 1; j < V; j++) {
			if (i != j) {
				cost[i][j] = aStage.asteroid(i-1).pos().dist(aStage.asteroid(j-1).pos());
				Cost[i][j] = aStage.asteroid(i-1).pos().dist(aStage.asteroid(j-1).pos());
			}
		}
	}

	for (int i = 1; i < V; i++) {
		cost[0][i] = aStage.ship().pos().dist(aStage.asteroid(i - 1).pos());
		cost[i][0] = aStage.ship().pos().dist(aStage.asteroid(i - 1).pos());
	}
	SumDist(aStage);

	order = 0;
}

//------------------------------------------------------------------------------
/// 各ターンでの行動を返します。
///
/// @param[in] aStage 現在ステージの情報。
///
/// @return これから行う行動を表す Action クラス。
Action Answer::getNextAction(const Stage& aStage)
{
	Ship ship = aStage.ship();
    // レーザーが発射できるときは、レーザーを発射します。
    // レーザーが発射できないときは、移動します。
    if(aStage.ship().canShoot()) {
        // 発射目標にする小惑星を決定します。
        Vector2 targetShootPos;

		vector<bool>astDes(V);
		vector<int>X;
		int cnt = 0;

		vector<pair<int, pair<float,float>>> Target;
		Vector2 vectorLaser = ship.pos();
		float rad1 = Math::DegToRad(1);

		for (int i = 1; i < V; i++) {
			if (aStage.asteroid(i-1).exists()) {
				cost[0][i] = ship.pos().dist(aStage.asteroid(i-1).pos());
				cost[i][0] = ship.pos().dist(aStage.asteroid(i-1).pos());
				Cost[0][i] = ship.pos().dist(aStage.asteroid(i - 1).pos());
				Cost[i][0] = ship.pos().dist(aStage.asteroid(i - 1).pos());
			}
			else {
				cost[0][i] = INF;
				cost[i][0] = INF;
				Cost[0][i] = INF;
				Cost[i][0] = INF;
			}
		}

		int Min = INF;
		int Maxcnt = 0;
		float ang;
		for (int i = 0; i < 360; i++) {
			int cnt = 0;
			vectorLaser.rotateRad(rad1);

			bool flag = false;
			for (int j = aStage.asteroidCount() - 1; j >= 0; --j) {
				if (aStage.asteroid(j).exists()) {
					bool isDestroyed = Util::CanShootAsteroid(ship.pos(), ship.pos() + vectorLaser, aStage.asteroid(j).pos(), aStage.asteroid(j).radius());
					if (astDes[j] != isDestroyed){
						flag = true;
						if (isDestroyed) {
							X.push_back(j);
							cnt++;
						}
						else {
							X.erase(find(X.begin(), X.end(), j));
							cnt--;
						}
						astDes[j] = isDestroyed;
					}
				}
			}
			if (flag) {
				for (int k = 0; k < X.size(); k++) {
					for (int l = 0; l < V; l++) {
						cost[X[k] + 1][l] = INF;
						cost[l][X[k] + 1] = INF;
					}
				}
				int res = SumDist(aStage);
				if (Maxcnt <= cnt) {
					Maxcnt = cnt;
					if (Min >= res/cnt) {
						Min = res/cnt;
						ang = i + 1;
					}
				}
				for (int k = 0; k < X.size(); k++) {
					for (int l = 0; l < V; l++) {
						cost[X[k] + 1][l] = Cost[X[k] + 1][l];
						cost[l][X[k] + 1] = Cost[l][X[k] + 1];
					}
				}
			}
		}
		
		float targetRad = Math::DegToRad(ang);
		vectorLaser.rotateRad(targetRad);

		for (int j = aStage.asteroidCount() - 1; j >= 0; --j) {
			if (aStage.asteroid(j).exists()) {
				if (Util::CanShootAsteroid(ship.pos(), ship.pos() + vectorLaser, aStage.asteroid(j).pos(), aStage.asteroid(j).radius())) {
					for (int l = 0; l < V; l++) {
						cost[j + 1][l] = INF;
						cost[l][j + 1] = INF;
						Cost[j + 1][l] = INF;
						Cost[l][j + 1] = INF;
					}
				}
			}
		}
		SumDist(aStage);
		order = 0;

        return Action::Shoot(ship.pos() + vectorLaser);
    } else {
        // 移動目標にする小惑星を決定します。
		/*
		vector<pair<int, int>>dist;
        Vector2 targetMovePos;
        for(int i = 0; i < aStage.asteroidCount(); ++i) {
            if(aStage.asteroid(i).exists()) {
                targetMovePos = aStage.asteroid(i).pos();
				dist.push_back(make_pair(targetMovePos.squareDist(ship.pos()), i));
                //break;
            }
        }
		sort(dist.begin(), dist.end());
		target = dist[0].second;
		return Action::Move(aStage.asteroid(target).pos());
		*/
		if (!aStage.asteroid(path[order]).exists())order++;
		return Action::Move(aStage.asteroid(path[order]).pos());
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
