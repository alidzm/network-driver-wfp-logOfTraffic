#include <ntifs.h>
#include <fwpsk.h>

DRIVER_INITIALIZE DriverEntry;

HANDLE fileHandle;
OBJECT_ATTRIBUTES objAttr;
UNICODE_STRING fileName;
IO_STATUS_BLOCK ioStatusBlock;

VOID NTAPI OnClassifyForStream(
	__in FWPS_INCOMING_VALUES0 const *pInFixedValuesStream,
	__in FWPS_INCOMING_METADATA_VALUES0 const *pInMetaValuesStream,
	__out VOID *pLayerDataStream,
	__in FWPS_FILTER0 const *pFilterStream,
	__in UINT64 FlowContextStream,
	__out FWPS_CLASSIFY_OUT0 *pClassifyOutStream
);

VOID NTAPI OnClassifyForConnect(
	__in FWPS_INCOMING_VALUES0 const *pInFixedValuesConnect,
	__in FWPS_INCOMING_METADATA_VALUES0 const *pInMetaValuesConnect,
	__out VOID *pLayerDataConnect,
	__in FWPS_FILTER0 const *pFilterConnect,
	__in UINT64 FlowContextConnect,
	__out FWPS_CLASSIFY_OUT0 *pClassifyOutConnect
);

NTSTATUS DriverEntry (_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS Status;
	DEVICE_OBJECT *pDeviceObj;
	FWPS_CALLOUT0 CalloutInitData;

	Status = IoCreateDevice(DriverObject, 0, NULL, FILE_DEVICE_UNKNOWN,	0, FALSE, &pDeviceObj);

	if (!NT_SUCCESS(Status))
	{
		LOG_ERROR("IoCreateDevice failed with status 0x%.81x.\r\n", Status);
	}

	RtlZeroMemory(&CalloutInitData, sizeof(CalloutInitData));

	// {EC8363B2-94FE-457B-ACF3-D92AEE51C04A}
	static const GUID GUID_MyWfpCalloutForStream =
	{ 0xec8363b2, 0x94fe, 0x457b,{ 0xac, 0xf3, 0xd9, 0x2a, 0xee, 0x51, 0xc0, 0x4a } };

	CalloutInitData.calloutKey = GUID_MyWfpCalloutForStream;
	CalloutInitData.flags = 0;
	CalloutInitData.classifyFn = OnClassifyForStream;
	CalloutInitData.notifyFn = OnNotifyForStream;
	CalloutInitData.flowDeleteFn = NULL;

	UINT32 StreamCalloutId;

	Status = FwpsCalloutRegister0(pDeviceObj, &CalloutInitData, &StreamCalloutId);

	//Second FwpsCalloutRegister0

	RtlZeroMemory(&CalloutInitData, sizeof(CalloutInitData));

	// {793B2AF9-5A53-46BE-A003-87617DEFD780}
	static const GUID GUID_MyWfpCalloutForConnect =
	{ 0x793b2af9, 0x5a53, 0x46be,{ 0xa0, 0x3, 0x87, 0x61, 0x7d, 0xef, 0xd7, 0x80 } };

	CalloutInitData.calloutKey = GUID_MyWfpCalloutForConnect;
	CalloutInitData.flags = 0;
	CalloutInitData.classifyFn = OnClassifyForConnect;
	CalloutInitData.notifyFn = OnNotifyForConnect;
	CalloutInitData.flowDeleteFn = NULL;

	UINT32 ConnectCalloutId;

	Status = FwpsCalloutRegister0(pDeviceObj, &CalloutInitData, &ConnectCalloutId);
}


VOID NTAPI OnClassifyForStream(
	__in FWPS_INCOMING_VALUES0 const *pInFixedValuesStream,
	__in FWPS_INCOMING_METADATA_VALUES0 const *pInMetaValuesStream,
	__out VOID *pLayerDataStream,
	__in FWPS_FILTER0 const *pFilterStream,
	__in UINT64 FlowContextStream,
	__out FWPS_CLASSIFY_OUT0 *pClassifyOutStream
)
{
	RtlInitUnicodeString(&fileName, L"\\??\\C:\\Log\\logfile.txt");
	InitializeObjectAttributes(
		&objAttr,
		&fileName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);

	ZwCreateFile(
		&fileHandle,
		GENERIC_WRITE,
		&objAttr,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_WRITE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	ZwClose(fileHandle);
}

VOID NTAPI OnClassifyForConnect(
	__in FWPS_INCOMING_VALUES0 const *pInFixedValuesConnect,
	__in FWPS_INCOMING_METADATA_VALUES0 const *pInMetaValuesConnect,
	__out VOID *pLayerDataConnect,
	__in FWPS_FILTER0 const *pFilterConnect,
	__in UINT64 FlowContextConnect,
	__out FWPS_CLASSIFY_OUT0 *pClassifyOutConnect
)
{
	RtlInitUnicodeString(&fileName, L"\\??\\C:\\Log\\logfile.txt");
	InitializeObjectAttributes(
		&objAttr,
		&fileName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);

	ZwCreateFile(
		&fileHandle,
		GENERIC_WRITE,
		&objAttr,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_WRITE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	UINT64 IpProt = pInFixedValuesConnect->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL].value.uint64;

	if (FWPS_IS_METADATA_FIELD_PRESENT(pInMetaValuesConnect, FWPS_METADATA_FIELD_PROCESS_ID))
	{
		UINT32 ProcessId = pInMetaValuesConnect->processId;
	}

	ZwClose(fileHandle);
}