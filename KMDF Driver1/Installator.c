#include <ntifs.h>
#include <fwpmk.h>
#include <winerror.h>
#include <ws2def.h>

int main()
{
	FWPM_SESSION0 Session;
	HANDLE hEngine;

	RtlZeroMemory(&Session, sizeof(Session));

	Session.displayData.name = L"MyWfpSession";
	Session.displayData.description = L"My WFP Session";

	DWORD Status = FwpmEngineOpen0(NULL, RPC_C_AUTHN_DEFAULT, NULL, &Session, &hEngine);

	if (Status != ERROR_SUCCESS)
	{
		THROW_EXC("FwpmEngineOpen0 failed with status 0x%.81x.", Status);
	}

	Status = FwpmTransactionBegin0(hEngine, NULL);

	if (Status != ERROR_SUCCESS)
	{
		THROW_EXC("FwpmTransactionBegin0 failed with status 0x%.81x.", Status);

		FwpmEngineClose0(hEngine);
	}

	// {4FCF7EC2-B982-471E-A189-A7866199EA16}
	static const GUID GUID_MyWfpProvider =
	{ 0x4fcf7ec2, 0xb982, 0x471e,{ 0xa1, 0x89, 0xa7, 0x86, 0x61, 0x99, 0xea, 0x16 } };

	FWPM_PROVIDER0 Provider;

	RtlZeroMemory(&Provider, sizeof(Provider));

	Provider.providerKey = GUID_MyWfpProvider;
	Provider.displayData.name = L"MyWfpProvider";
	Provider.displayData.description = L"My WFP Provider for test application";
	Provider.flags = FWPM_PROVIDER_FLAG_PERSISTENT;

	Status = FwpmProviderAdd0(hEngine, &Provider, NULL);

	if (Status != ERROR_SUCCESS)
	{
		THROW_EXC("FwpmProviderAdd0 failed with status 0x%.81x.", Status);

		DWORD TransactionStatus = FwpmTransactionAbort0(hEngine);

		if (TransactionStatus != ERROR_SUCCESS)
		{
			FwpmEngineClose0(hEngine);
		}
	}

	FWPM_SUBLAYER0 Sublayer;

	RtlZeroMemory(&Sublayer, sizeof(Sublayer));

	// {A92173F5-CE0B-4DAE-96EA-7270E6CE929F}
	static const GUID GUID_MyWfpSublayer =
	{ 0xa92173f5, 0xce0b, 0x4dae,{ 0x96, 0xea, 0x72, 0x70, 0xe6, 0xce, 0x92, 0x9f } };

	Sublayer.subLayerKey = GUID_MyWfpSublayer;
	Sublayer.displayData.name = L"MyWfpSublayer";
	Sublayer.displayData.description = L"My WFP Sublayer";
	Sublayer.flags = FWPM_SUBLAYER_FLAG_PERSISTENT;
	Sublayer.providerKey = (GUID *)&GUID_MyWfpProvider;
	Sublayer.weight = 0x100;

	Status = FwpmSubLayerAdd0(hEngine, &Sublayer, NULL);

	if (Status != ERROR_SUCCESS)
	{
		THROW_EXC("FwpmSubLayerAdd0 failed with status 0x%.81x.", Status);

		DWORD TransactionStatus = FwpmTransactionAbort0(hEngine);

		if (TransactionStatus != ERROR_SUCCESS)
		{
			FwpmProviderDeleteByKey0(hEngine, &GUID_MyWfpProvider);
			FwpmEngineClose0(hEngine);
		}
	}

	FWPM_CALLOUT0 Callout;

	RtlZeroMemory(&Callout, sizeof(Callout));

	// {EC8363B2-94FE-457B-ACF3-D92AEE51C04A}
	static const GUID GUID_MyWfpCalloutForStream =
	{ 0xec8363b2, 0x94fe, 0x457b,{ 0xac, 0xf3, 0xd9, 0x2a, 0xee, 0x51, 0xc0, 0x4a } };

	Callout.calloutKey = GUID_MyWfpCalloutForStream;
	Callout.displayData.name = L"MyWfpStreamCallout";
	Callout.displayData.description = L"My WFP Stream Callout";
	Callout.flags = FWPM_CALLOUT_FLAG_PERSISTENT;
	Callout.providerKey = (GUID *)&GUID_MyWfpProvider;
	Callout.applicableLayer = FWPM_LAYER_STREAM_V4;

	UINT32 CalloutIdForStream;

	Status = FwpmCalloutAdd0(hEngine, &Callout, NULL, &CalloutIdForStream);

	if (Status != ERROR_SUCCESS)
	{
		THROW_EXC("FwpmCalloutAdd0 failed with status 0x%.81x.", Status);

		DWORD TransactionStatus = FwpmTransactionAbort0(hEngine);

		if (TransactionStatus != ERROR_SUCCESS)
		{
			FwpmSubLayerDeleteByKey0(hEngine, &GUID_MyWfpSublayer);
			FwpmProviderDeleteByKey0(hEngine, &GUID_MyWfpProvider);
			FwpmEngineClose0(hEngine);
		}
	}

	RtlZeroMemory(&Callout, sizeof(Callout));

	// {793B2AF9-5A53-46BE-A003-87617DEFD780}
	static const GUID GUID_MyWfpCalloutForConnect =
	{ 0x793b2af9, 0x5a53, 0x46be,{ 0xa0, 0x3, 0x87, 0x61, 0x7d, 0xef, 0xd7, 0x80 } };

	Callout.calloutKey = GUID_MyWfpCalloutForConnect;
	Callout.displayData.name = L"MyWfpConnectCallout";
	Callout.displayData.description = L"My WFP Connection Callout";
	Callout.flags = FWPM_CALLOUT_FLAG_PERSISTENT;
	Callout.providerKey = (GUID *)&GUID_MyWfpProvider;
	Callout.applicableLayer = FWPM_LAYER_ALE_AUTH_CONNECT_V4;

	UINT32 CalloutIdForConnect;

	Status = FwpmCalloutAdd0(hEngine, &Callout, NULL, &CalloutIdForConnect);

	if (Status != ERROR_SUCCESS)
	{
		THROW_EXC("FwpmCalloutAdd0 failed with status 0x%.81x.", Status);

		DWORD TransactionStatus = FwpmTransactionAbort0(hEngine);

		if (TransactionStatus != ERROR_SUCCESS)
		{
			FwpmSubLayerDeleteByKey0(hEngine, &GUID_MyWfpSublayer);
			FwpmProviderDeleteByKey0(hEngine, &GUID_MyWfpProvider);
			FwpmEngineClose0(hEngine);
		}
	}

	FWPM_FILTER_CONDITION0 Condition;

	Condition.fieldKey = FWPM_CONDITION_DIRECTION;
	Condition.matchType = FWP_MATCH_EQUAL;
	Condition.conditionValue.type = FWP_UINT8;
	Condition.conditionValue.uint8 = IPPROTO_TCP;

	FWPM_FILTER0 Filter;
	RtlZeroMemory(&Filter, sizeof(Filter));

	// {759ED93A-6755-4C3C-A78A-6592CED9CB26}
	static const GUID GUID_MyWfpStreamFilter =
	{ 0x759ed93a, 0x6755, 0x4c3c,{ 0xa7, 0x8a, 0x65, 0x92, 0xce, 0xd9, 0xcb, 0x26 } };


	Filter.providerKey = (GUID *)&GUID_MyWfpProvider;
	Filter.displayData.name = L"MyWfpStreamFilter";
	Filter.displayData.description = L"My WFP Stream Filter";
	Filter.flags = FWPM_FILTER_FLAG_PERSISTENT;
	Filter.layerKey = FWPM_LAYER_ALE_AUTH_CONNECT_V4;
	Filter.subLayerKey = GUID_MyWfpSublayer;
	Filter.weight.type = FWP_UINT8;
	Filter.numFilterConditions = 1;
	Filter.filterCondition = &Condition;
	Filter.action.type = FWP_ACTION_CALLOUT_INSPECTION;
	Filter.action.calloutKey = GUID_MyWfpCalloutForStream;
	Filter.filterKey = GUID_MyWfpStreamFilter;
	Filter.weight.uint8 = 1;

	UINT64 FilterIdForStream;

	Status = FwpmFilterAdd0(hEngine, &Filter, NULL, &FilterIdForStream);
	if (Status != ERROR_SUCCESS)
	{
		THROW_EXC("FwpmFilterAdd0 failed with status 0x%.81x.", Status);

		DWORD TransactionStatus = FwpmTransactionAbort0(hEngine);

		if (TransactionStatus != ERROR_SUCCESS)
		{
			FwpmCalloutDeleteByKey0(hEngine, &GUID_MyWfpStreamFilter);
			FwpmSubLayerDeleteByKey0(hEngine, &GUID_MyWfpSublayer);
			FwpmProviderDeleteByKey0(hEngine, &GUID_MyWfpProvider);
			FwpmEngineClose0(hEngine);
		}
	}

	// {00E36EB5-BB41-4A76-87E1-39A7BFB0DED9}
	static const GUID GUID_MyWfpConnectFilter =
	{ 0xe36eb5, 0xbb41, 0x4a76,{ 0x87, 0xe1, 0x39, 0xa7, 0xbf, 0xb0, 0xde, 0xd9 } };

	Filter.providerKey = (GUID *)&GUID_MyWfpProvider;
	Filter.displayData.name = L"MyWfpConnectFilter";
	Filter.displayData.description = L"My WFP Connection Filter";
	Filter.flags = FWPM_FILTER_FLAG_PERSISTENT;
	Filter.layerKey = FWPM_LAYER_STREAM_V4;
	Filter.subLayerKey = GUID_MyWfpSublayer;
	Filter.weight.type = FWP_UINT8;
	Filter.numFilterConditions = 1;
	Filter.filterCondition = &Condition;
	Filter.action.type = FWP_ACTION_CALLOUT_INSPECTION;
	Filter.action.calloutKey = GUID_MyWfpCalloutForConnect;
	Filter.filterKey = GUID_MyWfpConnectFilter;
	Filter.weight.uint8 = 1;

	UINT64 FilterIdForConnect;

	Status = FwpmFilterAdd0(hEngine, &Filter, NULL, &FilterIdForConnect);
	if (Status != ERROR_SUCCESS)
	{
		THROW_EXC("FwpmFilterAdd0 failed with status 0x%.81x.", Status);

		DWORD TransactionStatus = FwpmTransactionAbort0(hEngine);

		if (TransactionStatus != ERROR_SUCCESS)
		{
			FwpmCalloutDeleteByKey0(hEngine, &GUID_MyWfpConnectFilter);
			FwpmSubLayerDeleteByKey0(hEngine, &GUID_MyWfpSublayer);
			FwpmProviderDeleteByKey0(hEngine, &GUID_MyWfpProvider);
			FwpmEngineClose0(hEngine);
		}
	}

	Status = FwpmTransactionCommit0(hEngine);

	if (Status != ERROR_SUCCESS)
	{
		THROW_EXC("FwpmTransactionCommit0 failed with status 0x%.81x.", Status);
	}
}