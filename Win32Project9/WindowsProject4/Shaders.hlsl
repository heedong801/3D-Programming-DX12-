//float4 VSMain(uint nVertexID : SV_VertexID) : SV_POSITION
//{ 
//	float4 output;
//	//������Ƽ��(�ﰢ��)�� �����ϴ� ������ �ε���(SV_VertexID)�� ���� ������ ��ȯ�Ѵ�. 
//	//������ ��ġ ��ǥ�� ��ȯ�� �� ��ǥ(SV_POSITION)�̴�. ��, ������ǥ���� ��ǥ�̴�.
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
////�ȼ� ���̴��� �����Ѵ�. 
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET 
//{ 
//	//������Ƽ���� ��� �ȼ��� ������ ��������� ��ȯ�Ѵ�.
//
//	return(float4(1.0f, 1.0f, 0.0f, 1.0f));
//	//Yellow 
//}

//���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�. 
struct VS_INPUT 
{ 
	 float3 position : POSITION;
	 float4 color : COLOR;
};

//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�. 
struct VS_OUTPUT
{
	 float4 position : SV_POSITION;
	 float4 color : COLOR;
};

//���� ���̴��� �����Ѵ�. 
VS_OUTPUT VSMain(VS_INPUT input)
{ 
	VS_OUTPUT output;
	output.position = float4(input.position, 1.0f); 
	output.color = input.color;
	
	return(output);
}

//�ȼ� ���̴��� �����Ѵ�. 
float4 PSMain(VS_OUTPUT input) : SV_TARGET 
{ 
	return(input.color);
}