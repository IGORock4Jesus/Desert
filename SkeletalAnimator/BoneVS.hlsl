
// �������� ����������� ������ ������ ��� ���������� ������
cbuffer PerFrameBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projMatrix;
    matrix boneMatrices[32];
};

cbuffer PerModelBuffer : register(b1)
{
    matrix worldMatrix;
}

struct Input
{
    float4 position : POSITION;
    float4 color : COLOR;
    float4 boneWeights : BLENDWEIGHT;
    int4 boneIndices : BLENDINDICES;
};

struct Output
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

Output main(Input input)
{
    Output output;
    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projMatrix);

    output.color = input.color;

    return output;
}