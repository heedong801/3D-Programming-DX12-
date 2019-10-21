//float4 VSMain(uint nVertexID : SV_VertexID) : SV_POSITION
//{ 
//	float4 output;
//	//프리미티브(삼각형)를 구성하는 정점의 인덱스(SV_VertexID)에 따라 정점을 반환한다. 
//	//정점의 위치 좌표는 변환이 된 좌표(SV_POSITION)이다. 즉, 투영좌표계의 좌표이다.
//	
//	if (nVertexID == 0) 
//		output = float4(0.0, 0.5, 0.5, 1.0); 
//	else if (nVertexID == 1) 
//		output = float4(0.5, -0.5, 0.5, 1.0); 
//	else if (nVertexID == 2)
//		output = float4(-0.5, -0.5, 0.5, 1.0);
//
//	return(output);
//}
////픽셀 셰이더를 정의한다. 
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET 
//{ 
//	//프리미티브의 모든 픽셀의 색상을 노란색으로 반환한다.
//
//	return(float4(1.0f, 1.0f, 0.0f, 1.0f));
//	//Yellow 
//}

//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_INPUT 
{ 
	 float3 position : POSITION;
	 float4 color : COLOR;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다. 
struct VS_OUTPUT
{
	 float4 position : SV_POSITION;
	 float4 color : COLOR;
};

//정점 셰이더를 정의한다. 
VS_OUTPUT VSMain(VS_INPUT input)
{ 
	VS_OUTPUT output;
	output.position = float4(input.position, 1.0f); 
	output.color = input.color;
	
	return(output);
}

//픽셀 셰이더를 정의한다. 
float4 PSMain(VS_OUTPUT input) : SV_TARGET 
{ 
	return(input.color);
}