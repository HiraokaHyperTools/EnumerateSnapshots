#include "stdafx.h"

typedef struct _IO_STATUS_BLOCK {
	union {
		PVOID    Pointer;
		NTSTATUS Status;
	} DUMMYUNIONNAME;
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

EXTERN_C WINBASEAPI NTSTATUS WINAPI ZwFsControlFile(
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

typedef struct _NT_Trans_Data {
	ULONG NumberOfSnapShots;
	ULONG NumberOfSnapShotsReturned;
	ULONG SnapShotArraySize;
	WCHAR SnapShotMultiSZ[1];
} NT_Trans_Data, *PNT_Trans_Data;

int wmain(int argc, wchar_t **argv)
{
	if (argc < 2)
	{
		fwprintf(stderr, L"EnumerateSnapshots \\\\server\\share\n");
		return 1;
	}
	// Desired Access:	Read Data/List Directory, Read Attributes, Synchronize
	HANDLE file = CreateFile(
		argv[1],
		FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES | SYNCHRONIZE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);
	if (file == INVALID_HANDLE_VALUE) {
		return 1;
	}
	static BYTE buffer[32768];
	IO_STATUS_BLOCK io = { 0 };
	// 2.2.7.2.2.1 FSCTL_SRV_ENUMERATE_SNAPSHOTS Response
	// https://msdn.microsoft.com/en-us/library/ff470025.aspx
#define FSCTL_SRV_ENUMERATE_SNAPSHOTS 0x00144064
	NTSTATUS status = ZwFsControlFile(file, NULL, NULL, NULL, &io, FSCTL_SRV_ENUMERATE_SNAPSHOTS, NULL, 0, buffer, sizeof(buffer));
	if (status != 0) {
		return 1;
	}
	NT_Trans_Data &data = *reinterpret_cast<PNT_Trans_Data>(buffer);
	LPCWSTR currentSnapShot = data.SnapShotMultiSZ;
	for (ULONG x = 0; x < data.NumberOfSnapShotsReturned; x++) {
		wprintf(L"%s\n", currentSnapShot);
		currentSnapShot = wcschr(currentSnapShot, 0) + 1;
	}
	return 0;
}

