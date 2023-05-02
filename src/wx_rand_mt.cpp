//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#include "wx_rand_mt.h"

namespace wx {

// ピリオドパラメータ
#define N			624
#define M			397
#define MATRIX_A	0x9908B0DF		//固定ベクトルＡ
#define UPPER_MASK	0x80000000		//most significant w-r bits
#define LOWER_MASK	0x7FFFFFFF		//least significant r bits

// Temperingパラメータ
#define TEMPERING_MASK_B		0x9D2C5680
#define TEMPERING_MASK_C		0xEFC60000
#define TEMPERING_SHIFT_U(y)	(y >> 11)
#define TEMPERING_SHIFT_S(y)	(y <<  7)
#define TEMPERING_SHIFT_T(y)	(y << 15)
#define TEMPERING_SHIFT_L(y)	(y >> 18)


//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
RandMT::RandMT()
	: m_mti(ARRAY_SIZE)
{
	// デフォルト値：4357 での初期化
	SetSeed(4357);
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
RandMT::~RandMT()
{
}

//-----------------------------------------------------------------------------
// シード設定
//-----------------------------------------------------------------------------
void RandMT::SetSeed(int seed)
{
	for (int i = 0; i < ARRAY_SIZE; i++) {
		m_mt[i] = seed & 0xFFFF0000;
		seed  = 69069 * seed + 1;

		m_mt[i] |= (seed & 0xFFFF0000) >> 16;
		seed   = 69069 * seed + 1;
	}

	// 再生成
	m_mti = ARRAY_SIZE;
	Generate();
}

//-----------------------------------------------------------------------------
// 発生させたランダム値の取得
//-----------------------------------------------------------------------------
int RandMT::GetValue()
{
	if (m_mti >= ARRAY_SIZE) {
		Generate();
	}

	int retVal = m_mtr[m_mti++];
	if (retVal < 0) {
		return -retVal;
	}
	
	return retVal;
}

//-----------------------------------------------------------------------------
// ランダム値の生成（Ｃバージョン）
//-----------------------------------------------------------------------------
void RandMT::Generate()
{
	const int mag01[2] = { 0x0, MATRIX_A };	// mag01[x] = x * MATRIX_A  for x=0,1

	int kk = 0;
	for (; kk < N - M; ++kk) {
		int y = (m_mt[kk] & UPPER_MASK) | (m_mt[kk + 1] & LOWER_MASK);
		m_mt[kk] = m_mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1];
	}

	m_mt[N] = m_mt[0];
	for (; kk < N; ++kk) {
		int y = (m_mt[kk] & UPPER_MASK) | (m_mt[kk + 1] & LOWER_MASK);
		m_mt[kk] = m_mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1];
	}

	for (kk = 0; kk < N; kk++) {
		int y = m_mt[kk];
		y ^= TEMPERING_SHIFT_U(y);
		y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
		y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
		y ^= TEMPERING_SHIFT_L(y);
		m_mtr[kk] = y;
	}

	m_mti = 0;
}

} //namespace wx
