#include "stdafx.h"


#include "BravoEngine.h"
#include "BravoGameInstance.h"


int main()
{
    std::shared_ptr<BravoEngine> Engine(new BravoEngine("BravoEngine"));
	Engine->Initialize();

	std::shared_ptr<BravoGameInstance> inst = Engine->SpawnGameInstance<BravoGameInstance>("Debug Game");

	Engine->GameLoop();

    return 0;
}
