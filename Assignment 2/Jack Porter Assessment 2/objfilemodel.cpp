// turn off fopen warnings
#define _CRT_SECURE_NO_WARNINGS

#include "ObjFileModel.h"


// draw object
void ObjFileModel::Draw(void)
{
	if(m_hasAnimation)
	{
		//for (int i = 0; i < numverts; i++)
		//{
		//	//vertices[i].Pos.x = m_vertices[i].position.x;
		//	//vertices[i].Pos.y = m_vertices[i].position.y;
		//	//vertices[i].Pos.z = m_vertices[i].position.z;
		//
		//	for (int j = 0; j < vertices[i].vertexBlendingInfos.size(); j++)
		//	{
		//		vertices[i].Pos.x += 
		//	}
		//}
	}

	UINT stride = sizeof(MODEL_POS_TEX_NORM_BLEND_VERTEX);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pImmediateContext->Draw(numverts, 0);

}

HRESULT ObjFileModel::LoadFBX(char* fileName)
{
	if (m_pFBXManager == nullptr)
	{
		m_pFBXManager = FbxManager::Create();

		FbxIOSettings* IOSettings = FbxIOSettings::Create(m_pFBXManager, IOSROOT);
		m_pFBXManager->SetIOSettings(IOSettings);
	}

	FbxImporter* importer = FbxImporter::Create(m_pFBXManager, "");
	m_FBXScene = FbxScene::Create(m_pFBXManager, "");

	bool success = importer->Initialize(fileName, -1, m_pFBXManager->GetIOSettings());
	if (!success)
	{
		return E_FAIL;
	}

	success = importer->Import(m_FBXScene);
	if (!success)
	{
		return E_FAIL;
	}

	importer->Destroy();
	FbxNode* FBXRootNode = m_FBXScene->GetRootNode();

	if (FBXRootNode)
	{
		m_FBX = true;
		//for (int i = 0; i < FBXRootNode->GetChildCount(); i++)
		//{
		//	FbxNode* FBXChildNode = FBXRootNode->GetChild(i);
		//
		//	if (FBXChildNode->GetNodeAttribute() == NULL)
		//	{
		//		continue;
		//	}
		//
		//	FbxNodeAttribute::EType attributeType = FBXChildNode->GetNodeAttribute()->GetAttributeType();
		//
		//	if (attributeType != FbxNodeAttribute::eMesh)
		//	{
		//		continue;
		//	}
		//
		//	FbxMesh* mesh = (FbxMesh*)FBXChildNode->GetNodeAttribute();
		//
		//	FbxVector4* currentVertices = mesh->GetControlPoints();
		//
		//	for (int j = 0; j < mesh->GetPolygonCount(); j++)
		//	{
		//		int numVerts = mesh->GetPolygonSize(j);
		//
		//		if (numVerts != 3);
		//		{
		//			//return E_FAIL;
		//		}
		//
		//		for (int k = 0; k < numVerts; k++)
		//		{
		//			int controlPointIndex = mesh->GetPolygonVertex(j, k);
		//
		//			ControlPoint* currentControlPoint = m_controlPoints[controlPointIndex];
		//
		//			xyz vertex;
		//			vertex.x = (float)currentVertices[controlPointIndex].mData[0];
		//			vertex.y = (float)currentVertices[controlPointIndex].mData[1];
		//			vertex.z = (float)currentVertices[controlPointIndex].mData[2];
		//
		//			pindices.push_back(j + k);
		//			position_list.push_back(vertex);
		//
		//			FbxVector4 FBXnormal;
		//			mesh->GetPolygonVertexNormal(j, k, FBXnormal);
		//
		//			xyz normal;
		//			normal.x = (float)FBXnormal[0];
		//			normal.y = (float)FBXnormal[1];
		//			normal.z = (float)FBXnormal[2];
		//
		//			nindices.push_back(j + k);
		//			normal_list.push_back(normal);
		//
		//			FbxVector2 FBXuv;
		//			FbxStringList uvList;
		//			mesh->GetUVSetNames(uvList);
		//			bool mapped;
		//			mesh->GetPolygonVertexUV(j, k, uvList[0], FBXuv, mapped);
		//
		//			xy uv;
		//			uv.x = (float)FBXuv[0];
		//			uv.y = (float)FBXuv[1];
		//
		//			tindices.push_back(j + k);
		//			texcoord_list.push_back(uv);
		//		}
		//	}
		//}
		
		ProcessSkeleton(FBXRootNode);
		if (m_skeleton.joints.empty())
		{
			m_hasAnimation = false;
		}
		else
		{
			m_hasAnimation = true;
		}
		ProcessGeometry(FBXRootNode);
	}

	// create vertex array to pass to vertex buffer from parsed data
	numverts = m_triangleCount * 3;

	vertices = new MODEL_POS_TEX_NORM_BLEND_VERTEX[numverts]; // create big enough vertex array

	//for (unsigned int i = 0; i< numverts; i++)
	//{
	//
	//	// set position data
	//	vertices[i].Pos.x = position_list[i].x;
	//	vertices[i].Pos.y = position_list[i].y;
	//	vertices[i].Pos.z = position_list[i].z;
	//
	//	// set texture coord data
	//	vertices[i].TexCoord.x = texcoord_list[i].x;
	//	vertices[i].TexCoord.y = texcoord_list[i].y;
	//
	//	// set normal data
	//	vertices[i].Normal.x = normal_list[i].x;
	//	vertices[i].Normal.y = normal_list[i].y;
	//	vertices[i].Normal.z = normal_list[i].z;
	//
	//}

	for (unsigned int i = 0; i< numverts; i++)
	{
	
		// set position data
		vertices[i].Pos.x = m_vertices[i].position.x;
		vertices[i].Pos.y = m_vertices[i].position.y;
		vertices[i].Pos.z = m_vertices[i].position.z;
	
		// set texture coord data
		vertices[i].TexCoord.x = m_vertices[i].UV.x;
		vertices[i].TexCoord.y = m_vertices[i].UV.y;
	
		// set normal data
		vertices[i].Normal.x = m_vertices[i].normal.x;
		vertices[i].Normal.y = m_vertices[i].normal.y;
		vertices[i].Normal.z = m_vertices[i].normal.z;
	
		vertices[i].vertexBlendingInfos = m_vertices[i].vertexBlendingInfos;
	}

	// Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;										// Used by CPU and GPU
	bufferDesc.ByteWidth = sizeof(vertices[0])*numverts;						// Total size of buffer
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							// Use as a vertex buffer
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							// Allow CPU access
	HRESULT hr = pD3DDevice->CreateBuffer(&bufferDesc, NULL, &pVertexBuffer);	// Create the buffer

	if (FAILED(hr))
	{
		return false;
	}

	// Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	pImmediateContext->Map(pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);	// Lock the buffer to allow writing
	memcpy(ms.pData, vertices, sizeof(vertices[0])*numverts);							// Copy the data
	pImmediateContext->Unmap(pVertexBuffer, NULL);										// Unlock the buffer

	return S_OK;
}

void ObjFileModel::ProcessSkeleton(FbxNode * rootNode)
{
	for (int childIndex = 0; childIndex < rootNode->GetChildCount(); childIndex++)
	{
		FbxNode* currentNode = rootNode->GetChild(childIndex);
		ProcessSkeletonHierarchy(currentNode, 0, 0, -1);
	}
}

void ObjFileModel::ProcessSkeletonHierarchy(FbxNode * node, int depth, int myIndex, int parentIndex)
{
	if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint currentBone;
		currentBone.parentIndex = parentIndex;
		currentBone.name = node->GetName();
		m_skeleton.joints.push_back(currentBone);
	}
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchy(node->GetChild(i), depth + 1, m_skeleton.joints.size(), myIndex);
	}
}

void ObjFileModel::ProcessJointsAndAnimations(FbxNode * node)
{

	FbxMesh* currentMesh = (FbxMesh*)node->GetNodeAttribute();
	unsigned int numOfDeformers = currentMesh->GetDeformerCount();

	FbxAMatrix geometryTransform = GetGeometryTransformation(node);
	int count = 0;
	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* currentSkin = reinterpret_cast<FbxSkin*>(currentMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currentSkin)
		{
			continue;
		}

		unsigned int numOfClusters = currentSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; clusterIndex++)
		{
			FbxCluster* currentCluster = currentSkin->GetCluster(clusterIndex);
			string currentJointName = currentCluster->GetLink()->GetName();
			unsigned int currentJointIndex = FindJointIndexUsingName(currentJointName);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBlindposeInverseMatrix;

			currentCluster->GetTransformMatrix(transformMatrix);
			currentCluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBlindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			m_skeleton.joints[currentJointIndex].globalBlindposeInverse = globalBlindposeInverseMatrix;
			m_skeleton.joints[currentJointIndex].node = currentCluster->GetLink();

			unsigned int numOfIndices = currentCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; i++)
			{
				BlendingIndexWeightPair currentBlendingIndexWeightPair;
				currentBlendingIndexWeightPair.blendingIndex = currentJointIndex;
				currentBlendingIndexWeightPair.blendingWeight = *currentCluster->GetControlPointWeights();
				int currentPoint = currentCluster->GetControlPointIndices()[i];
				m_controlPoints[currentPoint]->blendingInfo.push_back(currentBlendingIndexWeightPair);
				count++;
			}

			FbxAnimStack* currentAnimStack = m_FBXScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currentAnimStack->GetName();
			m_animationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = m_FBXScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			m_animationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			Keyframe** currentAnimation = &m_skeleton.joints[currentJointIndex].animation;

			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i < end.GetFrameCount(FbxTime::eFrames24); i++)
			{
				FbxTime currentTime;
				currentTime.SetFrame(i, FbxTime::eFrames24);
				*currentAnimation = new Keyframe();
				(*currentAnimation)->frameNumber = i;
				FbxAMatrix currentTransformOffset = node->EvaluateGlobalTransform(currentTime) * geometryTransform;
				(*currentAnimation)->globalTransform = currentTransformOffset.Inverse() * currentCluster->GetLink()->EvaluateGlobalTransform(currentTime);
				currentAnimation = &((*currentAnimation)->next);
			}
		}
	}

	BlendingIndexWeightPair currentBlendingIndexWeightPair;
	currentBlendingIndexWeightPair.blendingIndex = 0;
	currentBlendingIndexWeightPair.blendingWeight = 0;
	for (auto itr = m_controlPoints.begin(); itr != m_controlPoints.end(); ++itr)
	{
		for (unsigned int i = itr->second->blendingInfo.size(); i <= 4; ++i)
		{
			itr->second->blendingInfo.push_back(currentBlendingIndexWeightPair);
		}
	}

}

FbxAMatrix ObjFileModel::GetGeometryTransformation(FbxNode * node)
{
	if (!node)
	{
		throw exception("Null for mesh geometry");
	}

	const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

unsigned int ObjFileModel::FindJointIndexUsingName(const string & JointName)
{
	for (unsigned int i = 0; i < m_skeleton.joints.size(); i++)
	{
		if (m_skeleton.joints[i].name == JointName)
		{
			return i;
		}
	}

	throw std::exception("Skeleton information in FBX file is corrupted.");
}

bool ObjFileModel::EndsWith(const std::string & mainString, const std::string & toMatch)
{
	if (mainString.size() > toMatch.size() && mainString.compare(mainString.size() - toMatch.size(), toMatch.size(), toMatch) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ObjFileModel::ProcessGeometry(FbxNode * node)
{
	if (node->GetNodeAttribute() != NULL)
	{
		switch (node->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			ProcessControlPoints(node);
			if (m_hasAnimation)
			{
				ProcessJointsAndAnimations(node);
			}
			ProcessMesh(node);
			break;
		}
	}

	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		ProcessGeometry(node->GetChild(i));
	}
}

void ObjFileModel::ProcessMesh(FbxNode * node)
{
	FbxMesh* currentMesh = node->GetMesh();
	
	m_triangleCount = currentMesh->GetPolygonCount();
	int vertexCounter = 0;
	m_triangles.reserve(m_triangleCount);
	
	for (unsigned int i = 0; i < m_triangleCount; ++i)
	{
		XMFLOAT3 normal[3];
		XMFLOAT3 tangent[3];
		XMFLOAT3 binormal[3];
		XMFLOAT2 UV[3][2];
		Triangle currentTriangle;
		m_triangles.push_back(currentTriangle);
	
		for (unsigned int j = 0; j < 3; ++j)
		{
			int controlPointIndex = currentMesh->GetPolygonVertex(i, j);
			ControlPoint* currentControlPoint = m_controlPoints[controlPointIndex];
	
			ReadNormals(currentMesh, controlPointIndex, vertexCounter, normal[j]);
			// We only have diffuse texture
			for (int k = 0; k < 1; ++k)
			{
				ReadUV(currentMesh, controlPointIndex, currentMesh->GetTextureUVIndex(i, j), k, UV[j][k]);
			}
	
	
			PNTIWVertex temp;
			temp.position = currentControlPoint->position;			
			temp.normal = normal[j];
			temp.UV = UV[j][0];
			// Copy the blending info from each control point
			for (unsigned int i = 0; i < currentControlPoint->blendingInfo.size(); ++i)
			{
				VertexBlendingInfo currBlendingInfo;
				currBlendingInfo.blendingIndex = currentControlPoint->blendingInfo[i].blendingIndex;
				currBlendingInfo.blendingWeight = currentControlPoint->blendingInfo[i].blendingWeight;
				temp.vertexBlendingInfos.push_back(currBlendingInfo);
			}
			// Sort the blending info so that later we can remove
			// duplicated vertices
			temp.SortBlendingInfoByWeight();
	
			m_vertices.push_back(temp);
			m_triangles.back().indices.push_back(vertexCounter);
			++vertexCounter;
		}
	}
}

void ObjFileModel::ProcessControlPoints(FbxNode * node)
{
	FbxMesh* currentMesh = node->GetMesh();
	unsigned int ctrlPointCount = currentMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		ControlPoint* currentControlPoint = new ControlPoint();
		XMFLOAT3 currentPosition;
		currentPosition.x = static_cast<float>(currentMesh->GetControlPointAt(i).mData[0]);
		currentPosition.y = static_cast<float>(currentMesh->GetControlPointAt(i).mData[1]);
		currentPosition.z = static_cast<float>(currentMesh->GetControlPointAt(i).mData[2]);
		currentControlPoint->position = currentPosition;
		m_controlPoints[i] = currentControlPoint;
	}
}

void ObjFileModel::ReadNormals(FbxMesh * mesh, int controlPointIndex, int vertexCounter, XMFLOAT3 & normal)
{
	if (mesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(controlPointIndex);
			normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
			normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
			normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(vertexCounter);
			normal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			normal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			normal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void ObjFileModel::ReadUV(FbxMesh * mesh, int controlPointIndex, int textureUVIndex, int UVLayer, XMFLOAT2 & UV)
{
	if (UVLayer >= 2 || mesh->GetElementUVCount() <= UVLayer)
	{
		throw std::exception("Invalid UV Layer Number");
	}
	FbxGeometryElementUV* vertexUV = mesh->GetElementUV(UVLayer);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			UV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			UV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(controlPointIndex);
			UV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			UV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			UV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(textureUVIndex).mData[0]);
			UV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(textureUVIndex).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void ObjFileModel::CalculateToRootMatrix(Bone* bone, XMMATRIX parentMatrix)
{
	bone->toRoot = bone->toParent * parentMatrix;
	for (int i = 0; i < bone->children.size(); i++)
	{
		CalculateToRootMatrix(bone->children[i], bone->toRoot);
	}
}

ObjFileModel::Bone * ObjFileModel::FindBone(Bone * bone, string boneName)
{
	Bone* temporaryBone;
	if (bone->name == boneName)
	{
		return bone;
	}
	else
	{
		for (int i = 0; i < bone->children.size(); i++)
		{
			temporaryBone = bone->children[i];
			if (temporaryBone->name == boneName)
			{
				return temporaryBone;
			}
		}
	}
	return nullptr;
}

void ObjFileModel::CalculateOffsetMatrix(int boneIndex)
{
	string boneName = m_skin.bones[boneIndex]->name;

	Bone* bone = FindBone(m_skin.bones[0], boneName);
	if (bone != nullptr)
	{
		XMVECTOR determinant = XMMatrixDeterminant(bone->toRoot);
		m_offsetMatrix[boneIndex] = m_meshBone.toRoot * XMMatrixInverse(&determinant, bone->toRoot);
	}
}




// load object from obj file in constructor
ObjFileModel::ObjFileModel(char* fname, ID3D11Device* device, ID3D11DeviceContext* context)
{
	pD3DDevice = device;
	pImmediateContext = context;

	if(loadfile(fname)==0)
	{
		// file not loaded, check debug output;
		filename="FILE NOT LOADED";
		return;
	}

	
	
	filename = fname;

	if (EndsWith(filename, ".fbx"))
	{
		LoadFBX(fname);
	}
	else
	{
		parsefile();
		createVB();
	}



	delete[] fbuffer; // delete file buffer created in loadfile()
}


// load wavefront object file. adds terminating \n so last line of file can be correctly parsed as a 'line' later
// basic loader - only deals with vertices v, texcoords vt, normals vn 
//              - only copes with triangular meshes (no quads)
//              - doesn't deal with textures or materials
int ObjFileModel::loadfile(char* fname)
{
	FILE* pFile;

	pFile = fopen(fname , "r"); // if changed to bin format will read carriage return \r (0d) as well as \n (0a) into fbuffer, may need to add \r checks(but seemed to work with basic test)
	if (pFile==NULL) { DXTRACE_MSG("Failed to open model file");DXTRACE_MSG(fname); return 0 ;}

	// get file size
	fseek(pFile, 0, SEEK_END);
	fbuffersize = ftell(pFile);
	rewind(pFile);

	// allocate memory for entire file size
	fbuffer  = new char[fbuffersize+1]; // 1 added to cope with adding a \n later in case file doesn't end with \n 
	if (fbuffer == NULL) {fclose(pFile); DXTRACE_MSG("Failed allocate memory for model file");DXTRACE_MSG(fname); return 0 ;}

	// copy file into memory
	actualsize = fread(fbuffer,1,fbuffersize,pFile); // actualsize may be less than fbuffersize in text mode as \r are stripped
	if (actualsize == 0) {fclose(pFile); DXTRACE_MSG("Failed to read model file");DXTRACE_MSG(fname); return 0 ;}

	// add a newline at end in case file does not, so can deal with whole buffer as a set of lines of text
	fbuffer[actualsize] = '\n'; fclose(pFile);

	return 1;
}
 

// uses concept of getting parsable tokens seperated by whitespace and '/'
// one line of file is parsed at a time, lines seperated by '\n'
void ObjFileModel::parsefile()
{
	tokenptr=0; // token pointer points to first element of buffer

	int tokenstart, tokenlength;

	xyz tempxyz;
	xy tempxy;

	bool success;
	int line=0;

	do
	{	
		line++; // keep track of current line number for error reporting

		if(!getnexttoken(tokenstart, tokenlength)) continue; // get first token on line, go to next line if first token is \n

		// ADD FURTHER KEYWORDS HERE TO EXTEND CAPABILITIES
		if(strncmp(&fbuffer[tokenstart], "v ", 2)==0) // VERTEX POSITION - note the space in the string is needed (see vt, etc)
		{
			success=true; // used to see if correct number of tokens left on line for this type of attribute
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.x = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.y = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.z = (float) atof(&fbuffer[tokenstart]);

			// if not correct number of tokens, display error in debug output
			if(!success) {char s[100] = "ERROR: Badly formatted vertex, line : "; _itoa(line, &s[strlen(s)], 10); strcat(s, " : "); strcat(s, filename.c_str());  DXTRACE_MSG(s); }

			position_list.push_back(tempxyz); // add a new element to the list

		}
		else if(strncmp(&fbuffer[tokenstart], "vt", 2)==0) // TEXTURE COORDINATES
		{
			success=true;
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxy.x = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxy.y = (float) atof(&fbuffer[tokenstart]);

			if(!success) {char s[100] = "ERROR: Badly formatted texture coordinate, line : "; _itoa(line, &s[strlen(s)], 10); strcat(s, " : "); strcat(s, filename.c_str());  DXTRACE_MSG(s); }

			texcoord_list.push_back(tempxy);
		}
		else if(strncmp(&fbuffer[tokenstart], "vn", 2)==0)  // NORMALS
		{
			success=true;
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.x = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.y = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.z = (float) atof(&fbuffer[tokenstart]);

			if(!success) {char s[100] = "ERROR: Badly formatted normal, line : "; _itoa(line, &s[strlen(s)], 10); strcat(s, " : "); strcat(s, filename.c_str());  DXTRACE_MSG(s); }

			normal_list.push_back(tempxyz);
		}
		else if(strncmp(&fbuffer[tokenstart], "f ", 2)==0)  // FACE - only deals with triangles so far
		{
			int tempptr = tokenstart + 2; // skip "f "
			int forwardslashcount=0;
			bool adjacentslash = false;

			// this works out how many elements are specified for a face, e.g.
			// f 1 2 3				-> 0 forward slashes = just position
			// f 1/1 2/2 3/3		-> 3 slashes = position and texcoords
			// f 1/1/1 2/2/2 3/3/3	-> 6 slashes = position, texcoords, normals
			// f 1//1 2//2 3//3		-> 6 slashes with adjacent = position, normals
			while(fbuffer[tempptr] != '\n')
			{
				if(fbuffer[tempptr] == '/')
				{
					forwardslashcount++;
					if(fbuffer[tempptr-1] == '/') adjacentslash=true;
				}
				tempptr++;
			}

			success=true;

			// Get 3 sets of indices per face
			for(int i=0; i<3; i++)
			{
				// get vertex index
				success = success && getnexttoken(tokenstart, tokenlength);
				pindices.push_back(atoi(&fbuffer[tokenstart]));

				if(forwardslashcount>=3&& adjacentslash==false) // get texcoord index if required 
				{
					success = success && getnexttoken(tokenstart, tokenlength);
					tindices.push_back(atoi(&fbuffer[tokenstart]));
				}

				if(forwardslashcount==6 || adjacentslash==true) // get normal index if required 
				{
					success = success && getnexttoken(tokenstart, tokenlength);
					nindices.push_back(atoi(&fbuffer[tokenstart]));
				}
			}

			if(!success) {char s[100] = "ERROR: Badly formatted face, line : "; _itoa(line, &s[strlen(s)], 10); strcat(s, " : "); strcat(s, filename.c_str());  DXTRACE_MSG(s); }
		}
	} while(getnextline() == true);
}


// get next token. if \n is next token do not proceed, use getnextline() to resume
bool ObjFileModel::getnexttoken(int& tokenstart, int& tokenlength)
{
	tokenstart = tokenptr; 
	tokenlength=1; 
	int tokenend;

	while(fbuffer[tokenptr] == ' ' || fbuffer[tokenptr] == '\t' || fbuffer[tokenptr] == '/') tokenptr++; //skip whitespace and '/'

	if(fbuffer[tokenptr] == '\n') { return false; } // keeps tokenptr pointing to \n as a token to indicate end of line
													// doesn't point to next token, dealt with in getnextline()
	tokenend=tokenptr+1;

	while(fbuffer[tokenend] != ' ' && fbuffer[tokenend] != '\t' && fbuffer[tokenend] != '\n' && fbuffer[tokenend] != '/') tokenend++; // find length of token by finding next whitespace or '\n' or '/'

	tokenlength = tokenend - tokenptr;
	tokenstart = tokenptr;
	tokenptr+=tokenlength; //ready for next token

	return true;
}


// gets next line of buffer by skipping to next element after end of current line, returns false when end of buffer exceeded
bool ObjFileModel::getnextline()
{
	// relies on getnexttoken()leaving tokenptr pointing to \n if encountered

	while(fbuffer[tokenptr] != '\n' && tokenptr < actualsize) tokenptr++; // skip to end of line

	tokenptr++; // point to start of next line

	if (tokenptr >= actualsize) return false;
	else return true;
}


// create Vertex buffer from parsed file data
bool ObjFileModel::createVB()
{
	// create vertex array to pass to vertex buffer from parsed data
	numverts = pindices.size();

	vertices = new MODEL_POS_TEX_NORM_BLEND_VERTEX[numverts]; // create big enough vertex array

	for(unsigned int i = 0; i< numverts; i++)
	{
		int vindex = pindices[i]-1; // use -1 for indices as .obj files indices begin at 1

		// set position data
		vertices[i].Pos.x = position_list[vindex].x;
		vertices[i].Pos.y = position_list[vindex].y;
		vertices[i].Pos.z = position_list[vindex].z;

		if(tindices.size() > 0)
		{ 
			// if there are any, set texture coord data
			int tindex = tindices[i]-1;
			vertices[i].TexCoord.x = texcoord_list[tindex].x;
			vertices[i].TexCoord.y = texcoord_list[tindex].y;
		}

		if(nindices.size() > 0)
		{
			// if there are any, set normal data
			int nindex = nindices[i]-1;
			vertices[i].Normal.x = normal_list[nindex].x;
			vertices[i].Normal.y = normal_list[nindex].y;
			vertices[i].Normal.z = normal_list[nindex].z;
		}
	}

	// Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;										// Used by CPU and GPU
	bufferDesc.ByteWidth = sizeof(vertices[0])*numverts;						// Total size of buffer
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							// Use as a vertex buffer
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							// Allow CPU access
	HRESULT hr = pD3DDevice->CreateBuffer(&bufferDesc, NULL, &pVertexBuffer);	// Create the buffer

	if(FAILED(hr))
    {
        return false;
    }

	// Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	pImmediateContext->Map(pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);	// Lock the buffer to allow writing
	memcpy(ms.pData, vertices, sizeof(vertices[0])*numverts);							// Copy the data
	pImmediateContext->Unmap(pVertexBuffer, NULL);										// Unlock the buffer

	return true;
}


ObjFileModel::~ObjFileModel()
{
	// clean up memory used by object
	if(pVertexBuffer) pVertexBuffer->Release();

	delete [] vertices;

	position_list.clear();
	normal_list.clear();
	texcoord_list.clear();
}
