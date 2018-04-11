Texture2D texture0  : register(t0);
Texture2D texture1  : register(t1);
Texture2D texture2  : register(t2);
Texture2D texture3  : register(t3);
Texture2D texture4  : register(t4);


SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	return textureColor;
}