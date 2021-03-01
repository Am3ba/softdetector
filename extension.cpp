/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod SD Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"

IForward* g_hDetect;

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */
SD g_SD;		/**< Global singleton for extension's main interface */
SMEXT_LINK(&g_SD);

class CBaseClient : public IGameEventListener2, public IClient {};

CDetour *g_DetourEvents = NULL;

void *DetourAEvents = NULL;

int		g_iEvents[65];
bool	g_bLoaded[65];
bool	g_bDetect[65];

class CGameEventDescriptor;

DETOUR_DECL_MEMBER4(DetourEvents, signed int, CBaseClient *, a1, CGameEventDescriptor *, a2, int, a3, int, a4)
{
	if ( a3 && a3 == 2 )
	{
		META_CONPRINTF("");
		
		if ( a1 && !a1->IsHLTV() && !a1->IsFakeClient() && a1->IsConnected() )
		{
			META_CONPRINTF("");

			int client = a1->GetPlayerSlot() + 1;
			
			g_iEvents[client]++;

			if ( g_bLoaded[client] && g_iEvents[client] != 78 && g_iEvents[client] != 25 )
			{
				if ( !g_bDetect[client] )
				{
					g_hDetect->PushCell(client);
					g_hDetect->PushCell(1);

					g_hDetect->Execute(NULL);

					g_bDetect[client] = true;
				}
			}
		}
	}

	return DETOUR_MEMBER_CALL(DetourEvents)(a1, a2, a3, a4);
}

bool SD::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	CDetourManager::Init(smutils->GetScriptingEngine());
	
	META_CONPRINTF("\n\n\n\n~~~~~~~~~~~~~~~~~~~~ Soft-Detector Loaded\n");
	META_CONPRINTF("~~~~~~~~~~~~~~~~~~~~ https://vk.com/sourcepawn_ru\n\n\n\n");
	
	Dl_info info;
	
	if(dladdr(engine, &info) == 0)
	{
		return false;
	}
	
	void *pEngineSo = dlopen(info.dli_fname, RTLD_NOW);
	if(pEngineSo == NULL)
	{
		return false;
	}
	
	DetourAEvents = memutils->ResolveSymbol(pEngineSo,	"_ZN17CGameEventManager11AddListenerEPvP20CGameEventDescriptori");
	
	if (!DetourAEvents)
	{
		dlclose(pEngineSo);
		return false;
	}

	dlclose(pEngineSo);

	g_DetourEvents = DETOUR_CREATE_MEMBER(DetourEvents, DetourAEvents);
	g_DetourEvents->EnableDetour();
	
	sharesys->RegisterLibrary(myself, "softdetector");
	
	g_hDetect = forwards->CreateForward("SD_Detect", ET_Ignore, 2, NULL, Param_Cell, Param_Cell);
	
	playerhelpers->AddClientListener(&g_SD);
	
	return true;
}

void SD::OnClientPutInServer(int client)
{
	IGamePlayer *pClient = playerhelpers->GetGamePlayer(client);
	
	if ( !pClient || pClient->IsFakeClient() || !pClient->IsConnected() )
		return;
		
	g_bLoaded[client] = true;
		
	if ( g_iEvents[client] == 0 || (g_iEvents[client] > 25 && g_iEvents[client] != 78) )
	{
		g_hDetect->PushCell(client);
		g_hDetect->PushCell(0);
		
		g_hDetect->Execute(NULL);

		g_bDetect[client] = true;
	}
}

void SD::OnClientDisconnecting(int client)
{
	g_iEvents[client] = 0;
	g_bLoaded[client] = false;
	g_bDetect[client] = false;
}

bool SD::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	return true;
}

void SD::SDK_OnUnload()
{
	if (g_DetourEvents)
	{
		g_DetourEvents->Destroy();
		g_DetourEvents = NULL;
	}
	
	forwards->ReleaseForward(g_hDetect);
	
	playerhelpers->RemoveClientListener(&g_SD);
}