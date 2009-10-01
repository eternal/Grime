sampler2D ColorMapSampler : register(s0);

float4 pixelMain( float2 TexCoords : TEXCOORD0 ) : COLOR0 
{
	half4 Color = tex2D(ColorMapSampler,TexCoords);
	//Color.r = round(Color.r*30)/30;
	//Color.g = round(Color.g*30)/30;
	//Color.b = round(Color.b*30)/30;

	const float threshold = 0.2f;

	const int NUM = 9;
	const float2 c[NUM] =
	{
		float2(-0.0078125, 0.0078125), 
		float2( 0.00 ,     0.0078125),
		float2( 0.0078125, 0.0078125),
		float2(-0.0078125, 0.00 ),
		float2( 0.0,       0.0),
		float2( 0.0078125, 0.007 ),
		float2(-0.0078125,-0.0078125),
		float2( 0.00 ,    -0.0078125),
		float2( 0.0078125,-0.0078125),
	};	
	
	int i;
	float3 col[NUM];
	for (i=0; i < NUM; i++)
	{
		col[i] = tex2D(ColorMapSampler, TexCoords + 0.2*c[i]);
	}
	
	float3 rgb2lum = float3(0.30, 0.59, 0.11);
	float lum[NUM];
	for (i = 0; i < NUM; i++)
	{
		lum[i] = dot(col[i].xyz, rgb2lum);
	}
	float x = lum[2]+  lum[8]+2*lum[5]-lum[0]-2*lum[3]-lum[6];
	float y = lum[6]+2*lum[7]+  lum[8]-lum[0]-2*lum[1]-lum[2];
	float edge =(x*x + y*y < threshold)? 1.0:0.0;
	
	Color.rgb *= edge;
	return Color;

}

