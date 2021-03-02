#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <softdetector>

char		log_path[256];
int			pCount[MAXPLAYERS+1];

public void OnPluginStart()
{
	BuildPath(Path_SM, log_path, sizeof(log_path), "logs/softdetector.log");
}

public void SD_Detect(int client, int count)
{
	if ( count > 25 && count < 78 )
	{
		LogToFile(log_path, "%L soft detected!", client);
		KickClient(client, "На сервере запрещено использовать чит программы и другое дополнительное программное обеспечение");
	}
	
	pCount[client] = count;
}

public void OnClientPutInServer(int client)
{
	if ( !IsFakeClient(client) )
	{
		/*
			????????????????????
			net_blockmsg svc_GameEventList
		*/
		if ( pCount[client] == 0 )
		{
			LogToFile(log_path, "%L soft detected!", client);
			KickClient(client, "На сервере запрещено использовать чит программы и другое дополнительное программное обеспечение");
		}
	}
}

public void OnClientDisconnect(int client)
{
	pCount[client] = 0;
}