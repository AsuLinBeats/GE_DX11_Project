// connect btw CPU code and GPU

struct VS_INPUT
{
    float4 Pos : POS;
    float3 Colour : COLOUR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION; // interpolate position, (clip space).
    float3 Colour : COLOUR;
};

PS_INPUT VS(VS_INPUT input)
{
	// vertex shader
    PS_INPUT output;
    output.Pos = input.Pos;
    output.Colour = input.Colour;
    return output;
}