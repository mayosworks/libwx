//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2005 MAYO.
//=============================================================================

//#define WIN32_LEAN_AND_MEAN

#ifndef STRICT
#define STRICT
#endif //STRICT

#include <windows.h>
#include "wx_misc.h"
#include "wx_dde_client.h"

namespace wx {
namespace {

// DDE定数
const HDDEDATA DDE_OK = reinterpret_cast<HDDEDATA>(TRUE);
const HDDEDATA DDE_NG = reinterpret_cast<HDDEDATA>(TRUE);

// 処理用変数
DWORD				g_inst = 0;
HSZ					g_service = 0;
HSZ					g_topic = 0;
IDDEClientCallback*	g_cb = NULL;

} //namespace


//-----------------------------------------------------------------------------
// DDE初期化
//-----------------------------------------------------------------------------
bool DDEClient::Initialize(PCTSTR service_name, PCTSTR topic_name)
{
	DWORD inst = 0;
	DWORD af_cmd = APPCLASS_STANDARD | CBF_SKIP_ALLNOTIFICATIONS | CBF_FAIL_ADVISES | CBF_FAIL_REQUESTS;

	// DDEを初期化する（標準モード）
	if (DdeInitialize(&inst, DDEProc, af_cmd, 0) != DMLERR_NO_ERROR) {
		return false;
	}

	// サービス名とトピック名の文字列ハンドルを作成する
	HSZ service = DdeCreateStringHandle(inst, service_name, CP_WINUNICODE);
	HSZ topic = DdeCreateStringHandle(inst, topic_name, CP_WINUNICODE);
	if (!service || !topic) {
		DdeUninitialize(inst);
		return false;
	}

	// サービス名をDDEMLに登録する
	HDDEDATA data = DdeNameService(inst, service, 0, DNS_REGISTER);
	if (!data) {
		DdeFreeStringHandle(inst, service);
		DdeFreeStringHandle(inst, topic);
		DdeUninitialize(inst);
		return false;
	}

	g_inst = inst;
	g_service = service;
	g_topic = topic;
	return true;
}

//-----------------------------------------------------------------------------
// DDE終了
//-----------------------------------------------------------------------------
void DDEClient::Finalize()
{
	// DDEサービスの登録を解除する
	if (g_inst) {
		DdeNameService(g_inst, g_service, 0, DNS_UNREGISTER);
		DdeFreeStringHandle(g_inst, g_service);
		DdeFreeStringHandle(g_inst, g_topic);
		DdeUninitialize(g_inst);
	}

	g_inst = 0;
	g_service = 0;
	g_topic = 0;
	g_cb = NULL;
}

//-----------------------------------------------------------------------------
// 有効状態設定
//-----------------------------------------------------------------------------
void DDEClient::SetEnable(bool on)
{
	if (g_inst) {
		DdeEnableCallback(g_inst, NULL, on? EC_ENABLEALL : EC_DISABLE);
	}
}

//-----------------------------------------------------------------------------
// コールバック先設定
//-----------------------------------------------------------------------------
void DDEClient::SetCallback(IDDEClientCallback* cb)
{
	g_cb = cb;
}

//-----------------------------------------------------------------------------
// DDEコールバック
//-----------------------------------------------------------------------------
HDDEDATA DDEClient::DDEProc(UINT type, UINT fmt, HCONV conv,
			HSZ tpc1, HSZ tpc2, HDDEDATA data, DWORD data1, DWORD data2)
{
	WX_UNUSED(fmt);
	WX_UNUSED(conv);
	WX_UNUSED(data1);
	WX_UNUSED(data2);

	switch (type) {
	case XTYP_CONNECT:
		if (tpc1 == g_topic && tpc2 == g_service) {
			return DDE_OK;
		}
		break;

	case XTYP_DISCONNECT:
		return DDE_OK;

	case XTYP_EXECUTE:
		if (tpc1 == g_topic && g_cb) {
			const void* ddeData = DdeAccessData(data, NULL);
			g_cb->FileOpenRequest(static_cast<const wchar_t*>(ddeData));

			DdeUnaccessData(data);
			DdeFreeDataHandle(data);
			return DDE_OK;
		}

		DdeFreeDataHandle(data);
		return DDE_FNOTPROCESSED;

	case XTYP_REQUEST:
	case XTYP_POKE:
		break;
	}

	return DDE_NG;
}

} //namespace wx
