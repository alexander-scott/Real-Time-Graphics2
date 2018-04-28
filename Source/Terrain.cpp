#include "Terrain.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

Terrain::Terrain()
{
	_terrainFilename = nullptr;
	_colourMapFilename = nullptr;
	_heightMap = nullptr;
	_terrainModel = nullptr;
	_terrainCells = nullptr;
}

Terrain::Terrain(const Terrain& other)
{
}

Terrain::~Terrain()
{
}

bool Terrain::Initialize(ID3D11Device* device, char* setupFilename)
{
	bool result;

	// Get the terrain filename, dimensions, and so forth from the setup file.
	result = LoadSetupFile(setupFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the terrain height map with the data from the bitmap file.
	result = LoadRawHeightMap();
	if (!result)
	{
		return false;
	}

	// Setup the X and Z coordinates for the height map as well as scale the terrain height By the height scale value.
	SetTerrainCoordinates();

	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// Load in the Colour map for the terrain.
	result = LoadColourMap();
	if (!result)
	{
		return false;
	}

	// Now build the 3D model of the terrain.
	result = BuildTerrainModel();
	if (!result)
	{
		return false;
	}

	// We can now release the height map since it is no longer needed in memory once the 3D terrain model has been built.
	DestroyHeightMap();

	// Calculate the Tangent and Binormal for the terrain model.
	CalculateTerrainVectors();

	// Create and load the cells with the terrain data.
	result = LoadTerrainCells(device);
	if (!result)
	{
		return false;
	}

	// Release the terrain model now that the rendering buffers have been loaded.
	DestroyTerrainModel();

	return true;
}

void Terrain::Destroy()
{
	// Release the terrain cells.
	DestroyTerrainCells();

	// Release the terrain model.
	DestroyTerrainModel();

	// Release the height map.
	DestroyHeightMap();

	return;
}

void Terrain::Update()
{
	_renderCount = 0;
	_cellsDrawn = 0;
	_cellsCulled = 0;
	return;
}

bool Terrain::RenderCell(ID3D11DeviceContext* deviceContext, int cellId, Frustum* Frustum)
{
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;
	bool result;

	// Get the dimensions of the terrain cell.
	_terrainCells[cellId].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

	// Check if the cell is visible.  If it is not visible then just return and don't render it.
	result = Frustum->CheckRectangle2(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);
	if (!result)
	{
		// Increment the number of cells that were culled.
		_cellsCulled++;

		return false;
	}

	// If it is visible then render it.
	_terrainCells[cellId].Draw(deviceContext);

	// Add the polygons in the cell to the render count.
	_renderCount += (_terrainCells[cellId].GetVertexCount() / 3);

	// Increment the number of cells that were actually drawn.
	_cellsDrawn++;

	return true;
}

void Terrain::RenderCellLines(ID3D11DeviceContext* deviceContext, int cellId)
{
	_terrainCells[cellId].DrawLineBuffers(deviceContext);
	return;
}

int Terrain::GetCellIndexCount(int cellId)
{
	return _terrainCells[cellId].GetIndexCount();
}

int Terrain::GetCellLinesIndexCount(int cellId)
{
	return _terrainCells[cellId].GetLineBuffersIndexCount();
}

int Terrain::GetCellCount()
{
	return _cellCount;
}

int Terrain::GetRenderCount()
{
	return _renderCount;
}

int Terrain::GetCellsDrawn()
{
	return _cellsDrawn;
}

int Terrain::GetCellsCulled()
{
	return _cellsCulled;
}

bool Terrain::GetHeightAtPosition(float inputX, float inputZ, float& height)
{
	int i, cellId, index;
	float vertex1[3], vertex2[3], vertex3[3];
	bool foundHeight;
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;

	// Loop through all of the terrain cells to find out which one the inputX and inputZ would be inside.
	cellId = -1;
	for (i = 0; i<_cellCount; i++)
	{
		// Get the current cell dimensions.
		_terrainCells[i].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

		// Check to see if the positions are in this cell.
		if ((inputX < maxWidth) && (inputX > minWidth) && (inputZ < maxDepth) && (inputZ > minDepth))
		{
			cellId = i;
			i = _cellCount;
		}
	}

	// If we didn't find a cell then the input Position is off the terrain grid.
	if (cellId == -1)
	{
		return false;
	}

	// If this is the right cell then check all the triangles in this cell to see what the height of the triangle at this Position is.
	for (i = 0; i<(_terrainCells[cellId].GetVertexCount() / 3); i++)
	{
		index = i * 3;

		vertex1[0] = _terrainCells[cellId].VertexList[index].X;
		vertex1[1] = _terrainCells[cellId].VertexList[index].Y;
		vertex1[2] = _terrainCells[cellId].VertexList[index].Z;
		index++;

		vertex2[0] = _terrainCells[cellId].VertexList[index].X;
		vertex2[1] = _terrainCells[cellId].VertexList[index].Y;
		vertex2[2] = _terrainCells[cellId].VertexList[index].Z;
		index++;

		vertex3[0] = _terrainCells[cellId].VertexList[index].X;
		vertex3[1] = _terrainCells[cellId].VertexList[index].Y;
		vertex3[2] = _terrainCells[cellId].VertexList[index].Z;

		// Check to see if this is the polygon we are looking for.
		foundHeight = CheckHeightOfTriangle(inputX, inputZ, height, vertex1, vertex2, vertex3);
		if (foundHeight)
		{
			return true;
		}
	}

	return false;
}

bool Terrain::LoadSetupFile(char * filename)
{
	int stringLength;
	ifstream fin;
	char input;

	// Initialize the string that will hold the terrain file name.
	stringLength = 256;
	_terrainFilename = new char[stringLength];
	if (!_terrainFilename)
	{
		return false;
	}

	_colourMapFilename = new char[stringLength];
	if (!_colourMapFilename)
	{
		return false;
	}

	// Open the setup file.  If it could not open the file then exit.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// Read up to the terrain file name.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain file name.
	fin >> _terrainFilename;

	// Read up to the value of terrain height.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain height.
	fin >> _terrainHeight;

	// Read up to the value of terrain width.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain width.
	fin >> _terrainWidth;

	// Read up to the value of terrain height scaling.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain height scaling.
	fin >> _heightScale;

	// Read up to the Colour map file name.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the Colour map file name.
	fin >> _colourMapFilename;

	// Close the setup file.
	fin.close();

	return true;
}

bool Terrain::LoadBitmapHeightMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;

	// Start By creating the array structure to hold the height map data.
	_heightMap = new HeightMapType[_terrainWidth * _terrainHeight];
	if (!_heightMap)
	{
		return false;
	}

	// Open the bitmap map file in binary.
	error = fopen_s(&filePtr, _terrainFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the bitmap file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != _terrainHeight) || (bitmapInfoHeader.biWidth != _terrainWidth))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.  
	// Since we use non-divide By 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
	imageSize = _terrainHeight * ((_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the Position in the image data buffer.
	k = 0;

	// Read the image data into the height map array.
	for (j = 0; j<_terrainHeight; j++)
	{
		for (i = 0; i<_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (_terrainWidth * (_terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			_heightMap[index].Y = (float)height;

			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide By 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data now that the height map array has been loaded.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the terrain filename now that is has been read in.
	delete[] _terrainFilename;
	_terrainFilename = 0;

	return true;
}

bool Terrain::LoadRawHeightMap()
{
	int error, i, j, index;
	FILE* filePtr;
	unsigned long long imageSize, count;
	unsigned short* rawImage;

	// Create the float array to hold the height map data.
	_heightMap = new HeightMapType[_terrainWidth * _terrainHeight];
	if (!_heightMap)
	{
		return false;
	}

	// Open the 16 bit raw height map file for reading in binary.
	error = fopen_s(&filePtr, _terrainFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Calculate the size of the raw image data.
	imageSize = _terrainHeight * _terrainWidth;

	// Allocate memory for the raw image data.
	rawImage = new unsigned short[imageSize];
	if (!rawImage)
	{
		return false;
	}

	// Read in the raw image data.
	count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Copy the image data into the height map array.
	for (j = 0; j<_terrainHeight; j++)
	{
		for (i = 0; i<_terrainWidth; i++)
		{
			index = (_terrainWidth * j) + i;

			// Store the height at this point in the height map array.
			_heightMap[index].Y = (float)rawImage[index];
		}
	}

	// Release the bitmap image data.
	delete[] rawImage;
	rawImage = 0;

	// Release the terrain filename now that it has been read in.
	delete[] _terrainFilename;
	_terrainFilename = 0;

	return true;
}

void Terrain::DestroyHeightMap()
{
	// Release the height map array.
	if (_heightMap)
	{
		delete[] _heightMap;
		_heightMap = 0;
	}

	return;
}

void Terrain::SetTerrainCoordinates()
{
	int i, j, index;

	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (j = 0; j<_terrainHeight; j++)
	{
		for (i = 0; i<_terrainWidth; i++)
		{
			index = (_terrainWidth * j) + i;

			// Set the X and Z coordinates.
			_heightMap[index].X = (float)i;
			_heightMap[index].Z = -(float)j;

			// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			_heightMap[index].Z += (float)(_terrainHeight - 1);

			// Scale the height.
			_heightMap[index].Y /= _heightScale;
		}
	}

	return;
}

bool Terrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;

	// Create a temporary array to hold the face Normal vectors.
	normals = new VectorType[(_terrainHeight - 1) * (_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j<(_terrainHeight - 1); j++)
	{
		for (i = 0; i<(_terrainWidth - 1); i++)
		{
			index1 = ((j + 1) * _terrainWidth) + i;      // Bottom left vertex.
			index2 = ((j + 1) * _terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * _terrainWidth) + i;          // Upper left vertex.

														// Get three vertices from the face.
			vertex1[0] = _heightMap[index1].X;
			vertex1[1] = _heightMap[index1].Y;
			vertex1[2] = _heightMap[index1].Z;

			vertex2[0] = _heightMap[index2].X;
			vertex2[1] = _heightMap[index2].Y;
			vertex2[2] = _heightMap[index2].Z;

			vertex3[0] = _heightMap[index3].X;
			vertex3[1] = _heightMap[index3].Y;
			vertex3[2] = _heightMap[index3].Z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (_terrainWidth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face Normal.
			normals[index].X = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].Y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].Z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length.
			length = (float)sqrt((normals[index].X * normals[index].X) + (normals[index].Y * normals[index].Y) +
				(normals[index].Z * normals[index].Z));

			// Normalize the final value for this face using the length.
			normals[index].X = (normals[index].X / length);
			normals[index].Y = (normals[index].Y / length);
			normals[index].Z = (normals[index].Z / length);
		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (j = 0; j<_terrainHeight; j++)
	{
		for (i = 0; i<_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].X;
				sum[1] += normals[index].Y;
				sum[2] += normals[index].Z;
			}

			// Bottom right face.
			if ((i<(_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (_terrainWidth - 1)) + i;

				sum[0] += normals[index].X;
				sum[1] += normals[index].Y;
				sum[2] += normals[index].Z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j<(_terrainHeight - 1)))
			{
				index = (j * (_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].X;
				sum[1] += normals[index].Y;
				sum[2] += normals[index].Z;
			}

			// Upper right face.
			if ((i < (_terrainWidth - 1)) && (j < (_terrainHeight - 1)))
			{
				index = (j * (_terrainWidth - 1)) + i;

				sum[0] += normals[index].X;
				sum[1] += normals[index].Y;
				sum[2] += normals[index].Z;
			}

			// Calculate the length of this Normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * _terrainWidth) + i;

			// Normalize the final shared Normal for this vertex and store it in the height map array.
			_heightMap[index].Nx = (sum[0] / length);
			_heightMap[index].Ny = (sum[1] / length);
			_heightMap[index].Nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

bool Terrain::LoadColourMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;


	// Open the Colour map file in binary.
	error = fopen_s(&filePtr, _colourMapFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Make sure the Colour map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biWidth != _terrainWidth) || (bitmapInfoHeader.biHeight != _terrainHeight))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.  Since this is non-divide By 2 dimensions (eg. 257x257) need to add extra byte to each line.
	imageSize = _terrainHeight * ((_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the Position in the image data buffer.
	k = 0;

	// Read the image data into the Colour map portion of the height map structure.
	for (j = 0; j<_terrainHeight; j++)
	{
		for (i = 0; i<_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the array.
			index = (_terrainWidth * (_terrainHeight - 1 - j)) + i;

			_heightMap[index].B = (float)bitmapImage[k] / 255.0f;
			_heightMap[index].G = (float)bitmapImage[k + 1] / 255.0f;
			_heightMap[index].R = (float)bitmapImage[k + 2] / 255.0f;

			k += 3;
		}

		// Compensate for extra byte at end of each line in non-divide By 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the Colour map filename now that is has been read in.
	delete[] _colourMapFilename;
	_colourMapFilename = 0;

	return true;
}

bool Terrain::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;
	float quadsCovered, incrementSize, tu2Left, tu2Right, tv2Bottom, tv2Top;

	// Calculate the number of vertices in the 3D terrain model.
	_vertexCount = (_terrainHeight - 1) * (_terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	_terrainModel = new ModelType[_vertexCount];
	if (!_terrainModel)
	{
		return false;
	}

	// Setup the increment size for the second set of textures.
	// This is a fixed 33x33 vertex array per cell so there will be 32 rows of quads in a cell.
	quadsCovered = 32.0f;
	incrementSize = 1.0f / quadsCovered;

	// Initialize the texture increments.
	tu2Left = 0.0f;
	tu2Right = incrementSize;
	tv2Top = 0.0f;
	tv2Bottom = incrementSize;

	// Initialize the index into the height map array.
	index = 0;

	// Load the 3D terrain model with the height map terrain data.
	// We will be creating 2 triangles for each of the four points in a quad.
	for (j = 0; j<(_terrainHeight - 1); j++)
	{
		for (i = 0; i<(_terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (_terrainWidth * j) + i;          // Upper left.
			index2 = (_terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (_terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.
			_terrainModel[index].X = _heightMap[index1].X;
			_terrainModel[index].Y = _heightMap[index1].Y;
			_terrainModel[index].Z = _heightMap[index1].Z;
			_terrainModel[index].Tu = 0.0f;
			_terrainModel[index].Tv = 0.0f;
			_terrainModel[index].Nx = _heightMap[index1].Nx;
			_terrainModel[index].Ny = _heightMap[index1].Ny;
			_terrainModel[index].Nz = _heightMap[index1].Nz;
			_terrainModel[index].R = _heightMap[index1].R;
			_terrainModel[index].G = _heightMap[index1].G;
			_terrainModel[index].B = _heightMap[index1].B;
			_terrainModel[index].Tu2 = tu2Left;
			_terrainModel[index].Tv2 = tv2Top;
			index++;

			// Triangle 1 - Upper right.
			_terrainModel[index].X = _heightMap[index2].X;
			_terrainModel[index].Y = _heightMap[index2].Y;
			_terrainModel[index].Z = _heightMap[index2].Z;
			_terrainModel[index].Tu = 1.0f;
			_terrainModel[index].Tv = 0.0f;
			_terrainModel[index].Nx = _heightMap[index2].Nx;
			_terrainModel[index].Ny = _heightMap[index2].Ny;
			_terrainModel[index].Nz = _heightMap[index2].Nz;
			_terrainModel[index].R = _heightMap[index2].R;
			_terrainModel[index].G = _heightMap[index2].G;
			_terrainModel[index].B = _heightMap[index2].B;
			_terrainModel[index].Tu2 = tu2Right;
			_terrainModel[index].Tv2 = tv2Top;
			index++;

			// Triangle 1 - Bottom left.
			_terrainModel[index].X = _heightMap[index3].X;
			_terrainModel[index].Y = _heightMap[index3].Y;
			_terrainModel[index].Z = _heightMap[index3].Z;
			_terrainModel[index].Tu = 0.0f;
			_terrainModel[index].Tv = 1.0f;
			_terrainModel[index].Nx = _heightMap[index3].Nx;
			_terrainModel[index].Ny = _heightMap[index3].Ny;
			_terrainModel[index].Nz = _heightMap[index3].Nz;
			_terrainModel[index].R = _heightMap[index3].R;
			_terrainModel[index].G = _heightMap[index3].G;
			_terrainModel[index].B = _heightMap[index3].B;
			_terrainModel[index].Tu2 = tu2Left;
			_terrainModel[index].Tv2 = tv2Bottom;
			index++;

			// Triangle 2 - Bottom left.
			_terrainModel[index].X = _heightMap[index3].X;
			_terrainModel[index].Y = _heightMap[index3].Y;
			_terrainModel[index].Z = _heightMap[index3].Z;
			_terrainModel[index].Tu = 0.0f;
			_terrainModel[index].Tv = 1.0f;
			_terrainModel[index].Nx = _heightMap[index3].Nx;
			_terrainModel[index].Ny = _heightMap[index3].Ny;
			_terrainModel[index].Nz = _heightMap[index3].Nz;
			_terrainModel[index].R = _heightMap[index3].R;
			_terrainModel[index].G = _heightMap[index3].G;
			_terrainModel[index].B = _heightMap[index3].B;
			_terrainModel[index].Tu2 = tu2Left;
			_terrainModel[index].Tv2 = tv2Bottom;
			index++;

			// Triangle 2 - Upper right.
			_terrainModel[index].X = _heightMap[index2].X;
			_terrainModel[index].Y = _heightMap[index2].Y;
			_terrainModel[index].Z = _heightMap[index2].Z;
			_terrainModel[index].Tu = 1.0f;
			_terrainModel[index].Tv = 0.0f;
			_terrainModel[index].Nx = _heightMap[index2].Nx;
			_terrainModel[index].Ny = _heightMap[index2].Ny;
			_terrainModel[index].Nz = _heightMap[index2].Nz;
			_terrainModel[index].R = _heightMap[index2].R;
			_terrainModel[index].G = _heightMap[index2].G;
			_terrainModel[index].B = _heightMap[index2].B;
			_terrainModel[index].Tu2 = tu2Right;
			_terrainModel[index].Tv2 = tv2Top;
			index++;

			// Triangle 2 - Bottom right.
			_terrainModel[index].X = _heightMap[index4].X;
			_terrainModel[index].Y = _heightMap[index4].Y;
			_terrainModel[index].Z = _heightMap[index4].Z;
			_terrainModel[index].Tu = 1.0f;
			_terrainModel[index].Tv = 1.0f;
			_terrainModel[index].Nx = _heightMap[index4].Nx;
			_terrainModel[index].Ny = _heightMap[index4].Ny;
			_terrainModel[index].Nz = _heightMap[index4].Nz;
			_terrainModel[index].R = _heightMap[index4].R;
			_terrainModel[index].G = _heightMap[index4].G;
			_terrainModel[index].B = _heightMap[index4].B;
			_terrainModel[index].Tu2 = tu2Right;
			_terrainModel[index].Tv2 = tv2Bottom;
			index++;

			// Increment the second tu texture coords.
			tu2Left += incrementSize;
			tu2Right += incrementSize;

			// Reset the second tu texture coordinate increments.
			if (tu2Right > 1.0f)
			{
				tu2Left = 0.0f;
				tu2Right = incrementSize;
			}
		}

		// Increment the second tv texture coords.
		tv2Top += incrementSize;
		tv2Bottom += incrementSize;

		// Reset the second tu texture coordinate increments.
		if (tv2Bottom > 1.0f)
		{
			tv2Top = 0.0f;
			tv2Bottom = incrementSize;
		}
	}

	return true;
}

void Terrain::DestroyTerrainModel()
{
	// Release the terrain model data.
	if (_terrainModel)
	{
		delete[] _terrainModel;
		_terrainModel = 0;
	}

	return;
}

void Terrain::CalculateTerrainVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal;


	// Calculate the number of faces in the terrain model.
	faceCount = +_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the Tangent, Binormal, and Normal vectors.
	for (i = 0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the terrain model.
		vertex1.X = _terrainModel[index].X;
		vertex1.Y = _terrainModel[index].Y;
		vertex1.Z = _terrainModel[index].Z;
		vertex1.Tu = _terrainModel[index].Tu;
		vertex1.Tv = _terrainModel[index].Tv;
		vertex1.Nx = _terrainModel[index].Nx;
		vertex1.Ny = _terrainModel[index].Ny;
		vertex1.Nz = _terrainModel[index].Nz;
		index++;

		vertex2.X = _terrainModel[index].X;
		vertex2.Y = _terrainModel[index].Y;
		vertex2.Z = _terrainModel[index].Z;
		vertex2.Tu = _terrainModel[index].Tu;
		vertex2.Tv = _terrainModel[index].Tv;
		vertex2.Nx = _terrainModel[index].Nx;
		vertex2.Ny = _terrainModel[index].Ny;
		vertex2.Nz = _terrainModel[index].Nz;
		index++;

		vertex3.X = _terrainModel[index].X;
		vertex3.Y = _terrainModel[index].Y;
		vertex3.Z = _terrainModel[index].Z;
		vertex3.Tu = _terrainModel[index].Tu;
		vertex3.Tv = _terrainModel[index].Tv;
		vertex3.Nx = _terrainModel[index].Nx;
		vertex3.Ny = _terrainModel[index].Ny;
		vertex3.Nz = _terrainModel[index].Nz;
		index++;

		// Calculate the Tangent and Binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the Tangent and Binormal for this face back in the model structure.
		_terrainModel[index - 1].Tx = tangent.X;
		_terrainModel[index - 1].Ty = tangent.Y;
		_terrainModel[index - 1].Tz = tangent.Z;
		_terrainModel[index - 1].Bx = binormal.X;
		_terrainModel[index - 1].By = binormal.Y;
		_terrainModel[index - 1].Bz = binormal.Z;

		_terrainModel[index - 2].Tx = tangent.X;
		_terrainModel[index - 2].Ty = tangent.Y;
		_terrainModel[index - 2].Tz = tangent.Z;
		_terrainModel[index - 2].Bx = binormal.X;
		_terrainModel[index - 2].By = binormal.Y;
		_terrainModel[index - 2].Bz = binormal.Z;

		_terrainModel[index - 3].Tx = tangent.X;
		_terrainModel[index - 3].Ty = tangent.Y;
		_terrainModel[index - 3].Tz = tangent.Z;
		_terrainModel[index - 3].Bx = binormal.X;
		_terrainModel[index - 3].By = binormal.Y;
		_terrainModel[index - 3].Bz = binormal.Z;
	}

	return;
}

void Terrain::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;

	// Calculate the two vectors for this face.
	vector1[0] = vertex2.X - vertex1.X;
	vector1[1] = vertex2.Y - vertex1.Y;
	vector1[2] = vertex2.Z - vertex1.Z;

	vector2[0] = vertex3.X - vertex1.X;
	vector2[1] = vertex3.Y - vertex1.Y;
	vector2[2] = vertex3.Z - vertex1.Z;

	// Calculate the Tu and Tv TargaTexture space vectors.
	tuVector[0] = vertex2.Tu - vertex1.Tu;
	tvVector[0] = vertex2.Tv - vertex1.Tv;

	tuVector[1] = vertex3.Tu - vertex1.Tu;
	tvVector[1] = vertex3.Tv - vertex1.Tv;

	// Calculate the denominator of the Tangent/Binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply By the coefficient to get the Tangent and Binormal.
	tangent.X = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.Y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.Z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.X = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.Y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.Z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of the Tangent.
	length = (float)sqrt((tangent.X * tangent.X) + (tangent.Y * tangent.Y) + (tangent.Z * tangent.Z));

	// Normalize the Tangent and then store it.
	tangent.X = tangent.X / length;
	tangent.Y = tangent.Y / length;
	tangent.Z = tangent.Z / length;

	// Calculate the length of the Binormal.
	length = (float)sqrt((binormal.X * binormal.X) + (binormal.Y * binormal.Y) + (binormal.Z * binormal.Z));

	// Normalize the Binormal and then store it.
	binormal.X = binormal.X / length;
	binormal.Y = binormal.Y / length;
	binormal.Z = binormal.Z / length;

	return;
}

bool Terrain::LoadTerrainCells(ID3D11Device * device)
{
	int cellHeight, cellWidth, cellRowCount, i, j, index;
	bool result;

	// Set the height and width of each terrain cell to a fixed 33x33 vertex array.
	cellHeight = 33;
	cellWidth = 33;

	// Calculate the number of cells needed to store the terrain data.
	cellRowCount = (_terrainWidth - 1) / (cellWidth - 1);
	_cellCount = cellRowCount * cellRowCount;

	// Create the terrain cell array.
	_terrainCells = new TerrainCell[_cellCount];
	if (!_terrainCells)
	{
		return false;
	}

	// Loop through and initialize all the terrain cells.
	for (j = 0; j<cellRowCount; j++)
	{
		for (i = 0; i<cellRowCount; i++)
		{
			index = (cellRowCount * j) + i;

			result = _terrainCells[index].Initialize(device, _terrainModel, i, j, cellHeight, cellWidth, _terrainWidth);
			if (!result)
			{
				return false;
			}
		}
	}

	return true;
}

void Terrain::DestroyTerrainCells()
{
	int i;

	// Release the terrain cell array.
	if (_terrainCells)
	{
		for (i = 0; i<_cellCount; i++)
		{
			_terrainCells[i].Destroy();
		}

		delete[] _terrainCells;
		_terrainCells = 0;
	}

	return;
}

bool Terrain::CheckHeightOfTriangle(float x, float z, float& height, float v0[3], float v1[3], float v2[3])
{
	float startVector[3], directionVector[3], edge1[3], edge2[3], normal[3];
	float Q[3], e1[3], e2[3], e3[3], edgeNormal[3], temp[3];
	float magnitude, D, denominator, numerator, t, determinant;

	// Starting Position of the ray that is being cast.
	startVector[0] = x;
	startVector[1] = 0.0f;
	startVector[2] = z;

	// The direction the ray is being cast.
	directionVector[0] = 0.0f;
	directionVector[1] = -1.0f;
	directionVector[2] = 0.0f;

	// Calculate the two edges from the three points given.
	edge1[0] = v1[0] - v0[0];
	edge1[1] = v1[1] - v0[1];
	edge1[2] = v1[2] - v0[2];

	edge2[0] = v2[0] - v0[0];
	edge2[1] = v2[1] - v0[1];
	edge2[2] = v2[2] - v0[2];

	// Calculate the Normal of the triangle from the two edges.
	normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
	normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
	normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

	magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

	// Find the distance from the origin to the plane.
	D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));

	// Get the denominator of the equation.
	denominator = ((normal[0] * directionVector[0]) + (normal[1] * directionVector[1]) + (normal[2] * directionVector[2]));

	// Make sure the result doesn't get too close to zero to prevent divide By zero.
	if (fabs(denominator) < 0.0001f)
	{
		return false;
	}

	// Get the numerator of the equation.
	numerator = -1.0f * (((normal[0] * startVector[0]) + (normal[1] * startVector[1]) + (normal[2] * startVector[2])) + D);

	// Calculate where we intersect the triangle.
	t = numerator / denominator;

	// Find the intersection vector.
	Q[0] = startVector[0] + (directionVector[0] * t);
	Q[1] = startVector[1] + (directionVector[1] * t);
	Q[2] = startVector[2] + (directionVector[2] * t);

	// Find the three edges of the triangle.
	e1[0] = v1[0] - v0[0];
	e1[1] = v1[1] - v0[1];
	e1[2] = v1[2] - v0[2];

	e2[0] = v2[0] - v1[0];
	e2[1] = v2[1] - v1[1];
	e2[2] = v2[2] - v1[2];

	e3[0] = v0[0] - v2[0];
	e3[1] = v0[1] - v2[1];
	e3[2] = v0[2] - v2[2];

	// Calculate the Normal for the first edge.
	edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
	edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
	edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v0[0];
	temp[1] = Q[1] - v0[1];
	temp[2] = Q[2] - v0[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if (determinant > 0.001f)
	{
		return false;
	}

	// Calculate the Normal for the second edge.
	edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
	edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
	edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v1[0];
	temp[1] = Q[1] - v1[1];
	temp[2] = Q[2] - v1[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if (determinant > 0.001f)
	{
		return false;
	}

	// Calculate the Normal for the third edge.
	edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
	edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
	edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v2[0];
	temp[1] = Q[1] - v2[1];
	temp[2] = Q[2] - v2[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if (determinant > 0.001f)
	{
		return false;
	}

	// Now we have our height.
	height = Q[1];

	return true;
}