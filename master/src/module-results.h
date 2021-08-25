struct ModuleResults
{
	byte strikes = 0;
	byte deactivatedModules = 0;
	byte sounds[32];
	byte soundCount = 0;

	ModuleResults()
	{
		memset(this->sounds, 0, sizeof(this->sounds));
	}
};