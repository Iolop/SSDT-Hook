#include <ntddk.h>
#include <wdmsec.h>

#pragma warning(disable:4100 4189 4152 4055)
#define SSDT_HOOK (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x84e,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define control_device_object_symbol L"\\?\\cdo_sym_ssdt"
#define SYSTEMSERVICE_INDEX(serviceFunc) (*(PULONG)((PCHAR)serviceFunc + 1))

const GUID ssdt_hook_uuid = { 0xd47bf014L,0x7b37,0x11e7,{0xba,0x6f,0x00,0x0c,0x29,0xf3,0x4e,0xca} };
PDEVICE_OBJECT gdo = NULL;//for IoCreateDeviceSecure use<global device object>

typedef struct _SYSTEM_SERVICE_TABLE
{
	PULONG serviceTable;
	PULONG fidle2;
	ULONG nEntries;
	ULONG argumentTable;
}SYSTEM_SERVICE_TABLE,*PSYSTEM_SERVICE_TABLE;


typedef NTSTATUS(*pZwWriteFile)(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, PVOID, ULONG, PLARGE_INTEGER, PULONG);



VOID disableWP();
VOID enableWP();
VOID UnloadDriver(PDRIVER_OBJECT driver);
NTSTATUS ssdt_hook(PDEVICE_OBJECT device, PIRP pirp);

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING regPath)
{
	DbgBreakPoint();
	NTSTATUS status;
	ULONG i;
	pZwWriteFile oldNtWriteFile = NULL;
	__declspec(dllimport) PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable; //It's exactly the ssdt base addr!

	UNICODE_STRING uZwWriteFile = RTL_CONSTANT_STRING(L"ZwWriteFile");

	UNICODE_STRING sddl = RTL_CONSTANT_STRING(L"D:P(A;;GA;;;WD)");
	UNICODE_STRING control_device_object = RTL_CONSTANT_STRING(L"\\Device\\cdo_ssdt");
	UNICODE_STRING control_device_symbol = RTL_CONSTANT_STRING(control_device_object_symbol);

	status = IoCreateDeviceSecure(driver, 0, &control_device_object, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &sddl, (LPCGUID)&ssdt_hook_uuid, &gdo);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("    [-] IoCreateDeviceSecure error.\n");
		return status;
	}
	/*
	IoDeleteSymbolicLink(&control_device_symbol);
	status = IoCreateSymbolicLink(&control_device_symbol, &control_device_object);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("    [-] IoCreateSymbolicLink error while status=0x%X.\n",status);
		IoDeleteDevice(gdo);
		return status;
	}
	*/
	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driver->MajorFunction[i] = ssdt_hook;
	}
	driver->DriverUnload = UnloadDriver;
	gdo->Flags &= ~DO_DEVICE_INITIALIZING;//clear the init flags

	disableWP();
	oldNtWriteFile = (pZwWriteFile)(((PULONG)KeServiceDescriptorTable)[SYSTEMSERVICE_INDEX(MmGetSystemRoutineAddress(&uZwWriteFile))]);
	enableWP();
	return STATUS_SUCCESS;
}

VOID UnloadDriver(PDRIVER_OBJECT driver)
{
	UNICODE_STRING control_device_symbol = RTL_CONSTANT_STRING(control_device_object_symbol);
	IoDeleteSymbolicLink(&control_device_symbol);
	if (gdo != NULL)
	{
		IoDeleteDevice(gdo);
	}

}

NTSTATUS ssdt_hook(PDEVICE_OBJECT device, PIRP pirp)
{
	return STATUS_SUCCESS;
}

VOID disableWP()
{
	__asm
	{
		cli;
		push eax;
		mov eax, cr0;
		and eax, 0xFFFEFFFF;
		mov cr0, eax;
		pop eax;
	}
}

VOID enableWP()
{
	__asm
	{
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
		sti;
	}
}