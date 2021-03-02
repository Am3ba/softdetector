#if defined _softdetector_included
	#endinput
#endif

#define _softdetector_included

/**
 *
 * @param client			Client index.
 * @param count				Count events.
 */
forward void SD_Detect(int client, int count);

/** Do Not Edit Below This Line **/
public Extension __ext_softdetector =
{
	name = "soft_detector",
	file = "soft_detector.ext",

	#if defined AUTOLOAD_EXTENSIONS
		autoload = 1,
	#else
		autoload = 0,
	#endif

	#if defined REQUIRE_EXTENSIONS
		required = 1,
	#else
		required = 0,
	#endif
};