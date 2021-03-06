#include <d3dx9.h>
#include <vector>
#include <wrl.h>
#include <memory>

#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

using namespace std;
using namespace Microsoft::WRL;

#define BONE_COUNT 4


struct Bone;
void UpdateBones(Bone* bone, LPD3DXMATRIX parentMatrix = nullptr);
extern Bone rootBone;

struct KeyFrame
{
	float time;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scaling;
	/*float rotation;*/
	D3DXQUATERNION rotation;
};

struct Bone
{
	D3DXMATRIX offset, animated;
	std::vector<Bone> children;
};

class Channel
{
	Bone* bone; // ����� 
	std::vector<KeyFrame> keyFrames; // �������� �����
	const float length; // ����� �������� � ��������
	float time; // ������� �����

	D3DXMATRIX Interpolate(const KeyFrame& k0, const KeyFrame& k1) {
		float t0 = k0.time, t1 = k1.time;
		float lerpTime = (time - t0) / (t1 - t0);
		D3DXVECTOR3 lerpedT, lerpedS;
		D3DXQUATERNION lerpedR;
		D3DXVec3Lerp(&lerpedT, &k0.position, &k1.position, lerpTime);
		D3DXVec3Lerp(&lerpedS, &k0.scaling, &k1.scaling, lerpTime);
		D3DXQuaternionSlerp(&lerpedR, &k0.rotation, &k1.rotation, lerpTime);
		D3DXMATRIX t, r, s;
		D3DXMatrixTranslation(&t, lerpedT.x, lerpedT.y, lerpedT.z);
		D3DXMatrixScaling(&t, lerpedS.x, lerpedS.y, lerpedS.z);
		D3DXMatrixRotationQuaternion(&r, &lerpedR);
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

	D3DXMATRIX Get() {
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
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	int boneIndices[BONE_COUNT];
	float boneWeights[BONE_COUNT];
};


//class Animation {
//	double duration{ 0 };
//	float time{ 0 };
//	std::vector<Channel> channels;
//
//	void Interpolate(const KeyFrame& k0, const KeyFrame& k1, D3DXMATRIX& out) {
//		float t0 = k0.time, t1 = k1.time;
//		float lerpTime = (time - t0) / (t1 - t0);
//		D3DXVECTOR3 lerpedT, lerpedS;
//		D3DXQUATERNION lerpedR;
//		D3DXVec3Lerp(&lerpedT, &k0.position, &k1.position, lerpTime);
//		D3DXVec3Lerp(&lerpedS, &k0.scaling, &k1.scaling, lerpTime);
//		D3DXQuaternionSlerp(&lerpedR, &k0.rotation, &k1.rotation, lerpTime);
//		D3DXMATRIX t, r, s;
//		D3DXMatrixTranslation(&t, lerpedT.x, lerpedT.y, lerpedT.z);
//		D3DXMatrixScaling(&t, lerpedS.x, lerpedS.y, lerpedS.z);
//		D3DXMatrixRotationQuaternion(&r, &lerpedR);
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
ComPtr<IDirect3D9> direct3d;
ComPtr<IDirect3DDevice9> device3d;
int windowWidth, windowHeight;
constexpr DWORD VERTEX_SIZE = sizeof(Vertex);
constexpr DWORD VERTEX_FORMAT = D3DFVF_XYZ | D3DFVF_NORMAL;
ComPtr<IDirect3DVertexBuffer9> vertexBuffer;
ComPtr<IDirect3DIndexBuffer9> indexBuffer;
int indexCount;
Bone rootBone;
Channel* secondBoneAnimChannel;


void UpdateBones(Bone* bone, LPD3DXMATRIX parentMatrix) {
	if (parentMatrix)
		D3DXMatrixMultiply(&bone->animated, &bone->offset, parentMatrix);
	else
		bone->animated = bone->offset;

	for (auto&& c : bone->children) {
		UpdateBones(&c, &bone->animated);
	}
}
void RotateBone(Bone* bone, float degree) {
	if (!bone)return;
	D3DXMATRIX r;
	D3DXMatrixRotationZ(&r, D3DXToRadian(degree));
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

	if (!(direct3d = Direct3DCreate9(D3D_SDK_VERSION)))
		return false;

	D3DPRESENT_PARAMETERS pp{ 0 };
	pp.AutoDepthStencilFormat = D3DFMT_D24S8;
	pp.BackBufferFormat = D3DFMT_A8R8G8B8;

	pp.BackBufferHeight = windowHeight;
	pp.BackBufferWidth = windowWidth;
	pp.EnableAutoDepthStencil = TRUE;
	pp.hDeviceWindow = hwnd;
	pp.SwapEffect = D3DSWAPEFFECT::D3DSWAPEFFECT_DISCARD;
	pp.Windowed = TRUE;

	HRESULT result = S_OK;
	if (FAILED(result = direct3d->CreateDevice(0, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device3d)))
		return false;

	device3d->SetRenderState(D3DRS_LIGHTING, TRUE);

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

void RenderModel() {
	device3d->SetFVF(VERTEX_FORMAT);
	device3d->SetStreamSource(0, vertexBuffer.Get(), 0, VERTEX_SIZE);
	device3d->SetIndices(indexBuffer.Get());

	auto bone = &rootBone;
	D3DMATERIAL9 material{
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.2f, 0.2f, 0.2f, 1.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},
		1.0f
	};

	device3d->SetMaterial(&material);
	device3d->SetTransform(D3DTS_WORLD, &bone->animated);
	device3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 5, 0, 6);

	material.Diffuse = { 0.0f, 1.0f, 0.0f, 1.0f };
	device3d->SetMaterial(&material);
	bone = &bone->children.front();
	device3d->SetTransform(D3DTS_WORLD, &bone->animated);
	device3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 5, 0, 6);

	material.Diffuse = { 1.0f, 1.0f, 0.0f, 1.0f };
	device3d->SetMaterial(&material);
	bone = &bone->children.front();
	device3d->SetTransform(D3DTS_WORLD, &bone->animated);
	device3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 5, 0, 6);

	material.Diffuse = { 0.0f, 0.0f, 1.0f, 1.0f };
	device3d->SetMaterial(&material);
	bone = &bone->children.front();
	device3d->SetTransform(D3DTS_WORLD, &bone->animated);
	device3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 5, 0, 6);

}
void RenderLight() {
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Direction = { 0.0f, 0.0f, 1.0f };
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	device3d->SetLight(0, &light);
	device3d->LightEnable(0, TRUE);
}
void RenderGame() {
	device3d->Clear(0, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0xff404040, 1.0f, 0);
	device3d->BeginScene();

	D3DXMATRIX m;
	device3d->SetTransform(D3DTS_VIEW, D3DXMatrixLookAtLH(&m, &D3DXVECTOR3(20, 20, -1000), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0)));
	device3d->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&m, D3DX_PI / 4.0f, windowWidth / (float)windowHeight, 0.1f, 10000.0f));

	RenderLight();
	RenderModel();

	device3d->EndScene();
	device3d->Present(nullptr, nullptr, nullptr, nullptr);
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

		UpdateGame();
		RenderGame();

		Sleep(1);
	}
}
void ReleaseCore() {
	indexBuffer = nullptr;
	vertexBuffer = nullptr;
	device3d = nullptr;
	direct3d = nullptr;
}

void CreateModel() {
	float xc = windowWidth / 2.0f;
	float yc = windowHeight / 2.0f;
	float wi = 100.0f;

	D3DXVECTOR3 normals[5]{
		{0.0f, -1.0f, 0.0f},
	{-1.0f, 0.5f, -1.0f},
	{-1.0f, 0.5f, 1.0f},
	{1.0f, 0.5f, 1.0f},
	{1.0f, 0.5f, -1.0f},
	};
	for (auto&& n : normals) {
		D3DXVec3Normalize(&n, &n);
	}

	Vertex vertices[]{
		{{ 0, -wi,  0}, normals[0] }, // A
		{{-wi, 0, -wi}, normals[1] }, // B
		{{-wi, 0,  wi}, normals[2] }, // C
		{{ wi, 0,  wi}, normals[3] }, // D
		{{ wi, 0, -wi}, normals[4] }, // E
	};

	device3d->CreateVertexBuffer(sizeof(vertices), D3DUSAGE_WRITEONLY, VERTEX_FORMAT, D3DPOOL_MANAGED, &vertexBuffer, nullptr);
	LPVOID data;
	vertexBuffer->Lock(0, 0, &data, 0);
	memcpy(data, vertices, sizeof(vertices));
	vertexBuffer->Unlock();

	UINT indices[]{
		0, 2, 1,
		0, 3, 2,
		0, 4, 3,
		0, 1, 4,
		1, 2, 4,
		2, 3, 4
	};
	indexCount = ARRAYSIZE(indices);
	device3d->CreateIndexBuffer(sizeof(indices), D3DUSAGE_WRITEONLY, D3DFORMAT::D3DFMT_INDEX32, D3DPOOL_MANAGED, &indexBuffer, nullptr);
	indexBuffer->Lock(0, 0, &data, 0);
	memcpy(data, indices, sizeof(indices));
	indexBuffer->Unlock();
}

void CreateBones() {
	D3DXMATRIX identity, m;
	D3DXMatrixIdentity(&identity);

	D3DXMatrixTranslation(&m, 0, -100, 0);
	Bone b3{ m, identity, {} };
	D3DXMatrixTranslation(&m, 0, -100, 0);
	Bone b2{ m, identity, {b3} };
	D3DXMatrixTranslation(&m, 0, -100, 0);
	Bone b1{ m, identity, {b2} };
	D3DXMatrixTranslation(&m, 0, 150, 0);
	Bone b0{ m, identity, {b1} };
	rootBone = b0;

	UpdateBones(&rootBone);
}
void ReleaseBones() {
}

void CreateAnimations() {
	D3DXQUATERNION qs[3];
	D3DXQuaternionNormalize(&qs[0], &qs[0]);
	D3DXQuaternionRotationYawPitchRoll(&qs[1], D3DXToRadian(50.0f), 0.0f, 0.0f);
	D3DXQuaternionRotationYawPitchRoll(&qs[2], D3DXToRadian(-50.0f), 0.0f, 0.0f);

	D3DXMATRIX identity, m[4];
	D3DXMatrixIdentity(&identity);

	D3DXMatrixTranslation(&m[3], 0, -100, 0);
	D3DXMatrixTranslation(&m[2], 0, -100, 0);
	D3DXMatrixTranslation(&m[1], 0, -100, 0);
	D3DXMatrixTranslation(&m[0], 0, 150, 0);

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

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, LPSTR, int) {
	InitializeCore(hinstance);
	CreateModel();
	CreateBones();
	CreateAnimations();
	Run();
	ReleaseBones();
	ReleaseCore();
	return 0;
}
