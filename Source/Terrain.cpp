#include "Terrain.h"

Terrain::Terrain()
{
	_vertexBuffer = 0;
	_indexBuffer = 0;
	_terrainFilename = 0;
	_colourMapFilename = nullptr;
	_heightMap = 0;
	_terrainModel = 0;
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
	result = LoadBitmapHeightMap();
	if (!result)
	{
		return false;
	}

	// Setup the X and Z coordinates for the height map as well as scale the terrain height by the height scale value.
	SetTerrainCoordinates();

	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// Load in the color map for the terrain.
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

	// Calculate the tangent and binormal for the terrain model.
	CalculateTerrainVectors();

	// Load the rendering buffers with the terrain data.
	result = InitializeBuffers(device);
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
	// Release the rendering buffers.
	DestroyBuffers();

	// Release the terrain model.
	DestroyTerrainModel();

	// Release the height map.
	DestroyHeightMap();

	return;
}

bool Terrain::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}

int Terrain::GetIndexCount()
{
	return _indexCount;
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

	// Read up to the color map file name.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the color map file name.
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

	// Start by creating the array structure to hold the height map data.
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
	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
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

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
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

	// Create a temporary array to hold the face normal vectors.
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

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
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

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * _terrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
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


	// Open the color map file in binary.
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

	// Make sure the color map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biWidth != _terrainWidth) || (bitmapInfoHeader.biHeight != _terrainHeight))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.  Since this is non-divide by 2 dimensions (eg. 257x257) need to add extra byte to each line.
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

	// Read the image data into the color map portion of the height map structure.
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

		// Compensate for extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the color map filename now that is has been read in.
	delete[] _colourMapFilename;
	_colourMapFilename = 0;

	return true;
}

bool Terrain::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;

	// Calculate the number of vertices in the 3D terrain model.
	_vertexCount = (_terrainHeight - 1) * (_terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	_terrainModel = new ModelType[_vertexCount];
	if (!_terrainModel)
	{
		return false;
	}

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
			index++;
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

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
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

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
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

	// Calculate the Tu and Tv texture space vectors.
	tuVector[0] = vertex2.Tu - vertex1.Tu;
	tvVector[0] = vertex2.Tv - vertex1.Tv;

	tuVector[1] = vertex3.Tu - vertex1.Tu;
	tvVector[1] = vertex3.Tv - vertex1.Tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.X = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.Y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.Z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.X = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.Y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.Z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of the tangent.
	length = (float)sqrt((tangent.X * tangent.X) + (tangent.Y * tangent.Y) + (tangent.Z * tangent.Z));

	// Normalize the tangent and then store it.
	tangent.X = tangent.X / length;
	tangent.Y = tangent.Y / length;
	tangent.Z = tangent.Z / length;

	// Calculate the length of the binormal.
	length = (float)sqrt((binormal.X * binormal.X) + (binormal.Y * binormal.Y) + (binormal.Z * binormal.Z));

	// Normalize the binormal and then store it.
	binormal.X = binormal.X / length;
	binormal.Y = binormal.Y / length;
	binormal.Z = binormal.Z / length;

	return;
}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i, j, terrainWidth, terrainHeight;
	XMFLOAT4 color;
	float positionX, positionZ;

	// Set the height and width of the terrain grid.
	terrainHeight = 256;
	terrainWidth = 256;

	// Set the color of the terrain grid.
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	
	// Calculate the number of vertices in the terrain.
	_vertexCount = (_terrainWidth - 1) * (_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	_indexCount = _vertexCount;

	// Create the vertex array.
	vertices = new VertexType[_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array and index array with 3D terrain model data.
	for (i = 0; i<_vertexCount; i++)
	{
		vertices[i].Position = XMFLOAT3(_terrainModel[i].X, _terrainModel[i].Y, _terrainModel[i].Z);
		vertices[i].Texture = XMFLOAT2(_terrainModel[i].Tu, _terrainModel[i].Tv);
		vertices[i].Normal = XMFLOAT3(_terrainModel[i].Nx, _terrainModel[i].Ny, _terrainModel[i].Nz);
		vertices[i].Tangent = XMFLOAT3(_terrainModel[i].Tx, _terrainModel[i].Ty, _terrainModel[i].Tz);
		vertices[i].Binormal = XMFLOAT3(_terrainModel[i].Bx, _terrainModel[i].By, _terrainModel[i].Bz);
		vertices[i].Colour = XMFLOAT3(_terrainModel[i].R, _terrainModel[i].G, _terrainModel[i].B);
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void Terrain::DestroyBuffers()
{
	// Release the index buffer.
	if(_indexBuffer)
	{
		_indexBuffer->Release();
		_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(_vertexBuffer)
	{
		_vertexBuffer->Release();
		_vertexBuffer = 0;
	}

	return;
}

void Terrain::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}