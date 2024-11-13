#include "stdafx.h"


#include "BravoEngine.h"
#include "RTGameInstnace.h"


int main()
{
    std::shared_ptr<BravoEngine> Engine(new BravoEngine("BravoEngine"));
	Engine->Initialize();

	std::shared_ptr<RTGameInstnace> GameInstance = Engine->NewObject<RTGameInstnace>("RTGameInstnace");

	Engine->GameLoop();

    return 0;
}
