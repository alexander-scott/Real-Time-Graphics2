#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <directxmath.h>

#include "Transform.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	Transform* GetTransform() { return _transform; }

	void Render();
	void GetViewMatrix(XMMATRIX&);

	void RenderBaseViewMatrix();
	void GetBaseViewMatrix(XMMATRIX&);

private:
	XMMATRIX		_viewMatrix, _baseViewMatrix;

	Transform*		_transform;
};

#endif