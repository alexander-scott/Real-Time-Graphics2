#pragma once

enum BlockType
{
	BlockType_Default = 0,

	BlockType_Grass,
	BlockType_Dirt,
	BlockType_Water,
	BlockType_Stone,
	BlockType_Wood,
	BlockType_Sand,

	BlockType_NumTypes,
};

class Voxel
{
public:
	Voxel();
	~Voxel();

	bool IsActive();
	void SetActive(bool active);

private:
	bool m_active;

	BlockType m_blockType;
};