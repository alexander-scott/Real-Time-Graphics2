#include "TextureManager.h"

TextureManager::TextureManager()
{

}

TextureManager::TextureManager(const TextureManager &)
{
}

TextureManager::~TextureManager()
{
}

bool TextureManager::Initialize(int targaCount, int jpgCount)
{
	_textureCount = targaCount + jpgCount;

	//// Create the Colour TargaTexture object.
	//_textureArray = new TargaTexture[_textureCount];
	//if (!_textureArray)
	//{
	//	return false;
	//}

	return true;
}

void TextureManager::Destroy()
{
	int i;

	// Release the TargaTexture objects.
	if (_textureArray.size() > 0)
	{
		for (i = 0; i<_textureCount; i++)
		{
			if (_textureArray[i])
			{
				_textureArray[i]->Destroy();
			}
		}
		//delete[] _textureArray;
		//_textureArray = 0;
	}

	return;
}

bool TextureManager::LoadTargaTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, int location)
{
	bool result;

	// Initialize the Colour TargaTexture object.
	_textureArray.insert(std::make_pair(location, new TargaTexture));
	result = _textureArray[location]->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextureManager::LoadJPEGTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, int location)
{
	bool result;

	// Initialize the JPEG object.
	_textureArray.insert(std::make_pair(location, new JPEGTexture));
	result = _textureArray[location]->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView * TextureManager::GetTexture(int id)
{
	return _textureArray[id]->GetTexture();
}
