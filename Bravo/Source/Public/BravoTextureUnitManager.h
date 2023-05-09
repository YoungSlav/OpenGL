#pragma once


class BravoTextureUnitManager
{
public:
	BravoTextureUnitManager();
	static int32 BindTexture();
	static void UnbindTexture(int32 TextureUnit);

protected:
	static BravoTextureUnitManager& Instance()
	{
		static BravoTextureUnitManager UnitSelector;
		return UnitSelector;
	}

	int32 FindEmptyTextureUnit();

private:
	std::vector<bool> TextureUnits;
};