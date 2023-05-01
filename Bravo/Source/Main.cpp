#include "stdafx.h"


#include "BravoEngine.h"
#include "BravoGameInstance.h"


int main()
{
    std::shared_ptr<BravoEngine> Engine(new BravoEngine());
	Engine->Init();

	std::shared_ptr<BravoGameInstance> inst = Engine->SpawnObject<BravoGameInstance>();
	Engine->StartGame();

    return 0;
}
