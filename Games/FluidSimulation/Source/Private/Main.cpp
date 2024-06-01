#include "stdafx.h"


#include "BravoEngine.h"
#include "FluidGameInstance.h"


int main()
{
    std::shared_ptr<BravoEngine> Engine(new BravoEngine("BravoEngine"));
	Engine->Initialize();

	std::shared_ptr<FluidGameInstance> GameInstance = Engine->NewObject<FluidGameInstance>("FluidGameInstance");

	Engine->GameLoop();

    return 0;
}
