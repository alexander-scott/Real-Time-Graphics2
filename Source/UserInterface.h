#ifndef _USERINTERFACECLASS_H_
#define _USERINTERFACECLASS_H_

#include "Text.h"

class UserInterface
{
public:
	UserInterface();
	UserInterface(const UserInterface&);
	~UserInterface();

	bool Initialize(DX11Instance*, int, int);
	void Shutdown();

	bool Frame(ID3D11DeviceContext*, int, float, float, float, float, float, float);
	bool Render(DX11Instance*, ShaderManager*, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	bool UpdateFpsString(ID3D11DeviceContext*, int);
	bool UpdatePositionStrings(ID3D11DeviceContext*, float, float, float, float, float, float);

private:
	Font* m_Font1;
	Text *m_FpsString, *m_VideoStrings, *m_PositionStrings;
	int m_previousFps;
	int m_previousPosition[6];
};

#endif