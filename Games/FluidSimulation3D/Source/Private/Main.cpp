#include "stdafx.h"


#include "BravoEngine.h"
#include "FluidGameInstance3D.h"
#include "BravoGameInstance.h"


int main()
{
    std::shared_ptr<BravoEngine> Engine(new BravoEngine("BravoEngine"));
	Engine->Initialize();

	std::shared_ptr<FluidGameInstance3D> GameInstance = Engine->NewObject<FluidGameInstance3D>("FluidGameInstance3D");

	Engine->GameLoop();

    return 0;
}
