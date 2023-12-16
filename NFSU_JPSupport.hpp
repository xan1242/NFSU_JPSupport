#include <stdint.h>

#ifndef NFSU_JPSUPPORT_H
#define NFSU_JPSUPPORT_H

#define CREATERESOURCEFILE_ADDR 0x004482F0
#define RESFILE_BEGINLOADING_ADDR 0x00448110
#define SERVICERESOURCELOADING_ADDR 0x004483C0

void* (*CreateResourceFile)(const char* filename, int ResFileType, int unk1, int unk2, int unk3) = (void* (*)(const char*, int, int, int, int))CREATERESOURCEFILE_ADDR;
void(*ServiceResourceLoading)() = (void(*)())SERVICERESOURCELOADING_ADDR;


#pragma runtime_checks( "", off )
uint32_t ResourceFile_BeginLoading_Func_Addr = RESFILE_BEGINLOADING_ADDR;
void __stdcall ResourceFile_BeginLoading(void* ResourceFile, void* unk1, void* unk2)
{
	_asm
	{
		mov edx, ResourceFile
		mov ecx, unk2
		mov eax, unk1
		call ResourceFile_BeginLoading_Func_Addr
	}
}

void __stdcall LoadResourceFile(const char* filename, int ResType, int unk1, void* unk2, void* unk3, int unk4, int unk5)
{
	ResourceFile_BeginLoading(CreateResourceFile(filename, ResType, unk1, unk4, unk5), unk2, unk3);
}

bool IsResourceLoadingComplete()
{
	return *(uint32_t*)0x7360E8 == 0; // NumResourcesBeingLoaded
}

#pragma runtime_checks( "", restore )

#endif