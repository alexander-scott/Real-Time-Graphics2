#include "Texture.h"

Texture::Texture()
{
	_targaData = nullptr;
	_texture = nullptr;
	_textureView = nullptr;
}

Texture::Texture(const Texture& other)
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// Load the targa image data into memory.
	result = LoadTarga(filename, height, width);
	if(!result)
	{
		return false;
	}

	// Setup the description of the Texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty Texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &_texture);
	if(FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the Texture.
	deviceContext->UpdateSubresource(_texture, 0, NULL, _targaData, rowPitch, 0);

	// Setup the shader resource View description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource View for the Texture.
	hResult = device->CreateShaderResourceView(_texture, &srvDesc, &_textureView);
	if(FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this Texture.
	deviceContext->GenerateMips(_textureView);
	
	// Release the targa image data now that the image data has been loaded into the Texture.
	delete [] _targaData;
	_targaData = 0;

	return true;
}

void Texture::Destroy()
{
	// Release the Texture View resource.
	if(_textureView)
	{
		_textureView->Release();
		_textureView = 0;
	}

	// Release the Texture.
	if(_texture)
	{
		_texture->Release();
		_texture = 0;
	}

	// Release the targa data.
	if(_targaData)
	{
		delete [] _targaData;
		_targaData = 0;
	}

	return;
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return _textureView;
}

bool Texture::LoadTarga(char* filename, int& height, int& width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if(bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if(!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	_targaData = new unsigned char[imageSize];
	if(!_targaData)
	{
		return false;
	}
	
	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			_targaData[index + 0] = targaImage[k + 2];  // Red.
			_targaData[index + 1] = targaImage[k + 1];  // Green.
			_targaData[index + 2] = targaImage[k + 0];  // Blue
			_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}
	
	// Release the targa image data now that it was copied into the destination array.
	delete [] targaImage;
	targaImage = 0;

	return true;
}