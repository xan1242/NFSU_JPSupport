#include <windows.h>
#include "includes\injector\injector.hpp"
#include "includes\injector\assembly.hpp"
#include "includes\IniReader.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <cctype>
#include "NFSU_JPSupport.hpp"

bool bForceJPMovies = false;
uintptr_t ptrLoadGlobalAChunks = 0;

enum eLanguages
{
	eLANGUAGE_NONE = -1,
	eLANGUAGE_FIRST,
	eLANGUAGE_ENGLISH = eLANGUAGE_FIRST,
	eLANGUAGE_FRENCH,
	eLANGUAGE_GERMAN,
	eLANGUAGE_ITALIAN,
	eLANGUAGE_SPANISH,
	eLANGUAGE_DUTCH,
	eLANGUAGE_SWEDISH,
	eLANGUAGE_KOREAN,
	eLANGUAGE_CHINESE,
	eLANGUAGE_JAPANESE,
	eLANGUAGE_MAX
};

enum eBuildRegion
{
    BUILD_REGION_AMERICA = 0,
    BUILD_REGION_EU = 1,
    BUILD_REGION_KOREA = 2,
    BUILD_REGION_JAPAN = 3,
    BUILD_REGION_TAIWAN = 4,
    BUILD_REGION_UK = 5,
    BUILD_REGION_FRANCE = 6,
    BUILD_REGION_GERMANY = 7,
    BUILD_REGION_ITALY = 8,
    BUILD_REGION_SPAIN = 9,
    BUILD_REGION_RUSSIA = 10,
    BUILD_REGION_ROA = 11,
    BUILD_REGION_CHINA = 12,
    BUILD_REGION_THAILAND = 13,
    BUILD_REGION_SOUTHAMERICA = 14,
    BUILD_REGION_ASIA = 11,
    BUILD_REGION_EUROPE = 1,
};

const char* pathLangJpn = "LANGUAGES\\JAPANESE.BIN";

const char* pathLangEngJTune = "LANGUAGES\\JTUNE\\ENGLISH.BIN";
const char* pathLangFreJTune = "LANGUAGES\\JTUNE\\FRENCH.BIN";
const char* pathLangGerJTune = "LANGUAGES\\JTUNE\\GERMAN.BIN";
const char* pathLangItaJTune = "LANGUAGES\\JTUNE\\ITALIAN.BIN";
const char* pathLangSpaJTune = "LANGUAGES\\JTUNE\\SPANISH.BIN";
const char* pathLangDutJTune = "LANGUAGES\\JTUNE\\DUTCH.BIN";
const char* pathLangSweJTune = "LANGUAGES\\JTUNE\\SWEDISH.BIN";
const char* pathLangKorJTune = "LANGUAGES\\JTUNE\\KOREAN.BIN";
const char* pathLangChiJTune = "LANGUAGES\\JTUNE\\CHINESE.BIN";
const char* pathLangJpnJTune = "LANGUAGES\\JTUNE\\JAPANESE.BIN";
const char* pathLangTexJTune = "LANGUAGES\\JTUNE\\LANGUAGETEXTURES.BIN";

uint32_t hashLangJpn[28] =
{
    0x15192F5F,
    0xA87927BE,
    0x743BF1C1,
    0xBBBA71C2,
    0,
    0,
    0,
    0,
    0x9583AA1A,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0xAB6215C1,
    0x920075C,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

std::unordered_map<std::string, std::string> movieMap = 
{
    {"01_day"   , "jp01_day"},
    {"02_fan_s" , "jp02fans"},
    {"02_fan_e" , "jp02fane"},
    {"04_gum_s" , "jp04gums"},
    {"05_eng"   , "jp05_eng"},
    {"06_sam"   , "jp06_sam"},
    {"07_god"   , "jp07_god"},
    {"08_edd"   , "jp08_edd"},
    {"09_gbe"   , "jp09_gbe"},
    {"11_pnk"   , "jp11_pnk"},
    {"12_bud"   , "jp12_bud"},
    {"13_kit"   , "jp13_kit"},
    {"14_mut"   , "jp14_mut"},
    {"15_dik"   , "jp15_dik"},
    {"16_res"   , "jp16_res"},
    {"17_met"   , "jp17_met"},
    {"19_fin"   , "jp19_fin"},
    {"drift"    , "jp_drift"},
    {"eng_drag" , "jpn_drag"}
};


const char* CalcMovieName(char* inMovieName)
{
    if ((*(eLanguages*)0x70430C == eLANGUAGE_JAPANESE) || bForceJPMovies)
    {
        std::string mvn = inMovieName;

        // convert to lowercase
        std::transform(mvn.begin(), mvn.end(), mvn.begin(), [](unsigned char c) { return std::tolower(c); });

        if (movieMap.find(mvn) != movieMap.end())
            return movieMap[mvn].c_str();
    }

    return reinterpret_cast<char* (__cdecl*)(char*)>(0x59FD70)(inMovieName);
}

uintptr_t hkHondaLogoHook2_ExitTrue = 0x4C2B29;
uintptr_t hkHondaLogoHook2_ExitFalse = 0x4C2B16;

__declspec(naked) void hkHondaLogoHook2()
{
    _asm
    {
        mov ecx, ds:[0x734998]
        cmp ecx, 3 // BUILD_REGION_JAPAN
        jne hkHondaFalse
        cmp edx, 0xB
        je hkHondaTrue
        cmp edx, 0xE
        je hkHondaTrue
    hkHondaFalse:
        mov ecx, ds:[0x734998]
        jmp hkHondaLogoHook2_ExitFalse
        

    hkHondaTrue:
        push esi
        jmp hkHondaLogoHook2_ExitTrue
    }
}

uintptr_t hkHondaLogoHook3_ExitTrue = 0x49E544;
uintptr_t hkHondaLogoHook3_ExitFalse = 0x49E4B4;

__declspec(naked) void hkHondaLogoHook3()
{
    _asm
    {
        mov ecx, ds:[0x734998]
        cmp ecx, 3 // BUILD_REGION_JAPAN
        jne hkHondaFalse2
        cmp edx, 0xB
        je hkHondaTrue2
        cmp edx, 0xE
        je hkHondaTrue2
    hkHondaFalse2:
        cmp edx, 0xF
        mov ecx, 0x6B9172
        jmp hkHondaLogoHook3_ExitFalse
        

    hkHondaTrue2:
        jmp hkHondaLogoHook3_ExitTrue
    }
}

void Init()
{
    CIniReader inireader("");

    *(eLanguages*)0x70430C = (eLanguages)inireader.ReadInteger("MAIN", "OverrideLang", eLANGUAGE_JAPANESE);
    *(eBuildRegion*)0x734998 = (eBuildRegion)inireader.ReadInteger("MAIN", "OverrideBuildRegion", BUILD_REGION_JAPAN);
    bForceJPMovies = inireader.ReadInteger("MAIN", "ForceJPMovies", 1) != 0;

    bool bUseJTuneLang = inireader.ReadInteger("MAIN", "UseJTuneLang", 1) != 0;
    if (bUseJTuneLang)
    {
        injector::WriteMemory<const char*>(0x6EE658, pathLangEngJTune, true);
        injector::WriteMemory<const char*>(0x6EE668, pathLangFreJTune, true);
        injector::WriteMemory<const char*>(0x6EE678, pathLangGerJTune, true);
        injector::WriteMemory<const char*>(0x6EE688, pathLangItaJTune, true);
        injector::WriteMemory<const char*>(0x6EE698, pathLangSpaJTune, true);
        injector::WriteMemory<const char*>(0x6EE6A8, pathLangDutJTune, true);
        injector::WriteMemory<const char*>(0x6EE6B8, pathLangSweJTune, true);
        injector::WriteMemory<const char*>(0x6EE6C8, pathLangKorJTune, true);
        injector::WriteMemory<const char*>(0x6EE6D8, pathLangChiJTune, true);
        injector::WriteMemory<const char*>(0x6EE6E8, pathLangJpnJTune, true);

        injector::WriteMemory<const char*>(0x59F781 + 1, pathLangTexJTune, true);
        injector::WriteMemory<const char*>(0x59F790 + 1, pathLangTexJTune, true);
        injector::WriteMemory<const char*>(0x59F7AB + 1, pathLangTexJTune, true);
        injector::WriteMemory<const char*>(0x59F9EF + 1, pathLangTexJTune, true);
        injector::WriteMemory<const char*>(0x59FA07 + 1, pathLangTexJTune, true);
    }
    else
        injector::WriteMemory<const char*>(0x6EE6E8, pathLangJpn, true);

	injector::WriteMemory<uint32_t*>(0x6EE6EC, hashLangJpn, true);
    injector::MakeCALL(0x4C3AE7, CalcMovieName, true);
    injector::MakeCALL(0x4DEAED, CalcMovieName, true);
    injector::MakeCALL(0x4E8472, CalcMovieName, true);
    injector::MakeCALL(0x4E884D, CalcMovieName, true);


    struct HondaLogoHook1
    {
        void operator()(injector::reg_pack& regs)
        {
            uint32_t cartype = *(uint32_t*)(regs.eax + 0xC50);
            if (cartype == 11 || cartype == 14)
            {
                regs.ecx = 0x6C56B0; // "HONDA"
            }
        }
    }; 
    injector::MakeInline<HondaLogoHook1>(0x4C3341, 0x4C334F);
    injector::MakeInline<HondaLogoHook1>(0x4BC62D, 0x4BC63B);
    injector::MakeInline<HondaLogoHook1>(0x4C4451, 0x4C445F);
    injector::MakeInline<HondaLogoHook1>(0x4C68E7, 0x4C68F5);
    injector::MakeInline<HondaLogoHook1>(0x4C699F, 0x4C69AD);


    injector::MakeJMP(0x4C2B10, hkHondaLogoHook2);
    injector::MakeRangedNOP(0x49E4AC, 0x49E4B3);
    injector::MakeJMP(0x49E4AC, hkHondaLogoHook3);

    if ((*(eBuildRegion*)0x734998 != BUILD_REGION_AMERICA) || (*(eBuildRegion*)0x734998 != BUILD_REGION_UK))
    {
        // set default speedo units to km/h
        *(uint32_t*)0x006F1D80 = 0; // SkipFESpeedoMode
    }
}


BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		Init();
	}
	return TRUE;
}

