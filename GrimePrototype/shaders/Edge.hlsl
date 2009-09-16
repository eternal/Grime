sampler2D ScreenMapSampler : register(s1);
sampler2D DepthMapSampler : register(s2);

float unpackFloat(const float4 value)
{
	float extractedDistance = (value.r + (value.g / 256.0));
	return extractedDistance;
}

float calcSample(float depth, float2 texCoords)
{
	float4 tempDepth = tex2D(DepthMapSampler, texCoords);
	float otherDepth = unpackFloat(tempDepth);
	return min(abs(otherDepth - depth) * 25.0, 1.0);
}

float4 pixelMain (
float2 Texcoords 	: TEXCOORD0	
) : COLOR0
{
	const float2 offsetArray[4] = 
	{
		float2( 1.0 / SCREENX,0),
		float2( -1.0 / SCREENX,0),
		float2(0, 1.0 / SCREENY),
		float2(0, -1.0 / SCREENY)
	};

	float depth = unpackFloat(tex2D(DepthMapSampler, Texcoords.xy));
	float outline = float4(0.0, 0.0, 0.0, 0.0);

	for(int i = 0;i < 4;i++) outline += calcSample(depth, Texcoords + offsetArray[i]);

	return tex2D(ScreenMapSampler, Texcoords.xy) * min(1.0 - outline, 1.0);
}