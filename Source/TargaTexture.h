#include "ITexture.h"

class TargaTexture : public ITexture
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TargaTexture();
	TargaTexture(const TargaTexture&);
	~TargaTexture();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*) override;
	void Destroy() override;

private:
	bool LoadTarga(char*, int&, int&);

private:
	unsigned char*				_targaData;
	ID3D11Texture2D*			_texture;
};