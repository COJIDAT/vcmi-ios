/*
 * main.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "VCAI.h"

#ifdef __GNUC__
#define strcpy_s(a, b, c) strncpy(a, c, b)
#endif

#if defined(VCMI_IOS)
#define GetGlobalAiVersion VCAI_GetGlobalAiVersion
#define GetAiName VCAI_GetAiName
#define GetNewAI VCAI_GetNewAI
#endif

static const char *g_cszAiName = "VCAI";

extern "C" DLL_EXPORT int GetGlobalAiVersion()
{
	return AI_INTERFACE_VER;
}

extern "C" DLL_EXPORT void GetAiName(char* name)
{
	strcpy_s(name, strlen(g_cszAiName) + 1, g_cszAiName);
}

extern "C" DLL_EXPORT void GetNewAI(std::shared_ptr<CGlobalAI> &out)
{
	out = std::make_shared<VCAI>();
}
