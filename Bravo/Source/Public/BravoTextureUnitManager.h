#pragma once


class BravoTextureUnitManager
{
public:
	BravoTextureUnitManager();
	static int BindTexture();
	static void UnbindTexture(int TextureUnit);

protected:
	static BravoTextureUnitManager& Instance()
	{
		static BravoTextureUnitManager UnitSelector;
		return UnitSelector;
	}

	int FindEmptyTextureUnit();

private:
	std::vector<bool> TextureUnits;
};