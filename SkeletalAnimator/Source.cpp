#include <d3dx9.h>
#include <vector>


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


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, LPSTR, int) {

}
