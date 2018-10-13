
cbuffer PerFrameBuffer : register(b0)
{
    float3 direction;
    float pad;
    float4 ambient;
    float4 diffuse;
};

struct Input
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct Output
{
    float4 color : SV_Target;
};

Output main(Input input)
{
    input.normal = normalize(input.normal);

    float3 color = input.color * ambient;

    color += saturate(dot(direction, input.normal) * diffuse * input.color);

    Output output;
    output.color = float4(color, input.color.a);
    input.position.w = 1.0f;

    return output;
}
