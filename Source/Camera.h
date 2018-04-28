#include <directxmath.h>

#include "Transform.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	Transform* GetTransform() { return _transform; }

	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix);

	void RenderBaseViewMatrix();
	void GetBaseViewMatrix(XMMATRIX& viewMatrix);

private:
	XMMATRIX		_viewMatrix, _baseViewMatrix;

	Transform*		_transform;
};