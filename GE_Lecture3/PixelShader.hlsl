struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Colour : COLOUR;
};

float4 PS(PS_INPUT input) : SV_Target0
{
    // colour from vertex hsader
    return float4(input.Colour, 1.0);//1.0 is alpha value
}
