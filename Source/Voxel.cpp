#include "Voxel.h"

Voxel::Voxel()
{
	m_active = false;
}

Voxel::~Voxel()
{
}

bool Voxel::IsActive()
{
	return m_active;
}

void Voxel::SetActive(bool active)
{
	m_active = active;
}
