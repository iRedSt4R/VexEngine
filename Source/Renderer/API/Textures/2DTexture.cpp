#include "2DTexture.h"

// Texture2D Cache
Texture2D* Texture2DCache::CreateTexture2D(std::wstring pathToTexture, bool bMarkAsSRGB)
{
	if (m_textureCache.find(pathToTexture) != m_textureCache.end())
		return m_textureCache.at(pathToTexture);

	Texture2D* tex2D = new Texture2D(m_device);
	tex2D->CreateFromFile(m_cmdList, pathToTexture, bMarkAsSRGB);

	m_textureCache[pathToTexture] = tex2D;

	return tex2D;
}

