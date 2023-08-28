#include "stdafx.h"
#include "BravoDepthMap.h"
#include "BravoShaderAsset.h"


void BravoDepthMap::OnDestroy()
{
	if ( DepthMapShader )
		DepthMapShader->ReleaseFromGPU();
	DepthMapShader.reset();

	BravoObject::OnDestroy();
}