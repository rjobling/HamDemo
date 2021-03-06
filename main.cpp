////////////////////////////////////////////////////////////////////////////////
// main.cpp
////////////////////////////////////////////////////////////////////////////////

#include "core.h"
#include "ham.h"
#include "system.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int main()
{
	if (System_Init())
	{
		if (Ham_Init())
		{
			while (!System_TestLMB())
			{
				Ham_Update();
			}

			Ham_Deinit();
		}

		System_Deinit();
	}
}
