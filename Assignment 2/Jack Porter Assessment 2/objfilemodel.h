#pragma once

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fbxsdk.h>
#include <unordered_map>
#include <algorithm>

using namespace std;

class ObjFileModel
{
private:
	ID3D11Device*           pD3DDevice;
	ID3D11DeviceContext*    pImmediateContext;
	
	FbxManager* m_pFBXManager = nullptr;
	
//////////////////////////////////////////////////

	int loadfile(char* fname);

	char* fbuffer;
	long fbuffersize; // filesize
	size_t actualsize; // actual size of loaded data (can be less if loading as text files as ASCII CR (0d) are stripped out)

//////////////////////////////////////////////////

	void parsefile();
	bool getnextline() ;
	bool getnexttoken(int& tokenstart, int& tokenlength);

	unsigned int  tokenptr;

//////////////////////////////////////////////////

	bool createVB();

	ID3D11Buffer* pVertexBuffer; 

	struct VertexBlendingInfo
	{
		unsigned int blendingIndex;
		double blendingWeight;

		VertexBlendingInfo() :
			blendingIndex(0),
			blendingWeight(0.0)
		{}

		bool operator < (const VertexBlendingInfo& rhs)
		{
			return (blendingWeight > rhs.blendingWeight);
		}
	};

	struct BlendingIndexWeightPair
	{
		unsigned int blendingIndex;
		double blendingWeight;
		BlendingIndexWeightPair() :
			blendingIndex(0),
			blendingWeight(0)
		{}
	};

	struct ControlPoint
	{
		XMFLOAT3 position;
		vector<BlendingIndexWeightPair> blendingInfo;

		ControlPoint()
		{
			blendingInfo.reserve(4);
		}
	};

	struct Keyframe
	{
		FbxLongLong frameNumber;
		FbxAMatrix globalTransform;
		Keyframe* next;
		Keyframe() : next(nullptr)
		{}
	};

	struct Joint
	{
		int parentIndex;
		string name;
		FbxAMatrix globalBlindposeInverse;
		Keyframe* animation;
		FbxNode* node;
		Joint() : node(nullptr), animation(nullptr)
		{
			globalBlindposeInverse.SetIdentity();
			parentIndex = -1;
		}
		~Joint()
		{
			while (animation)
			{
				Keyframe* temp = animation->next;
				delete animation;
				animation = temp;
			}
		}
	};

	struct Skeleton 
	{
		vector<Joint> joints;
	};

	struct Triangle 
	{
		vector<unsigned int> indices;
	};

	struct Bone
	{
		string name;
		XMMATRIX transformationMatrix;
		FbxMesh* mesh;
		vector<Bone*> children;
		XMMATRIX toParent;
		XMMATRIX toRoot;
	};

	struct Skin
	{
		vector<Bone*> bones;
	};

	struct PNTIWVertex
	{
		XMFLOAT3 position;
		XMFLOAT2 UV;
		XMFLOAT3 normal;
		std::vector<VertexBlendingInfo> vertexBlendingInfos;

		void SortBlendingInfoByWeight()
		{
			sort(vertexBlendingInfos.begin(), vertexBlendingInfos.end());
		}
	};


	Skeleton m_skeleton;
	unordered_map<unsigned int, ControlPoint*> m_controlPoints;
	FbxLongLong m_animationLength;
	string m_animationName;
	bool m_FBX;
	bool m_hasAnimation;
	FbxScene* m_FBXScene;
	unsigned int m_triangleCount;
	vector<Triangle> m_triangles;
	Skin m_skin;
	vector<XMMATRIX> m_offsetMatrix;
	Bone m_meshBone;

public:

	struct xyz { float x, y, z; };	//used for vertices and normals during file parse
	struct xy { float x, y; };		//used for texture coordinates during file parse

	// Define model vertex structure
	struct MODEL_POS_TEX_NORM_BLEND_VERTEX
	{
		XMFLOAT3 Pos;
		XMFLOAT2 TexCoord;
		XMFLOAT3 Normal;
		std::vector<VertexBlendingInfo> vertexBlendingInfos;
	};

	bool joints = false;
	string filename;

	ObjFileModel(char* filename, ID3D11Device* device, ID3D11DeviceContext* context);
	~ObjFileModel();

	void Draw(void);

	vector <xyz> position_list;		// list of parsed positions
	vector <xyz> normal_list;		// list of parsed normals
	vector <xy> texcoord_list;		// list of parsed texture coordinates
	vector <XMFLOAT4> boneID_list;
	vector <XMFLOAT4> weight_list;
	vector <int> pindices, tindices, nindices; // lists of indicies into above lists derived from faces

	MODEL_POS_TEX_NORM_BLEND_VERTEX* vertices;
	std::vector<PNTIWVertex> m_vertices;

	unsigned int numverts;

	HRESULT LoadFBX(char* fileName);
	void ProcessSkeleton(FbxNode* rootNode);
	void ProcessSkeletonHierarchy(FbxNode* node, int depth, int myIndex, int parentIndex);
	void ProcessJointsAndAnimations(FbxNode* node);
	FbxAMatrix GetGeometryTransformation(FbxNode* node);
	unsigned int FindJointIndexUsingName(const string& JointName);
	bool EndsWith(const std::string &mainString, const std::string &toMatch);
	void ProcessGeometry(FbxNode* node);
	void ProcessMesh(FbxNode* node);
	void ProcessControlPoints(FbxNode* node);
	void ReadNormals(FbxMesh* mesh, int controlPointIndex, int vertexCounter, XMFLOAT3& normal);
	void ReadUV(FbxMesh* mesh, int controlPointIndex, int textureUVIndex, int UVLayer, XMFLOAT2& UV);
	void CalculateToRootMatrix(Bone* bone, XMMATRIX parentMatrix);
	Bone* FindBone(Bone* bone, string boneName);
	void CalculateOffsetMatrix(int boneIndex);
};


