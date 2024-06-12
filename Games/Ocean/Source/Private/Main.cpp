#include "stdafx.h"


#include "BravoEngine.h"


int main()
{
    std::shared_ptr<BravoEngine> Engine(new BravoEngine("BravoEngine"));
	Engine->Initialize();

	std::shared_ptr<BravoGameInstance> GameInstance = Engine->NewObject<BravoGameInstance>("BravoGameInstance");

	Engine->GameLoop();

    return 0;
}
