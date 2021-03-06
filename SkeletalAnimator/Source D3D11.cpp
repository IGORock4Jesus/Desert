#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include <memory>
#include <DirectXMath.h>
#include <fstream>
#include <DirectXCollision.h>
#include <windowsx.h>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>


#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "assimp.lib")

#ifdef near
#undef near
#endif // near
#ifdef far
#undef far
#endif // far


using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

constexpr auto MAX_BONES = 32;
constexpr auto BONES_PER_VERTEX = 4;

struct Bone;
void UpdateBones(Bone* bone, XMMATRIX* parentMatrix = nullptr);
extern Bone rootBone;

struct KeyFrame
{
	float time;
	XMVECTOR position;
	XMVECTOR scaling;
	/*float rotation;*/
	XMVECTOR rotation;
};

struct Bone
{
	XMMATRIX offset, animated;
	std::vector<Bone> children;
};

class Channel
{
	Bone* bone; // ����� 
	std::vector<KeyFrame> keyFrames; // �������� �����
	const float length; // ����� �������� � ��������
	float time; // ������� �����

	FXMMATRIX Interpolate(const KeyFrame& k0, const KeyFrame& k1) {
		float t0 = k0.time, t1 = k1.time;
		float lerpTime = (time - t0) / (t1 - t0);
		XMVECTOR lerpedT, lerpedS;
		XMVECTOR lerpedR;
		lerpedT = XMVectorLerp(k0.position, k1.position, lerpTime);
		lerpedS = XMVectorLerp(k0.scaling, k1.scaling, lerpTime);
		lerpedR = XMQuaternionSlerp(k0.rotation, k1.rotation, lerpTime);
		XMMATRIX t, r, s;
		t = XMMatrixTranslationFromVector(lerpedT);
		s = XMMatrixScalingFromVector(lerpedS);
		r = XMMatrixRotationQuaternion(lerpedR);
		return s * r * t; // r * s * t in the original
		/*float t0 = k0.time, t1 = k1.time;
		float lerpTime = (time - t0) / (t1 - t0);
		D3DXQUATERNION lerpedR;
		D3DXQuaternionSlerp(&lerpedR, &k0.rotation, &k1.rotation, lerpTime);
		return lerpedR;*/
	}
public:
	Channel(Bone* bone, std::vector<KeyFrame> keyFrames, float length)
		:bone{ bone }, keyFrames{ keyFrames }, length{ length }, time{ 0.0f } {

	}

	void Update(float elapsedTime) {
		time += elapsedTime;
		if (time >= length) {
			time -= length;
		}

		// �����
		bone->offset = Get();
		//bone->offset = m * bone->offset;

		UpdateBones(&rootBone);
	}

	XMMATRIX Get() {
		// ���� - ����� ������� ������� ��������� ������� �����
		int k0{ 0 };
		while (true)
		{
			if (k0 + 1 > keyFrames.size())
				break;
			if (time < keyFrames[k0 + 1].time) {
				return Interpolate(keyFrames[k0], keyFrames[k0 + 1]);
			}
			else {
				++k0;
			}
		}
	}
};

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT3 normal;
	float boneWeights[BONES_PER_VERTEX];
	int boneIndices[BONES_PER_VERTEX];
};

struct VSPerFrameConstantBuffer
{
	XMFLOAT4X4 view;
	XMFLOAT4X4 proj;
	XMFLOAT4X4 bones[MAX_BONES];
};

struct VSPerObjectConstantBuffer
{
	XMFLOAT4X4 world;
};

struct Light {
	XMFLOAT3 direction;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

struct PSPerFrameConstantBuffer
{
	Light light;
};

//class Animation {
//	double duration{ 0 };
//	float time{ 0 };
//	std::vector<Channel> channels;
//
//	void Interpolate(const KeyFrame& k0, const KeyFrame& k1, XMMATRIX& out) {
//		float t0 = k0.time, t1 = k1.time;
//		float lerpTime = (time - t0) / (t1 - t0);
//		D3DXVECTOR3 lerpedT, lerpedS;
//		D3DXQUATERNION lerpedR;
//		D3DXVec3Lerp(&lerpedT, &k0.position, &k1.position, lerpTime);
//		D3DXVec3Lerp(&lerpedS, &k0.scaling, &k1.scaling, lerpTime);
//		D3DXQuaternionSlerp(&lerpedR, &k0.rotation, &k1.rotation, lerpTime);
//		XMMATRIX t, r, s;
//		XMMatrixTranslation(&t, lerpedT.x, lerpedT.y, lerpedT.z);
//		XMMatrixScaling(&t, lerpedS.x, lerpedS.y, lerpedS.z);
//		XMMatrixRotationQuaternion(&r, &lerpedR);
//		out = s * r * t; // r * s * t in the original
//	}
//
//public:
//	Animation()
//	{
//
//	}
//	void Update(float delta) {
//		time += delta;
//		if (time >= duration)
//			time = 0;
//		for (UINT i = 0; i < channels.size(); ++i) {
//			auto& keyFrames = channels[i].keyFrames;
//			UINT k0 = 0;
//			while (true)
//			{
//				if (k0 + 1 >= keyFrames.size())
//					break;
//				if (time < keyFrames[k0 + 1].time)
//				{
//					Interpolate(keyFrames[k0], keyFrames[k0 + 1], channels[i].bone->animated);
//					break;
//				}
//				else {
//					++k0;
//				}
//			}
//		}
//	}
//	void SetDuration(double duration) { this->duration = duration; }
//	Channel* CreateChannel() {
//		channels.push_back(Channel());
//		return &channels.back();
//	}
//};




LPCSTR className = "SKELETAL WINDOW";
HINSTANCE hinstance;
HWND hwnd;
ComPtr<ID3D11Device> device;
ComPtr<ID3D11DeviceContext> context;
ComPtr<IDXGISwapChain> swapChain;
ComPtr<ID3D11RenderTargetView> renderTargetView;
int windowWidth, windowHeight;
constexpr DWORD VERTEX_SIZE = sizeof(Vertex);
const D3D11_INPUT_ELEMENT_DESC inputLayoutElements[]{
	{ "POSITION",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "COLOR",		 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "NORMAL",		 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "BONEWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
ComPtr<ID3D11Buffer> vertexBuffer;
ComPtr<ID3D11Buffer> indexBuffer;
int indexCount;
Bone rootBone;
Channel* secondBoneAnimChannel;
ComPtr<ID3D11VertexShader> vertexShader;
ComPtr<ID3D11PixelShader> pixelShader;
ComPtr<ID3D11InputLayout> inputLayout;
Light light;
ComPtr<ID3D11Buffer> vsPerFrameConstantBuffer, vsPerObjectConstantBuffer, psPerFrameConstantBuffer;
PSPerFrameConstantBuffer psPerFrameBuffer;
VSPerFrameConstantBuffer vsPerFrameBuffer;
VSPerObjectConstantBuffer vsPerObjectBuffer;
ComPtr<ID3D11DepthStencilView> depthStencilView;
BoundingSphere boundingSphere;
bool selectedChain[4];
Assimp::Importer importer;



BoundingSphere MakeBoundingSphere(XMVECTOR points[], size_t count) {
	BoundingSphere bs;
	XMStoreFloat3(&bs.Center, XMVectorSet(0, 0, 0, 0));
	for (size_t i = 0; i < count; i++)
	{
		auto rad = XMVector3Length(XMVectorSubtract(points[i], XMVectorSet(0, 0, 0, 0)));
		bs.Radius = max(XMVectorGetX(rad), bs.Radius);
	}
	return bs;
}

void UpdateBones(Bone* bone, XMMATRIX* parentMatrix) {
	if (parentMatrix)
		bone->animated = XMMatrixMultiply(bone->offset, *parentMatrix);
	else
		bone->animated = bone->offset;

	for (auto&& c : bone->children) {
		UpdateBones(&c, &bone->animated);
	}
}
void RotateBone(Bone* bone, float degree) {
	if (!bone)return;
	XMMATRIX r;
	r = XMMatrixRotationZ(XMConvertToRadians(degree));
	bone->offset = r * bone->offset;
	UpdateBones(&rootBone);
}
void RotateBone2(float degree) {
	auto b2 = &rootBone.children.front();
	RotateBone(b2, degree);
}
void RotateBone3(float degree) {
	auto b2 = &rootBone.children.front();
	RotateBone(&b2->children.front(), degree);
}
void KeyDown(BYTE key) {
	switch (key)
	{
	case VK_LEFT:
		RotateBone2(10.0f);
		break;
	case VK_RIGHT:
		RotateBone2(-10.0f);
		break;
	case VK_UP:
		RotateBone3(10.0f);
		break;
	case VK_DOWN:
		RotateBone3(-10.0f);
		break;
	}
}

void MouseDown(int x, int y) {
	// ��������� ���������� � ����� �� ����� ��������� ������������ ����
	auto proj = XMMatrixTranspose(XMLoadFloat4x4(&vsPerFrameBuffer.proj));
	auto view = XMMatrixTranspose(XMLoadFloat4x4(&vsPerFrameBuffer.view));

	XMVECTOR direction = XMVectorSet(
		(2.0f * x / windowWidth - 1.0f) / proj.r[0].m128_f32[0],
		(-2.0f * y / windowHeight + 1.0f) / proj.r[1].m128_f32[1],
		1, 1
	);
	auto origin = XMVectorSet(0, 0, 0, 0);

	auto invertedView = XMMatrixInverse(nullptr, view);

	origin = XMVector3TransformCoord(origin, invertedView);
	direction = XMVector3TransformNormal(direction, invertedView);
	direction = XMVector3Normalize(direction);


	float distance;
	BoundingSphere bs; //XMLoadFloat4x4(&vsPerFrameBuffer.view);

	auto bone = &rootBone;
	boundingSphere.Transform(bs, bone->animated);
	if (bs.Intersects(origin, direction, distance)) {
		MessageBox(0, ("����� � 1 = " + to_string(distance)).c_str(), 0, 0);
	}

	int index = 2;
	do {
		bone = &bone->children.front();
		boundingSphere.Transform(bs, bone->animated);
		if (bs.Intersects(origin, direction, distance)) {
			MessageBox(0, ("����� � " + to_string(index) + " = " + to_string(distance)).c_str(), 0, 0);
		}
		index++;
	} while (bone->children.empty() == false);


}

LRESULT WindowProcessor(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		KeyDown((BYTE)w);
		break;
	case WM_LBUTTONDOWN:
		MouseDown(GET_X_LPARAM(l), GET_Y_LPARAM(l));
		break;
	default:
		return DefWindowProc(h, m, w, l);
	}
	return LRESULT(0);
}
bool InitializeCore(HINSTANCE hisntance) {
	::hinstance = hinstance;

	WNDCLASS wc{ 0 };
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hInstance = hinstance;
	wc.lpszClassName = className;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProcessor;
	RegisterClass(&wc);

	hwnd = CreateWindow(className, className, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, HWND_DESKTOP, nullptr, hinstance, nullptr);

	ShowWindow(hwnd, SW_NORMAL);

	RECT cr;
	GetClientRect(hwnd, &cr);
	windowWidth = cr.right - cr.left;
	windowHeight = cr.bottom - cr.top;

	D3D_FEATURE_LEVEL featureLevels[]{
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_3
	};

	DXGI_SWAP_CHAIN_DESC swapChainDesc{
		{
			windowWidth,
			windowHeight,
			{1, 60},
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			DXGI_MODE_SCALING_UNSPECIFIED
		},
		{1, 0},
		DXGI_USAGE_RENDER_TARGET_OUTPUT,
		1,
		hwnd,
		TRUE,
		DXGI_SWAP_EFFECT_DISCARD,
		0
	};
	D3D_FEATURE_LEVEL featureLevel;
	auto resutl = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, &featureLevel, &context);

	ComPtr<ID3D11Texture2D> texture;
	swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));

	device->CreateRenderTargetView(texture.Get(), nullptr, &renderTargetView);

	// ����� ��� ������� ����� ������� - ��� � ��� ��� ��������� ����������� ��� � �����
	D3D11_TEXTURE2D_DESC depthDesc{ 0 };
	depthDesc.Width = ::windowWidth;
	depthDesc.Height = ::windowHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc = { 1,0 };
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	device->CreateTexture2D(&depthDesc, nullptr, &texture);

	device->CreateDepthStencilView(texture.Get(), nullptr, &depthStencilView);

	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	D3D11_VIEWPORT viewport{ 0 };
	viewport.Height = windowHeight;
	viewport.Width = windowWidth;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);




	return true;
}

void UpdateAnimations(float elapsedTime) {
	secondBoneAnimChannel->Update(elapsedTime);
}

void UpdateGame() {
	static DWORD oldTime = ::timeGetTime();
	DWORD newTime = ::timeGetTime();
	int elapsed = newTime - oldTime;
	oldTime = newTime;
	float felapsed = elapsed * 0.001f;

	UpdateAnimations(felapsed);
}

void SetSubresource(ComPtr<ID3D11Buffer>& buffer, void* data, size_t size) {
	D3D11_MAPPED_SUBRESOURCE subresource;
	auto result = context->Map(buffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	if (FAILED(result))
		throw exception("�� ���� �������� ������ � ���������� �� ������.");
	memcpy(subresource.pData, data, size);
	context->Unmap(buffer.Get(), 0);
}
void RenderModel() {
	context->IASetInputLayout(inputLayout.Get());

	context->UpdateSubresource(psPerFrameConstantBuffer.Get(), 0, nullptr, &psPerFrameBuffer, 0, 0);
	context->UpdateSubresource(vsPerFrameConstantBuffer.Get(), 0, nullptr, &vsPerFrameBuffer, 0, 0);

	context->VSSetShader(vertexShader.Get(), 0, 0);
	context->VSSetConstantBuffers(0, 1, vsPerFrameConstantBuffer.GetAddressOf());
	context->VSSetConstantBuffers(1, 1, vsPerObjectConstantBuffer.GetAddressOf());

	context->PSSetShader(pixelShader.Get(), 0, 0);
	context->PSSetConstantBuffers(0, 1, psPerFrameConstantBuffer.GetAddressOf());



	UINT strides[]{ VERTEX_SIZE };
	UINT offsets[]{ 0 };
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), strides, offsets);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0U);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto bone = &rootBone;

	XMStoreFloat4x4(&vsPerObjectBuffer.world, XMMatrixTranspose(bone->animated));
	context->UpdateSubresource(vsPerObjectConstantBuffer.Get(), 0, nullptr, &vsPerObjectBuffer, 0, 0);
	context->DrawIndexed(indexCount, 0, 0);

	bone = &bone->children.front();
	XMStoreFloat4x4(&vsPerObjectBuffer.world, XMMatrixTranspose(bone->animated));
	context->UpdateSubresource(vsPerObjectConstantBuffer.Get(), 0, nullptr, &vsPerObjectBuffer, 0, 0);
	context->DrawIndexed(indexCount, 0, 0);

	bone = &bone->children.front();
	XMStoreFloat4x4(&vsPerObjectBuffer.world, XMMatrixTranspose(bone->animated));
	context->UpdateSubresource(vsPerObjectConstantBuffer.Get(), 0, nullptr, &vsPerObjectBuffer, 0, 0);
	context->DrawIndexed(indexCount, 0, 0);

	bone = &bone->children.front();
	XMStoreFloat4x4(&vsPerObjectBuffer.world, XMMatrixTranspose(bone->animated));
	context->UpdateSubresource(vsPerObjectConstantBuffer.Get(), 0, nullptr, &vsPerObjectBuffer, 0, 0);
	context->DrawIndexed(indexCount, 0, 0);
}

void RenderGame() {
	float color[]{ 0.4f, 0.4f, 0.4f, 1.0f };
	context->ClearRenderTargetView(renderTargetView.Get(), color);
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	RenderModel();

	swapChain->Present(1, 0);
}
void Run() {
	MSG msg{ 0 };
	while (true) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				return;
		}

		//UpdateGame();
		RenderGame();

		Sleep(1);
	}
}
void ReleaseCore() {
}

vector<BYTE> LoadShader(string filename) {
	ifstream file{ filename, ios::end };
	if (file.bad() || !file.good())
		return {};

	file.seekg(0, ios::end);
	size_t size = file.tellg();
	file.seekg(0, ios::beg);

	vector<BYTE> buffer(size);
	file.read((char*)buffer.data(), size);

	return move(buffer);
}
void CreateModel() {
	auto_ptr<const aiScene> scene{ importer.ReadFile(R"(C:\Users\igoro\Documents\3dsMax\export\test1.DAE)", 0) };
	if (!scene->HasMeshes()) return;
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		auto mesh = scene->mMeshes[i];
		if (string(mesh->mName.C_Str()) == "Box001") {
			vector<Vertex> vertices(mesh->mNumVertices);
			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				vertices[i] = {
					XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
				};
			}
		}
	}

	float xc = windowWidth / 2.0f;
	float yc = windowHeight / 2.0f;
	float wi = 100.0f;

	XMVECTOR points[]{
		XMVectorSet(0,  -wi, 0,  0),
		XMVectorSet(-wi, 0,  -wi, 0),
		XMVectorSet(-wi, 0,  wi,  0),
		XMVectorSet(wi,  0,  wi,  0),
		XMVectorSet(wi,  0,  -wi, 0),
	};
	XMFLOAT4 color{ 1.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT3 normals[5];
	XMStoreFloat3(&normals[0], XMVector3Cross(XMVectorSubtract(points[1], points[0]), XMVectorSubtract(points[2], points[0])));
	XMStoreFloat3(&normals[1], XMVector3Cross(XMVectorSubtract(points[2], points[0]), XMVectorSubtract(points[3], points[0])));
	XMStoreFloat3(&normals[2], XMVector3Cross(XMVectorSubtract(points[3], points[0]), XMVectorSubtract(points[4], points[0])));
	XMStoreFloat3(&normals[3], XMVector3Cross(XMVectorSubtract(points[4], points[0]), XMVectorSubtract(points[1], points[0])));
	XMStoreFloat3(&normals[4], XMVector3Cross(XMVectorSubtract(points[1], points[2]), XMVectorSubtract(points[3], points[2])));

	XMFLOAT3 positions[5];
	for (size_t i = 0; i < 5; i++)
	{
		XMStoreFloat3(&positions[i], points[i]);
	}

	Vertex vertices[]{
		{positions[0], color, normals[0]},
		{positions[2], color, normals[0]},
		{positions[1], color, normals[0]},

		{positions[0], color, normals[1]},
		{positions[3], color, normals[1]},
		{positions[2], color, normals[1]},

		{positions[0], color, normals[2]},
		{positions[4], color, normals[2]},
		{positions[3], color, normals[2]},

		{positions[0], color, normals[3]},
		{positions[1], color, normals[3]},
		{positions[4], color, normals[3]},

		{positions[1], color, normals[4]},
		{positions[2], color, normals[4]},
		{positions[3], color, normals[4]},
		{positions[4], color, normals[4]},
	};

	boundingSphere = MakeBoundingSphere(points, ARRAYSIZE(points));

	D3D11_BUFFER_DESC desc{ 0 };
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(vertices);
	desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data{ 0 };
	data.pSysMem = vertices;

	auto result = device->CreateBuffer(&desc, &data, &vertexBuffer);
	if (FAILED(result))
		throw exception("�� ���� ������� ����� ������.");


	UINT indices[]{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		12, 14, 15
	};
	indexCount = ARRAYSIZE(indices);

	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(indices);

	data.pSysMem = indices;

	result = device->CreateBuffer(&desc, &data, &indexBuffer);
	if (FAILED(result))
		throw exception("�� ���� ������� ����� ��������.");

	auto vertexShaderCode = LoadShader("BoneVS.cso");
	result = device->CreateVertexShader(vertexShaderCode.data(), vertexShaderCode.size(), nullptr, &vertexShader);
	if (FAILED(result))
		throw exception("�� ���� ������� ��������� ������.");

	auto pixelShaderCode = LoadShader("BonePS.cso");
	result = device->CreatePixelShader(pixelShaderCode.data(), pixelShaderCode.size(), nullptr, &pixelShader);
	if (FAILED(result))
		throw exception("�� ���� ������� ���������� ������.");

	result = device->CreateInputLayout(inputLayoutElements, ARRAYSIZE(inputLayoutElements), vertexShaderCode.data(), vertexShaderCode.size(), &inputLayout);
	if (FAILED(result))
		throw exception("�� ���� ������� �������� ������ (Input Layout).");
	context->IASetInputLayout(inputLayout.Get());

	// ������� ������ ����������
	desc = { 0 };
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	/*desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;*/
	desc.ByteWidth = sizeof(VSPerFrameConstantBuffer);
	result = device->CreateBuffer(&desc, 0, &vsPerFrameConstantBuffer);
	if (FAILED(result))
		throw exception("�� ���� ������� ����� ��������.");

	desc.ByteWidth = sizeof(VSPerObjectConstantBuffer);
	result = device->CreateBuffer(&desc, 0, &vsPerObjectConstantBuffer);
	if (FAILED(result))
		throw exception("�� ���� ������� ����� ��������.");

	desc.ByteWidth = sizeof(PSPerFrameConstantBuffer);
	result = device->CreateBuffer(&desc, 0, &psPerFrameConstantBuffer);
	if (FAILED(result))
		throw exception("�� ���� ������� ����� ��������.");
}

void CreateBones() {
	XMMATRIX identity, m;
	identity = XMMatrixIdentity();

	Bone b3{ XMMatrixTranslation(0, -100, 0), identity, {} };
	Bone b2{ XMMatrixTranslation(0, -100, 0), identity, {b3} };
	Bone b1{ XMMatrixTranslation(0, -100, 0), identity, {b2} };
	Bone b0{ XMMatrixTranslation(0,  150, 0), identity, {b1} };
	rootBone = b0;

	UpdateBones(&rootBone);
}
void ReleaseBones() {
}

void CreateAnimations() {
	XMVECTOR qs[3];
	qs[2] = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(-50.0f));
	qs[0] = XMQuaternionNormalize(qs[0]);
	qs[1] = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(50.0f));

	secondBoneAnimChannel = new Channel(
		&rootBone.children.front(),
		{
			{0.0f, {0, -100, 0}, {1.0f, 1.0f, 1.0f}, qs[0]},
			{1.0f, {0, -100, 0}, {1.0f, 1.0f, 1.0f}, qs[1]},
			{2.0f, {0, -100, 0}, {1.0f, 1.0f, 1.0f}, qs[0]},
			{3.0f, {0, -100, 0}, {1.0f, 1.0f, 1.0f}, qs[2]},
			{4.0f, {0, -100, 0}, {1.0f, 1.0f, 1.0f}, qs[0]},
		},
		4.0f
		);
}

void CreateLight() {
	light.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	light.diffuse = { 1.0f, 1.0f, 1.0f,1.0f };
	light.direction = { 0.0f, 0.0f, 1.0f };
}

void UpdatePerFrameConstantBuffers() {
	XMMATRIX m;

	XMStoreFloat4x4(&vsPerFrameBuffer.view, XMMatrixTranspose(XMMatrixLookAtLH(
		XMVectorSet(20.0f, 20.0f, -1000.0f, 1.0f),
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(0, 1, 0, 0))));
	XMStoreFloat4x4(&vsPerFrameBuffer.proj, XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4, windowWidth / (float)windowHeight, 0.1f, 10000.0f)));

	psPerFrameBuffer.light = light;
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, LPSTR, int) {
	try {
		InitializeCore(hinstance);
		CreateLight();
		UpdatePerFrameConstantBuffers();
		CreateModel();
		CreateBones();
		CreateAnimations();
		Run();
		ReleaseBones();
		ReleaseCore();
	}
	catch (exception ex) {
		MessageBox(0, ex.what(), "exception", 0);
	}
	return 0;
}
