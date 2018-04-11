#include "TextureManager.h"

TextureManager::TextureManager()
{
	_textureArray = nullptr;
}

TextureManager::TextureManager(const TextureManager &)
{
}

TextureManager::~TextureManager()
{
}

bool TextureManager::Initialize(int count)
{
	_textureCount = count;

	// Create the Colour TargaTexture object.
	_textureArray = new TargaTexture[_textureCount];
	if (!_textureArray)
	{
		return false;
	}

	return true;
}

void TextureManager::Destroy()
{
	int i;

	// Release the TargaTexture objects.
	if (_textureArray)
	{
		for (i = 0; i<_textureCount; i++)
		{
			_textureArray[i].Destroy();
		}
		delete[] _textureArray;
		_textureArray = 0;
	}

	return;
}

bool TextureManager::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, int location)
{
	bool result;

	// Initialize the Colour TargaTexture object.
	result = _textureArray[location].Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView * TextureManager::GetTexture(int id)
{
	return _textureArray[id].GetTexture();
}
