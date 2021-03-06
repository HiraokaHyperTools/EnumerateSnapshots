// ConsoleApplication134.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

typedef struct _IO_STATUS_BLOCK {
	union {
		PVOID    Pointer;
		NTSTATUS Status;
	} DUMMYUNIONNAME;
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


typedef NTSTATUS (* ZwFsControlFile)(
	HANDLE           FileHandle,
	HANDLE           Event,
	PVOID            ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG            FsControlCode,
	PVOID            InputBuffer,
	ULONG            InputBufferLength,
	PVOID            OutputBuffer,
	ULONG            OutputBufferLength
);

int main()
{
	// Desired Access:	Read Data/List Directory, Read Attributes, Synchronize
	HANDLE file = CreateFile(L"\\\\DD7\\鵜野", FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES | SYNCHRONIZE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS
		, NULL);
	// FSCTL_SRV_ENUMERATE_SNAPSHOTS
	static BYTE buffer[1048576];
	DWORD cb;
	//BOOL f = DeviceIoControl(file, 0x00144064, NULL, 0, buffer, sizeof(buffer), &cb, NULL);
	ZwFsControlFile pfn = (ZwFsControlFile)GetProcAddress(LoadLibrary(L"ntdll.dll"), "ZwFsControlFile");
	IO_STATUS_BLOCK io = { 0 };
	NTSTATUS r = pfn(file, NULL, NULL, NULL, &io, 0x00144064, NULL, 0, buffer, sizeof(buffer));
	return 0;
}

