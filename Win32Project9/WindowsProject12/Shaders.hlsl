//플레이어 객체의 데이터를 위한 상수 버퍼 
cbuffer cbPlayerInfo : register(b0) 
{
	matrix gmtxPlayerWorld : packoffset(c0);
};

//카메라 객체의 데이터를 위한 상수 버퍼(스펙큘러 조명 계산을 위하여 카메라의 위치 벡터를 추가)
cbuffer cbCameraInfo : register(b1) 
{
	matrix gmtxView : packoffset(c0); 
	matrix gmtxProjection : packoffset(c4); 
	float3 gvCameraPosition : packoffset(c8); 
};

//게임 객체의 데이터를 위한 상수 버퍼(게임 객체에 대한 재질 번호를 추가)
cbuffer cbGameObjectInfo : register(b2) 
{
	matrix gmtxGameObject : packoffset(c0); 
	uint gnMaterial : packoffset(c4);
};

//상수 버퍼 선언 부분 다음에 조명 계산 쉐이더 함수를 포함하고 있는 파일을 포함하기 위하여 다음을 추가한다. 
#include "Light.hlsl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSGameObject(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSGameObject(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input) 
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;
	
	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

//정점 조명을 사용
#define _WITH_VERTEX_LIGHTING

//정점 쉐이더의 입력 정점 구조 
struct VS_LIGHTING_INPUT 
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

//정점 쉐이더의 출력 정점 구조
struct VS_LIGHTING_OUTPUT 
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
#ifdef _WITH_VERTEX_LIGHTING 
	float4 color : COLOR;
#else 
	float3 normalW : NORMAL;
#endif 
};
	
//정점 쉐이더 함수
VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;
	
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection); 
	
	float3 normalW = mul(input.normal, (float3x3)gmtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING 
	output.color = Lighting(output.positionW, normalize(normalW));
#else 
	output.normalW = normalW; 
#endif 
	return(output);
}
	
//픽셀 쉐이더 함수 
float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET 
{
#ifdef _WITH_VERTEX_LIGHTING 
	return(input.color);
#else
	float3 normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, normalW);
	
	return(color);
#endif 
}
