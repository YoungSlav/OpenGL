#pragma once

class IBravoRenderable
{
	friend class BravoEngine;

public:
	IBravoRenderable() = default;
	IBravoRenderable(int32 _RenderPriority) : RenderPriority(_RenderPriority) {}

	void SetRenderPriority(int32 _RenderPriority) { RenderPriority = _RenderPriority; }
	int32 GetRenderPriority() const { return RenderPriority; }


	void SetOutlined(bool _Outlined) { Outlined = _Outlined; if ( !Outlined) OutlinedIDs.clear(); }
	bool GetOutlined() const { return Outlined; }
	void SetOutlinedIDs(const std::vector<int32>& _OutlinedIDs) { OutlinedIDs = _OutlinedIDs; }
	const std::vector<int32>& GetOutlinedIDs() const { return OutlinedIDs; }
	void SetOutlineColor(const glm::vec3& _OutlineColor) { OutlineColor = _OutlineColor; }
	const glm::vec3& GetOutlineColor() const { return OutlineColor; }

	void SetCastShadows(bool _bCastShadows) { bCastShadows = _bCastShadows; }
	bool GetCastShadows() const { return bCastShadows; }

protected:
	virtual void Render() = 0;
	virtual void RenderSelectionID() {};
	virtual void RenderOutline_1stPass() {};
	virtual void RenderOutline_2ndPass() {};
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) {}
	
private:
	int32 RenderPriority = 0;
	bool bCastShadows = false;
	
	bool Outlined = false;
	std::vector<int32> OutlinedIDs;
	glm::vec3 OutlineColor = glm::vec3(1.0f, 1.0f, 0.0f);
};
