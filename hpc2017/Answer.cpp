//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Answer.hpp"
#include "Timer.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <iomanip>
#include <chrono>
using namespace std;

namespace hpc {
	//疑似乱数
	class Xor128 {
		unsigned static int x, y, z, w;
	public:
		Xor128() {
			x = 31103110, y = 123456789, z = 521288629, w = 88675123;
		}

		unsigned int rand()
		{
			unsigned int t;
			t = (x ^ (x << 11)); x = y; y = z; z = w;
			return(w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
		}
	};
	unsigned int Xor128::x, Xor128::y, Xor128::z, Xor128::w;
	Xor128 xor128;



	//Vector2と半径から交差判定
	bool IsIntersect(Vector2& lPos, float lRad, Vector2& rPos, float rRad) {
		float sumRadius = lRad + rRad;
		return lPos.squareDist(rPos) <= sumRadius * sumRadius;
	}

	int Dist[Parameter::MaxHouseCount+1][Parameter::MaxHouseCount+1];

	//簡易移動ターン数計算
	int EasyMoveTurn(Vector2& srcPos, float srcRad, Vector2& dstPos, float dstRad, float speed) {
		int turn = 0;
		float length = (dstPos - srcPos).length();
		length = max(0.0f, length - srcRad - dstRad);
		turn = (int)ceil(length / speed);
		return turn;
	}

	//srcPosを実際に移動させつつ、移動ターン数を計算
	int MoveVector2(Vector2& srcPos, float srcRad, Vector2& dstPos, float dstRad, float speed) {
		int turn = 0;
		while (!IsIntersect(srcPos, srcRad, dstPos, dstRad)) {
			Vector2 dir = dstPos - srcPos;

			float length = dir.length();
			//if (length > speed) {
				dir = dir / length * speed;
			//}

			srcPos += dir;
			turn++;
		}
		return turn;
	}

	//目的の位置へターン数分動かす
	void MoveVector2(Vector2& srcPos, Vector2& dstPos, float speed, int turn) {
		for (int t = 0; t < turn; t++) {
			Vector2 dir = dstPos - srcPos;

			float length = dir.length();
			if (length > speed) {
				dir = dir / length * speed;
			}

			srcPos += dir;
		}
	}

	//幾何
#define EPS (1e-10)
	static const int COUNTER_CLOCKWISE = 1;
	static const int CLOCKWISE = -1;
	static const int ONLINE_BACK = 2;
	static const int ONLINE_FRONT = -2;
	static const int ON_SEGMENT = 0;

	struct Segment {
		Vector2 p1, p2;
		Segment(Vector2 p1_ = Vector2(), Vector2 p2_ = Vector2()) :p1(p1_), p2(p2_) {}
	};
	struct Circle {
		Vector2 c;
		double r;
		Circle(Vector2 c_ = Vector2(), double r_ = 0.0) :c(c_), r(r_) {}
	};

	double dot(Vector2 a, Vector2& b) {
		return a.x * b.x - a.y*b.y;
	}
	double cross(Vector2& a, Vector2 b) {
		return a.x*b.y - a.y*b.x;
	}
	Vector2 project(Segment& s, Vector2& p) {
		Vector2 base = s.p2 - s.p1;
		double r = dot(p - s.p1, base) / base.squareLength();
		return s.p1 + base*r;
	}

	int ccw(Vector2& p0, Vector2& p1, Vector2& p2) {
		Vector2 a = p1 - p0;
		Vector2 b = p2 - p0;
		if (cross(a, b) > EPS)return COUNTER_CLOCKWISE;
		if (cross(a, b) < -EPS)return CLOCKWISE;
		if (dot(a, b) < -EPS)return ONLINE_BACK;
		if (a.squareLength() < b.squareLength())return ONLINE_FRONT;

		return ON_SEGMENT;
	}
	bool intersect(Vector2& p1, Vector2& p2, Vector2& p3, Vector2& p4) {
		return(ccw(p1, p2, p3)*ccw(p1, p2, p4) <= 0 && ccw(p3, p4, p1) * ccw(p3, p4, p2) <= 0);
	}
	bool intersect(Segment& s1, Segment& s2) {
		return intersect(s1.p1, s1.p2, s2.p1, s2.p2);
	}

	double arg(Vector2 p) { return atan2(p.y, p.x); }
	Vector2 polar(double a, double r) { return Vector2(cos(r)*a, sin(r)*a); }

	Vector2 getCrossPoint(Segment& s1, Segment& s2) {
		Vector2 base = s2.p2 - s2.p1;
		double d1 = abs(cross(base, s1.p1 - s2.p1));
		double d2 = abs(cross(base, s1.p2 - s2.p1));
		double t = d1 / (d1 + d2);
		return s1.p1 + (s1.p2 - s1.p1) * t;
	}
	Segment getCrossPoints(Circle& c1, Circle& c2) {
		double d = (c1.c - c2.c).length();
		double c = (c1.r*c1.r + d*d - c2.r*c2.r) / (2 * c1.r*d);
		c = max(-1.0, min(1.0, c));
		double a = acos(c);
		a *= 0.999;
		double t = arg(c2.c - c1.c);
		return Segment(c1.c + polar(c1.r, t + a), c1.c + polar(c1.r, t - a));
	}
	Segment getCrossPoints(Circle& c, Segment& s) {
		Vector2 pr = project(s, c.c);
		Vector2 e = (s.p2 - s.p1) / (s.p2 - s.p1).length();
		double base = sqrt(c.r*c.r - (pr - c.c).squareLength());
		return Segment(pr + e*base, pr - e*base);
	}

	Vector2 adjustPoint(Vector2& prePos, Vector2& predstPos, Vector2& dstPos, float r1, float r2) {
		r2 *= 0.9999;
		Circle c1(prePos, r1);
		Circle c2(predstPos, r2);

		Segment s1 = getCrossPoints(c1, c2);
		Segment s2(prePos, dstPos);
		Segment s3(predstPos, dstPos);
		int flag = 2 * intersect(s1, s2) + intersect(s1, s3);

		Segment s;

		if (flag >= 2) {
			s = getCrossPoints(c1, s2);
		}
		else if (flag == 1) {
			s = getCrossPoints(c2, s3);
		}
		else {
			s = s1;
		}

		if (dstPos.dist(s.p1) < dstPos.dist(s.p2)) {
			return s.p1;
		}
		else {
			return s.p2;
		}
	}


	struct TURNsort {
		int turn, index;
		TURNsort(int turn_ = 0, int index_ = 0, bool flag_ = false) :turn(turn_), index(index_) {}
		bool operator<(const TURNsort& r) const { return turn < r.turn;}
	};
	struct UFOINFO {
		Vector2 Pos;
		int box;
		UFOINFO(Vector2 Pos_ = Vector2(), int box_ = 0) :Pos(Pos_), box(box_) {}
	};

	static const int TARGET_HOUSE_NONE = -1;
	static const int MAX_TURN = 100; //最大ターン数、100以上になることはない

	vector<int> targetHouseInd;//配達する家のIndex
	vector<Vector2> targetHouseVec;//配達する家のVector2
	vector<Vector2> targetVec;//目的地のVector2

	vector<vector<int>>pathInd;
	vector<vector<Vector2>>pathVec;
	vector<vector<Vector2>>tarVec;

	vector<vector<int>>bestPathInd;
	vector<vector<Vector2>>bestPathVec;
	vector<vector<Vector2>>bestHouseVec;

	//UFOの数、家の数
	int ufoCount, houseCount;

	Answer::Answer()
	{
	}

	Answer::~Answer()
	{
	}

	void Answer::init(const Stage& aStage)
	{
		Timer tmr;
		tmr.start();

		ufoCount = aStage.ufos().count();
		houseCount = aStage.houses().count();

		for (int i = 0; i <= houseCount; i++) {
			for (int j = 0; j <= houseCount; j++) {
				Vector2 srcPos, dstPos;
				if (i)srcPos = aStage.houses()[i-1].pos();
				else srcPos = aStage.office().pos();
				if (j)dstPos = aStage.houses()[j-1].pos();
				else dstPos = aStage.office().pos();
				Dist[i][j] = EasyMoveTurn(srcPos, Parameter::SmallUFORadius, dstPos, Parameter::HouseRadius, Parameter::SmallUFOMaxSpeed);
			}
		}

		targetHouseInd.resize(ufoCount);
		targetHouseVec.resize(ufoCount);
		targetVec.resize(ufoCount);

		pathInd.resize(ufoCount);
		pathVec.resize(ufoCount);
		tarVec.resize(ufoCount);

		UFOINFO LargeUFOVecs[Parameter::LargeUFOCount][MAX_TURN + 10];

		//限界ターン数、このターン数を超えたらダメ
		int maxTurn = (MAX_TURN - 1)*(0.25 + 0.75*houseCount / 100.);

		//現在地、目的地、前の現在地、前の目的地
		Vector2 Pos, dstPos, prePos, predstPos, tmpPos;

		int loop = 0;
		while (tmr.elapsedSec() < 0.29) {
			vector<TURNsort>order(houseCount);
			for (int houseIndex = 0; houseIndex < houseCount; houseIndex++)order[houseIndex] = TURNsort(0, houseIndex);

			int house_cnt = 0; //配達した家の数
			int max_turn = 0; //かかったターン数

			for (int i = 0; i < ufoCount; i++) {
				pathInd[i].clear();
				pathVec[i].clear();
				tarVec[i].clear();

				int turn = 0, supply_turn = 0, pre_moveTurn = 0;

				UFOType ufoType = aStage.ufos()[i].type();
				float ufoRadius = aStage.ufos()[i].radius();
				float ufoSpeed = aStage.ufos()[i].maxSpeed();
				int ufoCapacity = aStage.ufos()[i].capacity();

				//補給回数
				int supply_cnt = 0;

				int preId = -1;

				Pos = aStage.office().pos();
				predstPos = prePos = Pos;
				while (1) {
					tmpPos = Pos;
					int minMoveTurn = 1e9;

					int size = order.size();

					//配達地点がなかったら終わり
					if (size == 0)break;

					int l1 = xor128.rand() % size, r1 = xor128.rand() % size;
					swap(order[l1], order[r1]);

					float preTargetRadius;
					if (preId < houseCount)preTargetRadius = Parameter::HouseRadius;
					else if (preId > houseCount)preTargetRadius = Parameter::LargeUFORadius;
					else preTargetRadius = Parameter::OfficeRadius;

					int minId = 0;

					//一番近い配達地点へ貪欲に
					for (int j = 0; j < size; ++j) {
						int houseIndex = order[j].index;

						dstPos = aStage.houses()[houseIndex].pos();

						int moveTurn;
						if (preId < houseCount && ufoType) moveTurn = Dist[preId + 1][houseIndex + 1];
						else moveTurn = EasyMoveTurn(tmpPos, ufoRadius, dstPos, Parameter::HouseRadius, ufoSpeed);

						if (pre_moveTurn && moveTurn < minMoveTurn + 1) {
							Pos = adjustPoint(prePos, predstPos, dstPos, pre_moveTurn*ufoSpeed, preTargetRadius + ufoRadius);

							moveTurn = EasyMoveTurn(Pos, ufoRadius, dstPos, Parameter::HouseRadius, ufoSpeed);
						}

						if (moveTurn < minMoveTurn) {
							minMoveTurn = moveTurn;
							minId = j;
						}
						else if (moveTurn == minMoveTurn) {
							if (xor128.rand() % 2)minId = j;
						}

						order[j].turn = moveTurn;
					}

					//大型UFOと小型UFO１つの最初の配達地点はランダム
					if (turn == 0) {
						if (i < Parameter::LargeUFOCount) {
							sort(order.begin(), order.end());
							minId = 0;

							int l = 2, r = size;
							int m;
							while (l < r) {
								m = (l + r) / 2;
								if (turn + order[m].turn + supply_turn > maxTurn)r = m;
								else l = m + 1;
							}
							l /= 1.4;
							int x = xor128.rand() % l;
							swap(order[minId], order[x]);
						}
						else if (i == Parameter::LargeUFOCount + 1) {
							sort(order.begin(), order.end());
							minId = 0;

							int l = 1, r = size;
							int m;
							while (l < r) {
								m = (l + r) / 2;
								if (turn + order[m].turn + supply_turn > maxTurn)r = m;
								else l = m + 1;
							}
							int x = xor128.rand() % l;
							swap(order[minId], order[x]);
						}
					}

					//限界ターン数を超えたらダメ
					if (turn + order[minId].turn + supply_turn > maxTurn)break;
					else {
						//補給地点へ行っていたらその分のターン数を足す
						if ((pathInd[i].size() - supply_cnt + 1) % ufoCapacity == 0)turn += supply_turn;
						supply_turn = 0;

						dstPos = aStage.houses()[order[minId].index].pos();

						if (turn) {
							if (pre_moveTurn) Pos = adjustPoint(prePos, predstPos, dstPos, pre_moveTurn*ufoSpeed, preTargetRadius + ufoRadius);
							else Pos = prePos;
							MoveVector2(prePos, Pos, ufoSpeed, pre_moveTurn);

							if (IsIntersect(prePos, ufoRadius, predstPos, preTargetRadius) == false) {
								Pos = predstPos;
								prePos = tmpPos;
							}
							pathInd[i].emplace_back(preId);
							pathVec[i].emplace_back(Pos);
							tarVec[i].emplace_back(predstPos);
						}

						Pos = prePos;
						predstPos = dstPos;
						preId = order[minId].index;

						pre_moveTurn = MoveVector2(Pos, ufoRadius, dstPos, Parameter::HouseRadius, ufoSpeed);
						turn += pre_moveTurn;

						swap(order[minId], order[size - 1]);
						order.pop_back();

						house_cnt++;

						//小型UFOは5回配達したら一番近い補給地点に行く
						if (ufoType == UFOType_Small && (pathInd[i].size() - supply_cnt + 1) % Parameter::SmallUFOCapacity == 0) {
							tmpPos = Pos;
							minMoveTurn = 1e9;
							//一番近い補給地点へ貪欲に
							vector<TURNsort>targetSUPPLY;
							for (int k = 0; k < Parameter::LargeUFOCount + 1; k++) {
								int moveTurn = MAX_TURN;
								//大型UFO
								if (k) {
									if (LargeUFOVecs[k - 1][maxTurn].box >= Parameter::SmallUFOCapacity) {
										int l = turn, r = maxTurn + 1;
										int m;
										while (l < r) {
											m = (l + r) / 2;
											dstPos = LargeUFOVecs[k - 1][m].Pos;

											moveTurn = EasyMoveTurn(tmpPos, ufoRadius, dstPos, Parameter::LargeUFORadius, ufoSpeed);

											if (pre_moveTurn && max(moveTurn, m - turn) < minMoveTurn + 2) {
												Pos = adjustPoint(prePos, predstPos, dstPos, pre_moveTurn*ufoSpeed, Parameter::HouseRadius + ufoRadius);

												moveTurn = EasyMoveTurn(Pos, ufoRadius, dstPos, Parameter::LargeUFORadius, ufoSpeed);

												minMoveTurn = min(minMoveTurn, max(moveTurn, m - turn));
											}

											if (moveTurn <= m - turn)r = m;
											else l = m + 1;
										}

										moveTurn = r - turn;
									}
								}
								//農場
								else {
									dstPos = aStage.office().pos();

									moveTurn = EasyMoveTurn(tmpPos, ufoRadius, dstPos, Parameter::OfficeRadius, ufoSpeed);

									if (pre_moveTurn && moveTurn < minMoveTurn + 2) {
										Pos = adjustPoint(prePos, predstPos, dstPos, pre_moveTurn*ufoSpeed, Parameter::HouseRadius + ufoRadius);

										moveTurn = EasyMoveTurn(Pos, ufoRadius, dstPos, Parameter::OfficeRadius, ufoSpeed);

										minMoveTurn = min(minMoveTurn, moveTurn);
									}
								}

								targetSUPPLY.emplace_back(TURNsort(moveTurn, k));
							}
							sort(targetSUPPLY.begin(), targetSUPPLY.end());

							if (turn + targetSUPPLY[0].turn > maxTurn)break;

							//大型UFO
							if (targetSUPPLY[0].index) {
								dstPos = LargeUFOVecs[targetSUPPLY[0].index - 1][turn + targetSUPPLY[0].turn].Pos;

								if (pre_moveTurn)Pos = adjustPoint(prePos, predstPos, dstPos, pre_moveTurn*ufoSpeed, Parameter::HouseRadius + ufoRadius);
								else Pos = prePos;
								MoveVector2(prePos, Pos, ufoSpeed, pre_moveTurn);

								if (IsIntersect(prePos, ufoRadius, predstPos, Parameter::HouseRadius) == false) {
									Pos = predstPos;
									prePos = tmpPos;
								}
								pathInd[i].emplace_back(preId);
								pathVec[i].emplace_back(Pos);
								tarVec[i].emplace_back(predstPos);

								Pos = prePos;
								preId = houseCount + targetSUPPLY[0].index;

								supply_turn = MoveVector2(Pos, Parameter::SmallUFORadius, dstPos, Parameter::LargeUFORadius, Parameter::SmallUFOMaxSpeed);

								if (supply_turn <= targetSUPPLY[0].turn)supply_turn = targetSUPPLY[0].turn;

								dstPos = LargeUFOVecs[targetSUPPLY[0].index - 1][turn + supply_turn].Pos;
								Pos = prePos;

								pre_moveTurn = MoveVector2(Pos, Parameter::SmallUFORadius, dstPos, Parameter::LargeUFORadius, Parameter::SmallUFOMaxSpeed);

								predstPos = dstPos;

								for (int t = turn + supply_turn; t < maxTurn + 1; t++)LargeUFOVecs[targetSUPPLY[0].index - 1][t].box -= Parameter::SmallUFOCapacity;
							}
							//農場
							else {
								dstPos = aStage.office().pos();

								if (pre_moveTurn)Pos = adjustPoint(prePos, predstPos, dstPos, pre_moveTurn*ufoSpeed, Parameter::HouseRadius + ufoRadius);
								else Pos = prePos;
								MoveVector2(prePos, Pos, ufoSpeed, pre_moveTurn);

								if (IsIntersect(prePos, ufoRadius, predstPos, Parameter::HouseRadius) == false) {
									Pos = predstPos;
									prePos = tmpPos;
								}
								pathInd[i].emplace_back(preId);
								pathVec[i].emplace_back(Pos);
								tarVec[i].emplace_back(predstPos);

								Pos = prePos;
								predstPos = dstPos;
								preId = houseCount + targetSUPPLY[0].index;

								supply_turn = MoveVector2(Pos, Parameter::SmallUFORadius, dstPos, Parameter::OfficeRadius, Parameter::SmallUFOMaxSpeed);
								pre_moveTurn = supply_turn;
							}
							supply_cnt++;
						}
						//大型UFOは40回配達したら農場へ
						else if (ufoType == UFOType_Large && (pathInd[i].size() - supply_cnt + 1) % Parameter::LargeUFOCapacity == 0) {
							dstPos = aStage.office().pos();

							if (pre_moveTurn)Pos = adjustPoint(prePos, predstPos, dstPos, pre_moveTurn*ufoSpeed, Parameter::HouseRadius + ufoRadius);
							else Pos = prePos;
							MoveVector2(prePos, Pos, ufoSpeed, pre_moveTurn);

							if (IsIntersect(prePos, ufoRadius, predstPos, Parameter::HouseRadius) == false) {
								Pos = predstPos;
								prePos = tmpPos;
							}
							pathInd[i].emplace_back(preId);
							pathVec[i].emplace_back(Pos);
							tarVec[i].emplace_back(predstPos);

							Pos = prePos;
							predstPos = dstPos;
							preId = houseCount;

							supply_turn = MoveVector2(Pos, Parameter::LargeUFORadius, dstPos, Parameter::OfficeRadius, Parameter::LargeUFOMaxSpeed);
							pre_moveTurn = supply_turn;
							supply_cnt++;
						}
					}
				}

				pathInd[i].emplace_back(preId);
				pathVec[i].emplace_back(tmpPos);
				tarVec[i].emplace_back(predstPos);

				max_turn = max(max_turn, turn);


				//大型UFOの位置をターンごとに保存
				if (ufoType == UFOType_Large) {
					Pos = aStage.office().pos();
					int t = 0, box = Parameter::LargeUFOCapacity;
					for (int j = 0; j < (int)pathInd[i].size(); j++) {
						dstPos = pathVec[i][j];

						while (!IsIntersect(Pos, Parameter::LargeUFORadius, tarVec[i][j], Parameter::HouseRadius)) {
							Vector2 dir = dstPos - Pos;

							float length = dir.length();
							float speed = ufoSpeed;
							if (length > speed) {
								dir = dir / length * speed;
							}

							Pos += dir;
							t++;
							LargeUFOVecs[i][t] = UFOINFO(Pos, box);
						}
						box--;
					}

					for (; t < maxTurn + 1; t++)LargeUFOVecs[i][t] = UFOINFO(Pos, box);
				}

			}

			//配達し切れたら限界ターン数を下げる
			if (house_cnt == houseCount) {
				maxTurn = max_turn - 1;

				bestPathInd = pathInd;
				bestPathVec = pathVec;
				bestHouseVec = tarVec;
			}

			loop++;
			tmr.stop();
		}

		//cerr << "loop:" << loop << endl;
		cerr << maxTurn + 2 << endl;

		//ベストな道のり通りに目的地を設定
		for (int ufoIndex = 0; ufoIndex < ufoCount; ++ufoIndex) {
			bestPathInd[ufoIndex].emplace_back(TARGET_HOUSE_NONE);
			reverse(bestPathInd[ufoIndex].begin(), bestPathInd[ufoIndex].end());
			bestPathVec[ufoIndex].emplace_back(aStage.office().pos());
			reverse(bestPathVec[ufoIndex].begin(), bestPathVec[ufoIndex].end());
			bestHouseVec[ufoIndex].emplace_back(aStage.office().pos());
			reverse(bestHouseVec[ufoIndex].begin(), bestHouseVec[ufoIndex].end());

			targetHouseInd[ufoIndex] = bestPathInd[ufoIndex][bestPathInd[ufoIndex].size() - 1];
			targetVec[ufoIndex] = bestPathVec[ufoIndex][bestPathVec[ufoIndex].size() - 1];
			targetHouseVec[ufoIndex] = bestHouseVec[ufoIndex][bestPathVec[ufoIndex].size() - 1];
			if (targetHouseInd[ufoIndex] != TARGET_HOUSE_NONE) {
				bestPathInd[ufoIndex].pop_back();
				bestPathVec[ufoIndex].pop_back();
				bestHouseVec[ufoIndex].pop_back();
			}
		}
	}


	void Answer::moveItems(const Stage& aStage, Actions& aActions)
	{
		//１ターンに複数のアクションが取れるなら取る
		vector<char>supplied(ufoCount);
		bool flag = true;
		while (flag) {
			flag = false;

			for (int ufoIndex = 0; ufoIndex < ufoCount; ++ufoIndex) {
				const auto& ufo = aStage.ufos()[ufoIndex];

				//農場の上にいたら箱を積み込む
				if (Util::IsIntersect(ufo, aStage.office())) {
					aActions.add(Action::PickUp(ufoIndex));

					if (targetHouseInd[ufoIndex] == houseCount) {
						targetHouseInd[ufoIndex] = bestPathInd[ufoIndex][bestPathInd[ufoIndex].size() - 1];
						targetVec[ufoIndex] = bestPathVec[ufoIndex][bestPathVec[ufoIndex].size() - 1];
						targetHouseVec[ufoIndex] = bestHouseVec[ufoIndex][bestPathVec[ufoIndex].size() - 1];
						if (targetHouseInd[ufoIndex] != TARGET_HOUSE_NONE) {
							bestPathInd[ufoIndex].pop_back();
							bestPathVec[ufoIndex].pop_back();
							bestHouseVec[ufoIndex].pop_back();
						}
						flag = true;
						supplied[ufoIndex] = true;
					}
				}
				else {
					for (int k = 0; k < Parameter::LargeUFOCount; k++) {
						if (Util::IsIntersect(ufo, aStage.ufos()[k])) {
							if (targetHouseInd[ufoIndex] == houseCount + k + 1) {
								aActions.add(Action::Pass(k, ufoIndex));

								targetHouseInd[ufoIndex] = bestPathInd[ufoIndex][bestPathInd[ufoIndex].size() - 1];
								targetVec[ufoIndex] = bestPathVec[ufoIndex][bestPathVec[ufoIndex].size() - 1];
								targetHouseVec[ufoIndex] = bestHouseVec[ufoIndex][bestPathVec[ufoIndex].size() - 1];
								if (targetHouseInd[ufoIndex] != TARGET_HOUSE_NONE) {
									bestPathInd[ufoIndex].pop_back();
									bestPathVec[ufoIndex].pop_back();
									bestHouseVec[ufoIndex].pop_back();
								}
								flag = true;
								supplied[ufoIndex] = true;
							}
						}
					}
				}


				// 担当の家の上にいたら配達する

				int houseIndex = targetHouseInd[ufoIndex];
				if (houseIndex == TARGET_HOUSE_NONE || houseIndex >= houseCount) {
					continue;
				}

				if ((ufo.itemCount() == 0 && supplied[ufoIndex] == false) || !Util::IsIntersect(ufo, aStage.houses()[houseIndex])) {
					continue;
				}

				aActions.add(Action::Deliver(ufoIndex, houseIndex));

				targetHouseInd[ufoIndex] = bestPathInd[ufoIndex][bestPathInd[ufoIndex].size() - 1];
				targetVec[ufoIndex] = bestPathVec[ufoIndex][bestPathVec[ufoIndex].size() - 1];
				targetHouseVec[ufoIndex] = bestHouseVec[ufoIndex][bestPathVec[ufoIndex].size() - 1];
				if (targetHouseInd[ufoIndex] != TARGET_HOUSE_NONE) {
					bestPathInd[ufoIndex].pop_back();
					bestPathVec[ufoIndex].pop_back();
					bestHouseVec[ufoIndex].pop_back();
				}
				flag = true;
			}
		}
	}


	void Answer::moveUFOs(const Stage& aStage, TargetPositions& aTargetPositions)
	{
		for (int ufoIndex = 0; ufoIndex < ufoCount; ++ufoIndex) {
			const auto& ufo = aStage.ufos()[ufoIndex];

			int houseIndex = targetHouseInd[ufoIndex];
			Vector2 target = targetVec[ufoIndex];

			float preTargetRadius;
			if (houseIndex < houseCount)preTargetRadius = Parameter::HouseRadius;
			else if (houseIndex > houseCount)preTargetRadius = Parameter::LargeUFORadius;
			else preTargetRadius = Parameter::OfficeRadius;
			Vector2 pos = ufo.pos();
			if (IsIntersect(pos, ufo.radius(), targetHouseVec[ufoIndex], preTargetRadius))houseIndex = TARGET_HOUSE_NONE;

			if (houseIndex != TARGET_HOUSE_NONE)aTargetPositions.add(target);
			else aTargetPositions.add(ufo.pos());
		}
	}


	void Answer::finalize(const Stage& aStage)
	{
	}

}
