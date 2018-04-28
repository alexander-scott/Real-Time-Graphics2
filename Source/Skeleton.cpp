#include "Skeleton.h"

#include <iostream>
#include <fstream>

Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{
}

bool Skeleton::Initialize(ID3D11Device * device, ID3D11DeviceContext* context, TextureManager* textureManager, std::wstring meshFileName, std::wstring animFileName)
{
	if (!LoadMD5Model(device, context, meshFileName, _md5Model, textureManager))
		return false;

	if (!LoadMD5Anim(animFileName, _md5Model))
		return false;

	_transform = new Transform;

	return true;
}

void Skeleton::Destroy()
{
	for (int i = 0; i < _md5Model.NumSubsets; i++)
	{
		_md5Model.Subsets[i].IndexBuff->Release();
		_md5Model.Subsets[i].VertBuff->Release();
	}
}

void Skeleton::Update(ID3D11DeviceContext* context, float deltaTime)
{
	int animation = 0;

	_md5Model.Animations[animation].CurrAnimTime += deltaTime;            // Update the current animation time

	if (_md5Model.Animations[animation].CurrAnimTime > _md5Model.Animations[animation].TotalAnimTime)
		_md5Model.Animations[animation].CurrAnimTime = 0.0f;

	// Which frame are we on
	float currentFrame = _md5Model.Animations[animation].CurrAnimTime * _md5Model.Animations[animation].FrameRate;
	int frame0 = (int)floorf(currentFrame);
	int frame1 = frame0 + 1;

	// Make sure we don't go over the number of frames    
	if (frame0 == _md5Model.Animations[animation].NumFrames - 1)
		frame1 = 0;

	float interpolation = currentFrame - frame0;    // Get the remainder (in time) between frame0 and frame1 to use as interpolation factor

	std::vector<Joint> interpolatedSkeleton;        // Create a frame skeleton to store the interpolated skeletons in

	// Compute the interpolated skeleton
	for (int i = 0; i < _md5Model.Animations[animation].NumJoints; i++)
	{
		Joint tempJoint;
		Joint joint0 = _md5Model.Animations[animation].FrameSkeleton[frame0][i];        // Get the i'th joint of frame0's skeleton
		Joint joint1 = _md5Model.Animations[animation].FrameSkeleton[frame1][i];        // Get the i'th joint of frame1's skeleton

		tempJoint.ParentID = joint0.ParentID;                                            // Set the tempJoints parent id

																						 // Turn the two quaternions into XMVECTORs for easy computations
		XMVECTOR joint0Orient = XMVectorSet(joint0.Orientation.x, joint0.Orientation.y, joint0.Orientation.z, joint0.Orientation.w);
		XMVECTOR joint1Orient = XMVectorSet(joint1.Orientation.x, joint1.Orientation.y, joint1.Orientation.z, joint1.Orientation.w);

		// Interpolate positions
		tempJoint.Postion.x = joint0.Postion.x + (interpolation * (joint1.Postion.x - joint0.Postion.x));
		tempJoint.Postion.y = joint0.Postion.y + (interpolation * (joint1.Postion.y - joint0.Postion.y));
		tempJoint.Postion.z = joint0.Postion.z + (interpolation * (joint1.Postion.z - joint0.Postion.z));

		// Interpolate orientations using spherical interpolation (Slerp)
		XMStoreFloat4(&tempJoint.Orientation, XMQuaternionSlerp(joint0Orient, joint1Orient, interpolation));

		interpolatedSkeleton.push_back(tempJoint);        // Push the joint back into our interpolated skeleton
	}

	for (int k = 0; k < _md5Model.NumSubsets; k++)
	{
		for (int i = 0; i < (int)_md5Model.Subsets[k].Vertices.size(); ++i)
		{
			Vertex tempVert = _md5Model.Subsets[k].Vertices[i];
			tempVert.Pos = XMFLOAT3(0, 0, 0);    // Make sure the vertex's pos is cleared first
			tempVert.Normal = XMFLOAT3(0, 0, 0);    // Clear vertices normal

			// Sum up the joints and weights information to get vertex's position and normal
			for (int j = 0; j < tempVert.WeightCount; ++j)
			{
				Weight tempWeight = _md5Model.Subsets[k].Weights[tempVert.StartWeight + j];
				Joint tempJoint = interpolatedSkeleton[tempWeight.JointID];

				// Convert joint orientation and weight pos to vectors for easier computation
				XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.Orientation.x, tempJoint.Orientation.y, tempJoint.Orientation.z, tempJoint.Orientation.w);
				XMVECTOR tempWeightPos = XMVectorSet(tempWeight.Position.x, tempWeight.Position.y, tempWeight.Position.z, 0.0f);

				// We will need to use the conjugate of the joint orientation quaternion
				XMVECTOR tempJointOrientationConjugate = XMQuaternionInverse(tempJointOrientation);

				// Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
				// We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"
				XMFLOAT3 rotatedPoint;
				XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

				// Now move the verices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
				tempVert.Pos.x += (tempJoint.Postion.x + rotatedPoint.x) * tempWeight.Bias;
				tempVert.Pos.y += (tempJoint.Postion.y + rotatedPoint.y) * tempWeight.Bias;
				tempVert.Pos.z += (tempJoint.Postion.z + rotatedPoint.z) * tempWeight.Bias;

				// Compute the normals for this frames skeleton using the weight normals from before
				// We can comput the normals the same way we compute the vertices position, only we don't have to translate them (just rotate)
				XMVECTOR tempWeightNormal = XMVectorSet(tempWeight.Normal.x, tempWeight.Normal.y, tempWeight.Normal.z, 0.0f);

				// Rotate the normal
				XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightNormal), tempJointOrientationConjugate));

				// Add to vertices normal and ake weight bias into account
				tempVert.Normal.x -= rotatedPoint.x * tempWeight.Bias;
				tempVert.Normal.y -= rotatedPoint.y * tempWeight.Bias;
				tempVert.Normal.z -= rotatedPoint.z * tempWeight.Bias;
			}

			_md5Model.Subsets[k].Positions[i] = tempVert.Pos;                // Store the vertices position in the position vector instead of straight into the vertex vector
			_md5Model.Subsets[k].Vertices[i].Normal = tempVert.Normal;        // Store the vertices normal
			XMStoreFloat3(&_md5Model.Subsets[k].Vertices[i].Normal, XMVector3Normalize(XMLoadFloat3(&_md5Model.Subsets[k].Vertices[i].Normal)));
		}

		// Put the positions into the vertices for this subset
		for (int i = 0; i < (int)_md5Model.Subsets[k].Vertices.size(); i++)
		{
			_md5Model.Subsets[k].Vertices[i].Pos = _md5Model.Subsets[k].Positions[i];
		}

		// Update the subsets vertex buffer
		// First lock the buffer
		D3D11_MAPPED_SUBRESOURCE mappedVertBuff;
		context->Map(_md5Model.Subsets[k].VertBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertBuff);

		// Copy the data into the vertex buffer.
		memcpy(mappedVertBuff.pData, &_md5Model.Subsets[k].Vertices[0], (sizeof(Vertex) * _md5Model.Subsets[k].Vertices.size()));

		context->Unmap(_md5Model.Subsets[k].VertBuff, 0);

		// The line below is another way to update a buffer. You will use this when you want to update a buffer less
		// than once per frame, since the GPU reads will be faster (the buffer was created as a DEFAULT buffer instead
		// of a DYNAMIC buffer), and the CPU writes will be slower. You can try both methods to find out which one is faster
		// for you. if you want to use the line below, you will have to create the buffer with D3D11_USAGE_DEFAULT instead
		// of D3D11_USAGE_DYNAMIC
		//d3d11DevCon->UpdateSubresource( MD5Model.subsets[k].vertBuff, 0, NULL, &MD5Model.subsets[k].vertices[0], 0, 0 );
	}
}

void Skeleton::Draw(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	///***Draw MD5 Model***///
	for (int i = 0; i < _md5Model.NumSubsets; i++)
	{
		//Set the grounds index buffer
		deviceContext->IASetIndexBuffer(_md5Model.Subsets[i].IndexBuff, DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		deviceContext->IASetVertexBuffers(0, 1, &_md5Model.Subsets[i].VertBuff, &stride, &offset);
	}
}

void Skeleton::DrawSubset(ID3D11DeviceContext * deviceContext, int index)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	//Set the grounds index buffer
	deviceContext->IASetIndexBuffer(_md5Model.Subsets[index].IndexBuff, DXGI_FORMAT_R32_UINT, 0);
	//Set the grounds vertex buffer
	deviceContext->IASetVertexBuffers(0, 1, &_md5Model.Subsets[index].VertBuff, &stride, &offset);
}

int Skeleton::GetIndexCount(int index)
{
	return _md5Model.Subsets[index].NumTriangles * 3;
}

bool Skeleton::LoadMD5Model(ID3D11Device* device,
	ID3D11DeviceContext* context,
	std::wstring filename,
	Model3D& MD5Model,
	TextureManager* textureManager)
{
	int meshCount = 0;

	std::wifstream fileIn(filename.c_str());        // Open file

	std::wstring checkString;                        // Stores the next string from our file

	if (fileIn)                                        // Check if the file was opened
	{
		while (fileIn)                                // Loop until the end of the file is reached
		{
			fileIn >> checkString;                    // Get next string from file

			if (checkString == L"MD5Version")        // Get MD5 version (this function supports version 10)
			{
				/*fileIn >> checkString;
				MessageBox(0, checkString.c_str(),    //display message
				L"MD5Version", MB_OK);*/
			}
			else if (checkString == L"commandline")
			{
				std::getline(fileIn, checkString);    // Ignore the rest of this line
			}
			else if (checkString == L"numJoints")
			{
				fileIn >> MD5Model.NumJoints;        // Store number of joints
			}
			else if (checkString == L"numMeshes")
			{
				fileIn >> MD5Model.NumSubsets;        // Store number of meshes or subsets which we will call them
			}
			else if (checkString == L"joints")
			{
				Joint tempJoint;

				fileIn >> checkString;                // Skip the "{"

				for (int i = 0; i < MD5Model.NumJoints; i++)
				{
					fileIn >> tempJoint.Name;        // Store joints name
													 // Sometimes the names might contain spaces. If that is the case, we need to continue
													 // to read the name until we get to the closing " (quotation marks)
					if (tempJoint.Name[tempJoint.Name.size() - 1] != '"')
					{
						wchar_t checkChar;
						bool jointNameFound = false;
						while (!jointNameFound)
						{
							checkChar = fileIn.get();

							if (checkChar == '"')
								jointNameFound = true;

							tempJoint.Name += checkChar;
						}
					}

					fileIn >> tempJoint.ParentID;    // Store Parent joint's ID

					fileIn >> checkString;            // Skip the "("

													  // Store position of this joint (swap y and z axis if model was made in RH Coord Sys)
					fileIn >> tempJoint.Postion.x >> tempJoint.Postion.z >> tempJoint.Postion.y;

					fileIn >> checkString >> checkString;    // Skip the ")" and "("

															 // Store orientation of this joint
					fileIn >> tempJoint.Orientation.x >> tempJoint.Orientation.z >> tempJoint.Orientation.y;

					// Remove the quotation marks from joints name
					tempJoint.Name.erase(0, 1);
					tempJoint.Name.erase(tempJoint.Name.size()-1, 1);

					// Compute the w axis of the quaternion (The MD5 model uses a 3D vector to describe the
					// direction the bone is facing. However, we need to turn this into a quaternion, and the way
					// quaternions work, is the xyz values describe the axis of rotation, while the w is a value
					// between 0 and 1 which describes the angle of rotation)
					float t = 1.0f - (tempJoint.Orientation.x * tempJoint.Orientation.x)
						- (tempJoint.Orientation.y * tempJoint.Orientation.y)
						- (tempJoint.Orientation.z * tempJoint.Orientation.z);
					if (t < 0.0f)
					{
						tempJoint.Orientation.w = 0.0f;
					}
					else
					{
						tempJoint.Orientation.w = -sqrtf(t);
					}

					std::getline(fileIn, checkString);        // Skip rest of this line

					MD5Model.Joints.push_back(tempJoint);    // Store the joint into this models joint vector
				}

				fileIn >> checkString;                    // Skip the "}"
			}
			else if (checkString == L"mesh")
			{
				ModelSubset subset;
				int numVerts, numTris, numWeights;

				fileIn >> checkString;                    // Skip the "{"

				fileIn >> checkString;
				while (checkString != L"}")            // Read until '}'
				{
					// In this lesson, for the sake of simplicity, we will assume a textures filename is givin here.
					// Usually though, the name of a material (stored in a material library. Think back to the lesson on
					// loading .obj files, where the material library was contained in the file .mtl) is givin. Let this
					// be an exercise to load the material from a material library such as obj's .mtl file, instead of
					// just the texture like we will do here.
					if (checkString == L"shader")        // Load the texture or material
					{
						std::wstring fileNamePath;
						fileIn >> fileNamePath;            // Get texture's filename

						// Take spaces into account if filename or material name has a space in it
						if (fileNamePath[fileNamePath.size() - 1] != '"')
						{
							wchar_t checkChar;
							bool fileNameFound = false;
							while (!fileNameFound)
							{
								checkChar = fileIn.get();

								if (checkChar == '"')
									fileNameFound = true;

								fileNamePath += checkChar;
							}
						}

						// Remove the quotation marks from texture path
						fileNamePath.erase(0, 1);
						fileNamePath.erase(fileNamePath.size() - 1, 1);

						textureManager->LoadJPEGTexture(device, context, fileNamePath, 10 + meshCount);
						meshCount++;

						std::getline(fileIn, checkString);                // Skip rest of this line
					}
					else if (checkString == L"numverts")
					{
						fileIn >> numVerts;                                // Store number of vertices

						std::getline(fileIn, checkString);                // Skip rest of this line

						for (int i = 0; i < numVerts; i++)
						{
							Vertex tempVert;

							fileIn >> checkString                        // Skip "vert # ("
								>> checkString
								>> checkString;

							fileIn >> tempVert.TexCoord.x                // Store tex coords
								>> tempVert.TexCoord.y;

							fileIn >> checkString;                        // Skip ")"

							fileIn >> tempVert.StartWeight;                // Index of first weight this vert will be weighted to

							fileIn >> tempVert.WeightCount;                // Number of weights for this vertex

							std::getline(fileIn, checkString);            // Skip rest of this line

							subset.Vertices.push_back(tempVert);        // Push back this vertex into subsets vertex vector
						}
					}
					else if (checkString == L"numtris")
					{
						fileIn >> numTris;
						subset.NumTriangles = numTris;

						std::getline(fileIn, checkString);                // Skip rest of this line

						for (int i = 0; i < numTris; i++)                // Loop through each triangle
						{
							DWORD tempIndex;
							fileIn >> checkString;                        // Skip "tri"
							fileIn >> checkString;                        // Skip tri counter

							for (int k = 0; k < 3; k++)                    // Store the 3 indices
							{
								fileIn >> tempIndex;
								subset.Indices.push_back(tempIndex);
							}

							std::getline(fileIn, checkString);            // Skip rest of this line
						}
					}
					else if (checkString == L"numweights")
					{
						fileIn >> numWeights;

						std::getline(fileIn, checkString);                // Skip rest of this line

						for (int i = 0; i < numWeights; i++)
						{
							Weight tempWeight;
							fileIn >> checkString >> checkString;        // Skip "weight #"

							fileIn >> tempWeight.JointID;                // Store weight's joint ID

							fileIn >> tempWeight.Bias;                    // Store weight's influence over a vertex

							fileIn >> checkString;                        // Skip "("

							fileIn >> tempWeight.Position.x                    // Store weight's pos in joint's local space
								>> tempWeight.Position.z
								>> tempWeight.Position.y;

							std::getline(fileIn, checkString);            // Skip rest of this line

							subset.Weights.push_back(tempWeight);        // Push back tempWeight into subsets Weight array
						}

					}
					else
						std::getline(fileIn, checkString);                // Skip anything else

					fileIn >> checkString;                                // Skip "}"
				}

				//*** find each vertex's position using the joints and weights ***//
				for (int i = 0; i < (int)subset.Vertices.size(); ++i)
				{
					Vertex tempVert = subset.Vertices[i];
					tempVert.Pos = XMFLOAT3(0, 0, 0);    // Make sure the vertex's pos is cleared first

														 // Sum up the joints and weights information to get vertex's position
					for (int j = 0; j < tempVert.WeightCount; ++j)
					{
						Weight tempWeight = subset.Weights[tempVert.StartWeight + j];
						Joint tempJoint = MD5Model.Joints[tempWeight.JointID];

						// Convert joint orientation and weight pos to vectors for easier computation
						// When converting a 3d vector to a quaternion, you should put 0 for "w", and
						// When converting a quaternion to a 3d vector, you can just ignore the "w"
						XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.Orientation.x, tempJoint.Orientation.y, tempJoint.Orientation.z, tempJoint.Orientation.w);
						XMVECTOR tempWeightPos = XMVectorSet(tempWeight.Position.x, tempWeight.Position.y, tempWeight.Position.z, 0.0f);

						// We will need to use the conjugate of the joint orientation quaternion
						// To get the conjugate of a quaternion, all you have to do is inverse the x, y, and z
						XMVECTOR tempJointOrientationConjugate = XMVectorSet(-tempJoint.Orientation.x, -tempJoint.Orientation.y, -tempJoint.Orientation.z, tempJoint.Orientation.w);

						// Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
						// We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"
						XMFLOAT3 rotatedPoint;
						XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

						// Now move the verices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
						// The weight bias is used because multiple weights might have an effect on the vertices final position. Each weight is attached to one joint.
						tempVert.Pos.x += (tempJoint.Postion.x + rotatedPoint.x) * tempWeight.Bias;
						tempVert.Pos.y += (tempJoint.Postion.y + rotatedPoint.y) * tempWeight.Bias;
						tempVert.Pos.z += (tempJoint.Postion.z + rotatedPoint.z) * tempWeight.Bias;

						// Basically what has happened above, is we have taken the weights position relative to the joints position
						// we then rotate the weights position (so that the weight is actually being rotated around (0, 0, 0) in world space) using
						// the quaternion describing the joints rotation. We have stored this rotated point in rotatedPoint, which we then add to
						// the joints position (because we rotated the weight's position around (0,0,0) in world space, and now need to translate it
						// so that it appears to have been rotated around the joints position). Finally we multiply the answer with the weights bias,
						// or how much control the weight has over the final vertices position. All weight's bias effecting a single vertex's position
						// must add up to 1.
					}

					subset.Positions.push_back(tempVert.Pos);            // Store the vertices position in the position vector instead of straight into the vertex vector
																		 // since we can use the positions vector for certain things like collision detection or picking
																		 // without having to work with the entire vertex structure.
				}

				// Put the positions into the vertices for this subset
				for (int i = 0; i < (int)subset.Vertices.size(); i++)
				{
					subset.Vertices[i].Pos = subset.Positions[i];
				}

				//*** Calculate vertex normals using normal averaging ***///
				std::vector<XMFLOAT3> tempNormal;

				//normalized and unnormalized normals
				XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

				//Used to get vectors (sides) from the position of the verts
				float vecX, vecY, vecZ;

				//Two edges of our triangle
				XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

				//Compute face normals
				for (int i = 0; i < subset.NumTriangles; ++i)
				{
					//Get the vector describing one edge of our triangle (edge 0,2)
					vecX = subset.Vertices[subset.Indices[(i * 3)]].Pos.x - subset.Vertices[subset.Indices[(i * 3) + 2]].Pos.x;
					vecY = subset.Vertices[subset.Indices[(i * 3)]].Pos.y - subset.Vertices[subset.Indices[(i * 3) + 2]].Pos.y;
					vecZ = subset.Vertices[subset.Indices[(i * 3)]].Pos.z - subset.Vertices[subset.Indices[(i * 3) + 2]].Pos.z;
					edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our first edge

																	//Get the vector describing another edge of our triangle (edge 2,1)
					vecX = subset.Vertices[subset.Indices[(i * 3) + 2]].Pos.x - subset.Vertices[subset.Indices[(i * 3) + 1]].Pos.x;
					vecY = subset.Vertices[subset.Indices[(i * 3) + 2]].Pos.y - subset.Vertices[subset.Indices[(i * 3) + 1]].Pos.y;
					vecZ = subset.Vertices[subset.Indices[(i * 3) + 2]].Pos.z - subset.Vertices[subset.Indices[(i * 3) + 1]].Pos.z;
					edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our second edge

																	//Cross multiply the two edge vectors to get the un-normalized face normal
					XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

					tempNormal.push_back(unnormalized);
				}

				//Compute vertex normals (normal Averaging)
				XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				int facesUsing = 0;
				float tX, tY, tZ;    //temp axis variables

									 //Go through each vertex
				for (int i = 0; i < (int)subset.Vertices.size(); ++i)
				{
					//Check which triangles use this vertex
					for (int j = 0; j < subset.NumTriangles; ++j)
					{
						if ((int)subset.Indices[j * 3] == i ||
							(int)subset.Indices[(j * 3) + 1] == i ||
							(int)subset.Indices[(j * 3) + 2] == i)
						{
							tX = XMVectorGetX(normalSum) + tempNormal[j].x;
							tY = XMVectorGetY(normalSum) + tempNormal[j].y;
							tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

							normalSum = XMVectorSet(tX, tY, tZ, 0.0f);    //If a face is using the vertex, add the unormalized face normal to the normalSum

							facesUsing++;
						}
					}

					//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
					normalSum = normalSum / (float)facesUsing;

					//Normalize the normalSum vector
					normalSum = XMVector3Normalize(normalSum);

					//Store the normal and tangent in our current vertex
					subset.Vertices[i].Normal.x = -XMVectorGetX(normalSum);
					subset.Vertices[i].Normal.y = -XMVectorGetY(normalSum);
					subset.Vertices[i].Normal.z = -XMVectorGetZ(normalSum);

					//Clear normalSum, facesUsing for next vertex
					normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
					facesUsing = 0;
				}

				// Create index buffer
				D3D11_BUFFER_DESC indexBufferDesc;
				ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

				indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				indexBufferDesc.ByteWidth = sizeof(DWORD) * subset.NumTriangles * 3;
				indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				indexBufferDesc.CPUAccessFlags = 0;
				indexBufferDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA iinitData;

				iinitData.pSysMem = &subset.Indices[0];
				device->CreateBuffer(&indexBufferDesc, &iinitData, &subset.IndexBuff);

				//Create Vertex Buffer
				D3D11_BUFFER_DESC vertexBufferDesc;
				ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

				vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                            // We will be updating this buffer, so we must set as dynamic
				vertexBufferDesc.ByteWidth = sizeof(Vertex) * subset.Vertices.size();
				vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                // Give CPU power to write to buffer
				vertexBufferDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA vertexBufferData;

				ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
				vertexBufferData.pSysMem = &subset.Vertices[0];
				device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &subset.VertBuff);

				// Push back the temp subset into the models subset vector
				MD5Model.Subsets.push_back(subset);
			}
		}
	}
	else
	{
		// create message
		std::wstring message = L"Could not open: ";
		message += filename;

		MessageBox(0, message.c_str(),    // display message
			L"Error", MB_OK);

		return false;
	}

	return true;
}

bool Skeleton::LoadMD5Anim(std::wstring filename, Model3D & MD5Model)
{
	ModelAnimation tempAnim;						// Temp animation to later store in our model's animation array

	std::wifstream fileIn(filename.c_str());		// Open file

	std::wstring checkString;						// Stores the next string from our file

	if (fileIn)										// Check if the file was opened
	{
		while (fileIn)								// Loop until the end of the file is reached
		{
			fileIn >> checkString;					// Get next string from file

			if (checkString == L"MD5Version")		// Get MD5 version (this function supports version 10)
			{
				fileIn >> checkString;
				/*MessageBox(0, checkString.c_str(),	//display message
				L"MD5Version", MB_OK);*/
			}
			else if (checkString == L"commandline")
			{
				std::getline(fileIn, checkString);	// Ignore the rest of this line
			}
			else if (checkString == L"numFrames")
			{
				fileIn >> tempAnim.NumFrames;				// Store number of frames in this animation
			}
			else if (checkString == L"numJoints")
			{
				fileIn >> tempAnim.NumJoints;				// Store number of joints (must match .md5mesh)
			}
			else if (checkString == L"frameRate")
			{
				fileIn >> tempAnim.FrameRate;				// Store animation's frame rate (frames per second)
			}
			else if (checkString == L"numAnimatedComponents")
			{
				fileIn >> tempAnim.NumAnimatedComponents;	// Number of components in each frame section
			}
			else if (checkString == L"hierarchy")
			{
				fileIn >> checkString;				// Skip opening bracket "{"

				for (int i = 0; i < tempAnim.NumJoints; i++)	// Load in each joint
				{
					AnimJointInfo tempJoint;

					fileIn >> tempJoint.Name;		// Get joints name
													// Sometimes the names might contain spaces. If that is the case, we need to continue
													// to read the name until we get to the closing " (quotation marks)
					if (tempJoint.Name[tempJoint.Name.size() - 1] != '"')
					{
						wchar_t checkChar;
						bool jointNameFound = false;
						while (!jointNameFound)
						{
							checkChar = fileIn.get();

							if (checkChar == '"')
								jointNameFound = true;

							tempJoint.Name += checkChar;
						}
					}

					// Remove the quotation marks from joints name
					tempJoint.Name.erase(0, 1);
					tempJoint.Name.erase(tempJoint.Name.size() - 1, 1);

					fileIn >> tempJoint.ParentID;			// Get joints parent ID
					fileIn >> tempJoint.Flags;				// Get flags
					fileIn >> tempJoint.StartIndex;			// Get joints start index

															// Make sure the joint exists in the model, and the parent ID's match up
															// because the bind pose (md5mesh) joint hierarchy and the animations (md5anim)
															// joint hierarchy must match up
					bool jointMatchFound = false;
					for (int k = 0; k < MD5Model.NumJoints; k++)
					{
						if (MD5Model.Joints[k].Name == tempJoint.Name)
						{
							if (MD5Model.Joints[k].ParentID == tempJoint.ParentID)
							{
								jointMatchFound = true;
								tempAnim.JointInfo.push_back(tempJoint);
							}
						}
					}
					if (!jointMatchFound)					// If the skeleton system does not match up, return false
						return false;						// You might want to add an error message here

					std::getline(fileIn, checkString);		// Skip rest of this line
				}
			}
			else if (checkString == L"bounds")			// Load in the AABB for each animation
			{
				fileIn >> checkString;						// Skip opening bracket "{"

				for (int i = 0; i < tempAnim.NumFrames; i++)
				{
					BoundingBox tempBB;

					fileIn >> checkString;					// Skip "("
					fileIn >> tempBB.Min.x >> tempBB.Min.z >> tempBB.Min.y;
					fileIn >> checkString >> checkString;	// Skip ") ("
					fileIn >> tempBB.Max.x >> tempBB.Max.z >> tempBB.Max.y;
					fileIn >> checkString;					// Skip ")"

					tempAnim.FrameBounds.push_back(tempBB);
				}
			}
			else if (checkString == L"baseframe")			// This is the default position for the animation
			{												// All frames will build their skeletons off this
				fileIn >> checkString;						// Skip opening bracket "{"

				for (int i = 0; i < tempAnim.NumJoints; i++)
				{
					Joint tempBFJ;

					fileIn >> checkString;						// Skip "("
					fileIn >> tempBFJ.Postion.x >> tempBFJ.Postion.z >> tempBFJ.Postion.y;
					fileIn >> checkString >> checkString;		// Skip ") ("
					fileIn >> tempBFJ.Orientation.x >> tempBFJ.Orientation.z >> tempBFJ.Orientation.y;
					fileIn >> checkString;						// Skip ")"

					tempAnim.BaseFrameJoints.push_back(tempBFJ);
				}
			}
			else if (checkString == L"frame")		// Load in each frames skeleton (the parts of each joint that changed from the base frame)
			{
				FrameData tempFrame;

				fileIn >> tempFrame.FrameID;		// Get the frame ID

				fileIn >> checkString;				// Skip opening bracket "{"

				for (int i = 0; i < tempAnim.NumAnimatedComponents; i++)
				{
					float tempData;
					fileIn >> tempData;				// Get the data

					tempFrame.FrameDataVec.push_back(tempData);
				}

				tempAnim.FrameData.push_back(tempFrame);

				///*** build the frame skeleton ***///
				std::vector<Joint> tempSkeleton;

				for (int i = 0; i < (int)tempAnim.JointInfo.size(); i++)
				{
					int k = 0;						// Keep track of position in frameData array

													// Start the frames joint with the base frame's joint
					Joint tempFrameJoint = tempAnim.BaseFrameJoints[i];

					tempFrameJoint.ParentID = tempAnim.JointInfo[i].ParentID;

					// Notice how I have been flipping y and z. this is because some modeling programs such as
					// 3ds max (which is what I use) use a right handed coordinate system. Because of this, we
					// need to flip the y and z axes. If your having problems loading some models, it's possible
					// the model was created in a left hand coordinate system. in that case, just reflip all the
					// y and z axes in our md5 mesh and anim loader.
					if (tempAnim.JointInfo[i].Flags & 1)		// pos.x	( 000001 )
						tempFrameJoint.Postion.x = tempFrame.FrameDataVec[tempAnim.JointInfo[i].StartIndex + k++];

					if (tempAnim.JointInfo[i].Flags & 2)		// pos.y	( 000010 )
						tempFrameJoint.Postion.z = tempFrame.FrameDataVec[tempAnim.JointInfo[i].StartIndex + k++];

					if (tempAnim.JointInfo[i].Flags & 4)		// pos.z	( 000100 )
						tempFrameJoint.Postion.y = tempFrame.FrameDataVec[tempAnim.JointInfo[i].StartIndex + k++];

					if (tempAnim.JointInfo[i].Flags & 8)		// orientation.x	( 001000 )
						tempFrameJoint.Orientation.x = tempFrame.FrameDataVec[tempAnim.JointInfo[i].StartIndex + k++];

					if (tempAnim.JointInfo[i].Flags & 16)	// orientation.y	( 010000 )
						tempFrameJoint.Orientation.z = tempFrame.FrameDataVec[tempAnim.JointInfo[i].StartIndex + k++];

					if (tempAnim.JointInfo[i].Flags & 32)	// orientation.z	( 100000 )
						tempFrameJoint.Orientation.y = tempFrame.FrameDataVec[tempAnim.JointInfo[i].StartIndex + k++];


					// Compute the quaternions w
					float t = 1.0f - (tempFrameJoint.Orientation.x * tempFrameJoint.Orientation.x)
						- (tempFrameJoint.Orientation.y * tempFrameJoint.Orientation.y)
						- (tempFrameJoint.Orientation.z * tempFrameJoint.Orientation.z);
					if (t < 0.0f)
					{
						tempFrameJoint.Orientation.w = 0.0f;
					}
					else
					{
						tempFrameJoint.Orientation.w = -sqrtf(t);
					}

					// Now, if the upper arm of your skeleton moves, you need to also move the lower part of your arm, and then the hands, and then finally the fingers (possibly weapon or tool too)
					// This is where joint hierarchy comes in. We start at the top of the hierarchy, and move down to each joints child, rotating and translating them based on their parents rotation
					// and translation. We can assume that by the time we get to the child, the parent has already been rotated and transformed based of it's parent. We can assume this because
					// the child should never come before the parent in the files we loaded in.
					if (tempFrameJoint.ParentID >= 0)
					{
						Joint parentJoint = tempSkeleton[tempFrameJoint.ParentID];

						// Turn the XMFLOAT3 and 4's into vectors for easier computation
						XMVECTOR parentJointOrientation = XMVectorSet(parentJoint.Orientation.x, parentJoint.Orientation.y, parentJoint.Orientation.z, parentJoint.Orientation.w);
						XMVECTOR tempJointPos = XMVectorSet(tempFrameJoint.Postion.x, tempFrameJoint.Postion.y, tempFrameJoint.Postion.z, 0.0f);
						XMVECTOR parentOrientationConjugate = XMVectorSet(-parentJoint.Orientation.x, -parentJoint.Orientation.y, -parentJoint.Orientation.z, parentJoint.Orientation.w);

						// Calculate current joints position relative to its parents position
						XMFLOAT3 rotatedPos;
						XMStoreFloat3(&rotatedPos, XMQuaternionMultiply(XMQuaternionMultiply(parentJointOrientation, tempJointPos), parentOrientationConjugate));

						// Translate the joint to model space by adding the parent joint's pos to it
						tempFrameJoint.Postion.x = rotatedPos.x + parentJoint.Postion.x;
						tempFrameJoint.Postion.y = rotatedPos.y + parentJoint.Postion.y;
						tempFrameJoint.Postion.z = rotatedPos.z + parentJoint.Postion.z;

						// Currently the joint is oriented in its parent joints space, we now need to orient it in
						// model space by multiplying the two orientations together (parentOrientation * childOrientation) <- In that order
						XMVECTOR tempJointOrient = XMVectorSet(tempFrameJoint.Orientation.x, tempFrameJoint.Orientation.y, tempFrameJoint.Orientation.z, tempFrameJoint.Orientation.w);
						tempJointOrient = XMQuaternionMultiply(parentJointOrientation, tempJointOrient);

						// Normalize the orienation quaternion
						tempJointOrient = XMQuaternionNormalize(tempJointOrient);

						XMStoreFloat4(&tempFrameJoint.Orientation, tempJointOrient);
					}

					// Store the joint into our temporary frame skeleton
					tempSkeleton.push_back(tempFrameJoint);
				}

				// Push back our newly created frame skeleton into the animation's frameSkeleton array
				tempAnim.FrameSkeleton.push_back(tempSkeleton);

				fileIn >> checkString;				// Skip closing bracket "}"
			}
		}

		// Calculate and store some usefull animation data
		tempAnim.FrameTime = 1.0f / tempAnim.FrameRate;						// Set the time per frame
		tempAnim.TotalAnimTime = tempAnim.NumFrames * tempAnim.FrameTime;	// Set the total time the animation takes
		tempAnim.CurrAnimTime = 0.0f;										// Set the current time to zero

		MD5Model.Animations.push_back(tempAnim);							// Push back the animation into our model object
	}
	else	// If the file was not loaded
	{
		 // create message
		std::wstring message = L"Could not open: ";
		message += filename;

		MessageBox(0, message.c_str(),				// display message
			L"Error", MB_OK);

		return false;
	}

	return true;
}