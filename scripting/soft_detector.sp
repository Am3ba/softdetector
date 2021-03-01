#pragma semicolon 1
#pragma newdecls required

#include <softdetector>

char	log_path[256];

public void OnPluginStart()
{
	BuildPath(Path_SM, log_path, sizeof(log_path), "logs/softdetector.log");
}

public void SD_Detect(int client, bool game)
{
	LogToFile(log_path, "%L soft detected%s!", client, (game) ? " [ in game injected ] " : "");
	KickClient(client, "На сервере запрещено использовать чит программы и другое дополнительное программное обеспечение");
}