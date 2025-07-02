struct VSInput
{
    float3 position : TEXCOORD0;
    float4 color : TEXCOORD1;
    float2 tex_coord : TEXCOORD2;
};

struct VSOutput
{
    float4 color : TEXCOORD0;
    float2 tex_coord : TEXCOORD1;
    float4 position : SV_Position;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.color = input.color;
    output.tex_coord = input.tex_coord;
    output.position = float4(input.position, 0);
    return output;
}
