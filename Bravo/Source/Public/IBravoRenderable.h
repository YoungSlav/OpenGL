#pragma once

class IBravoRenderable
{
	friend class BravoEngine;

public:
	IBravoRenderable() = default;
	IBravoRenderable(int32 _RenderPriority) : RenderPriority(_RenderPriority) {}

	void SetRenderPriority(int32 _RenderPriority) { RenderPriority = _RenderPriority; }
	int32 GetRenderPriority() const { return RenderPriority; }

	void SetCastShadows(bool _bCastShadows) { bCastShadows = _bCastShadows; }
	bool GetCastShadows() const { return bCastShadows; }

protected:
	virtual void Render() = 0;
	virtual void RenderSelection() {};
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) {}
	
private:
	int32 RenderPriority = 0;
	bool bCastShadows = false;
};
