#include <d3dx9.h>
#include <vector>
#include <wrl.h>

#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

using namespace std;
using namespace Microsoft::WRL;

#define BONE_COUNT 4

struct KeyFrame
{
	float time;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scaling;
	D3DXQUATERNION rotation;
};

struct Bone
{
	D3DXMATRIX offset, animated;
	std::vector<Bone> children;
};

struct Channel
{
	Bone* bone;
	std::vector<KeyFrame> keyFrames;
};

struct Vertex
{
	D3DXVECTOR4 position;
	D3DCOLOR color;
	int boneIndices[BONE_COUNT];
	float boneWeights[BONE_COUNT];
};


class Animation {
	double duration{ 0 };
	float time{ 0 };
	std::vector<Channel> channels;

	void Interpolate(const KeyFrame& k0, const KeyFrame& k1, D3DXMATRIX& out) {
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
		out = s * r * t; // r * s * t in the original
	}

public:
	Animation()
	{

	}
	void Update(float delta) {
		time += delta;
		if (time >= duration)
			time = 0;
		for (UINT i = 0; i < channels.size(); ++i) {
			auto& keyFrames = channels[i].keyFrames;
			UINT k0 = 0;
			while (true)
			{
				if (k0 + 1 >= keyFrames.size())
					break;
				if (time < keyFrames[k0 + 1].time)
				{
					Interpolate(keyFrames[k0], keyFrames[k0 + 1], channels[i].bone->animated);
					break;
				}
				else {
					++k0;
				}
			}
		}
	}
	void SetDuration(double duration) { this->duration = duration; }
	Channel* CreateChannel() {
		channels.push_back(Channel());
		return &channels.back();
	}
};




LPCSTR className = "SKELETAL WINDOW";
HINSTANCE hinstance;
HWND hwnd;
ComPtr<IDirect3D9> direct3d;
ComPtr<IDirect3DDevice9> device3d;
int windowWidth, windowHeight;
constexpr DWORD VERTEX_SIZE = sizeof(Vertex);
constexpr DWORD VERTEX_FORMAT = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
ComPtr<IDirect3DVertexBuffer9> vertexBuffer;
ComPtr<IDirect3DIndexBuffer9> indexBuffer;
int indexCount;


LRESULT WindowProcessor(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		//KeyDown((BYTE)w);
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

	return true;
}
void UpdateGame() {

}

void RenderModel() {
	device3d->SetFVF(VERTEX_FORMAT);
	device3d->SetStreamSource(0, vertexBuffer.Get(), 0, VERTEX_SIZE);
	device3d->SetIndices(indexBuffer.Get());
	device3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, indexCount, 0, indexCount / 3);
}

void RenderGame() {
	device3d->Clear(0, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0xff404040, 1.0f, 0);
	device3d->BeginScene();

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

	Vertex vertices[]{
		{{xc - 100,	yc - 200, 0, 1.0f}, 0xffff0000 },
		{{xc + 100,	yc - 200, 0, 1.0f}, 0xffff0000 },
		{{xc,		yc - 100, 0, 1.0f}, 0xffff0000 },

		{{xc - 100,	yc - 100, 0, 1.0f}, 0xff00ff00 },
		{{xc + 100,	yc - 100, 0, 1.0f}, 0xff00ff00 },
		{{xc,		yc,		 0, 1.0f}, 0xff00ff00 },

		{{xc - 100,	yc,		 0, 1.0f}, 0xff0000ff },
		{{xc + 100,	yc,		 0, 1.0f}, 0xff0000ff },
		{{xc,		yc + 100, 0, 1.0f}, 0xff0000ff },

		{{xc - 100,	yc + 100, 0, 1.0f}, 0xffffff00 },
		{{xc + 100,	yc + 100, 0, 1.0f}, 0xffffff00 },
		{{xc,		yc + 200, 0, 1.0f}, 0xffffff00 },
	};

	device3d->CreateVertexBuffer(sizeof(vertices), D3DUSAGE_WRITEONLY, VERTEX_FORMAT, D3DPOOL_MANAGED, &vertexBuffer, nullptr);
	LPVOID data;
	vertexBuffer->Lock(0, 0, &data, 0);
	memcpy(data, vertices, sizeof(vertices));
	vertexBuffer->Unlock();

	UINT indices[]{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11
	};
	indexCount = ARRAYSIZE(indices);
	device3d->CreateIndexBuffer(sizeof(indices), D3DUSAGE_WRITEONLY, D3DFORMAT::D3DFMT_INDEX32, D3DPOOL_MANAGED, &indexBuffer, nullptr);
	indexBuffer->Lock(0, 0, &data, 0);
	memcpy(data, indices, sizeof(indices));
	indexBuffer->Unlock();

}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, LPSTR, int) {
	InitializeCore(hinstance);
	CreateModel();
	Run();
	ReleaseCore();
	return 0;
}
