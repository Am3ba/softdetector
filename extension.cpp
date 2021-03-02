#include "extension.h"

SD g_SD;
SMEXT_LINK(&g_SD);

IForward* g_hDetect;

class CBaseClient : public IGameEventListener2, public IClient {};

CDetour *g_DetourEvents = NULL;
void *DetourAEvents = NULL;

class CLC_ListenEvents
{
public:
	char nop[16]; // спасибо JDW
	CBitVec<MAX_EVENT_NUMBER> m_EventArray;
};

DETOUR_DECL_MEMBER1(DetourEvents, bool, CLC_ListenEvents*, msg)
{
	int client = (reinterpret_cast<CBaseClient*>(this))->GetPlayerSlot() + 1;
	IGamePlayer *pClient = playerhelpers->GetGamePlayer(client);
	
	if (pClient->IsFakeClient()) return DETOUR_MEMBER_CALL(DetourEvents)(msg);

	int count = 0;

	for (int i = 0; i < MAX_EVENT_NUMBER; i++)
		if (msg->m_EventArray.Get(i))
			count++;
	
	g_hDetect->PushCell(client);
	g_hDetect->PushCell(count);
	g_hDetect->Execute(NULL);
	
	return DETOUR_MEMBER_CALL(DetourEvents)(msg);
}

bool SD::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	CDetourManager::Init(smutils->GetScriptingEngine());
	
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
	
	DetourAEvents = memutils->ResolveSymbol(pEngineSo,	"_ZN11CBaseClient19ProcessListenEventsEP16CLC_ListenEvents");
	
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
	
	META_CONPRINTF("\n\n\n\n~~~~~~~~~~~~~~~~~~~~ Soft-Detector Loaded\n");
	META_CONPRINTF("~~~~~~~~~~~~~~~~~~~~ https://vk.com/sourcepawn_ru\n\n\n\n");
	
	return true;
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