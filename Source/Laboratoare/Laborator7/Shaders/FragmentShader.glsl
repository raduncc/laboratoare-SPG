#version 410

layout(location = 0) in vec2 texture_coord;

uniform sampler2D textureImage;
uniform ivec2 screenSize;
uniform int flipVertical;

// 0 - original
// 1 - grayscale
// 2 - blur
uniform int outputMode = 2;

// Flip texture horizontally when
vec2 textureCoord = vec2(texture_coord.x, (flipVertical != 0) ? 1 - texture_coord.y : texture_coord.y);

layout(location = 0) out vec4 out_color;

float arr_r[9], arr_g[9], arr_b[9];


float sort_and_get_median(float a[9])
{
   int i, j;
   for (i=0;i<8;++i){
	   for(j=0;j<8-i;++j){
		if (a[j]>a[j+1]){
			float aux=a[j];
			a[j]=a[j+1];
			a[j+1] = aux;
		}
	   }
   }
   return a[4];
}


vec4 grayscale()
{
	vec4 color = texture(textureImage, textureCoord);
	float gray = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b; 
	return vec4(gray, gray, gray,  0);
}

vec4 blur(int blurRadius)
{
	vec2 texelSize = 1.0f / screenSize;
	vec4 sum = vec4(0);
	for(int i = -blurRadius; i <= blurRadius; i++)
	{
		for(int j = -blurRadius; j <= blurRadius; j++)
		{
			sum += texture(textureImage, textureCoord + vec2(i, j) * texelSize);
		}
	}
		
	float samples = pow((2 * blurRadius + 1), 2);
	return sum / samples;
}




vec4 medie(){
	vec2 texelSize = 1.0f / screenSize;
	vec4 color = texture(textureImage, textureCoord);
	vec4 sum = vec4(0);
	if (textureCoord.x==0||textureCoord.y==0||textureCoord.y==screenSize.y-1||textureCoord.x==screenSize.x-1) {
		return vec4(color.r,color.g,color.b,0);
	} else {
		for(int i = -1; i <= 1; i++)
		{
			for(int j = -1; j <= 1; j++)
			{
				sum += texture(textureImage, textureCoord + vec2(i, j) * texelSize);
			}
		}
		sum -= color;
		return sum/8;
	}
}



vec4 gauss(){
	mat3 matrix;
	matrix[0][0] = 1;
	matrix[0][1] = 2;
	matrix[0][2] = 1;
	matrix[1][0] = 2;
	matrix[1][1] = 4;
	matrix[1][2] = 2;
	matrix[2][0] = 1;
	matrix[2][1] = 2;
	matrix[2][2] = 1;
	vec2 texelSize = 1.0f / screenSize;
	vec4 color = texture(textureImage, textureCoord);
	vec4 sum = vec4(0);
	if (textureCoord.x==0||textureCoord.y==0||textureCoord.y==screenSize.y-1||textureCoord.x==screenSize.x-1) {
		return vec4(color.r,color.g,color.b,0);
	} else {
		for(int i = -1; i <= 1; i++)
		{
			for(int j = -1; j <= 1; j++)
			{
				sum += texture(textureImage, textureCoord + vec2(i, j) * texelSize) * matrix[i+1][j+1];
			}
		}
		sum -= color;
		return sum/16;
	}
}


vec4 median(){
	vec2 texelSize = 1.0f / screenSize;
	vec4 color = texture(textureImage, textureCoord);
	if (textureCoord.x==0||textureCoord.y==0||textureCoord.y==screenSize.y-1||textureCoord.x==screenSize.x-1) {
		return vec4(color.r,color.g,color.b,0);
	} else {
		
		arr_r[0] = texture(textureImage, textureCoord + vec2(0, 0) * texelSize).r;
		arr_g[0] = texture(textureImage, textureCoord + vec2(0, 0) * texelSize).g;
		arr_b[0] = texture(textureImage, textureCoord + vec2(0, 0) * texelSize).b;

		arr_r[1] = texture(textureImage, textureCoord + vec2(1, 0) * texelSize).r;
		arr_g[1] = texture(textureImage, textureCoord + vec2(1, 0) * texelSize).g;
		arr_b[1] = texture(textureImage, textureCoord + vec2(1, 0) * texelSize).b;

		arr_r[2] = texture(textureImage, textureCoord + vec2(-1, 0) * texelSize).r;
		arr_g[2] = texture(textureImage, textureCoord + vec2(-1, 0) * texelSize).g;
		arr_b[2] = texture(textureImage, textureCoord + vec2(-1, 0) * texelSize).b;

		arr_r[3] = texture(textureImage, textureCoord + vec2(0, 1) * texelSize).r;
		arr_g[3] = texture(textureImage, textureCoord + vec2(0, 1) * texelSize).g;
		arr_b[3] = texture(textureImage, textureCoord + vec2(0, 1) * texelSize).b;

		arr_r[4] = texture(textureImage, textureCoord + vec2(1, 1) * texelSize).r;
		arr_g[4] = texture(textureImage, textureCoord + vec2(1, 1) * texelSize).g;
		arr_b[4] = texture(textureImage, textureCoord + vec2(1, 1) * texelSize).b;

		arr_r[5] = texture(textureImage, textureCoord + vec2(-1, 1) * texelSize).r;
		arr_g[5] = texture(textureImage, textureCoord + vec2(-1, 1) * texelSize).g;
		arr_b[5] = texture(textureImage, textureCoord + vec2(-1, 1) * texelSize).b;

		arr_r[6] = texture(textureImage, textureCoord + vec2(0, -1) * texelSize).r;
		arr_g[6] = texture(textureImage, textureCoord + vec2(0, -1) * texelSize).g;
		arr_b[6] = texture(textureImage, textureCoord + vec2(0, -1) * texelSize).b;

		arr_r[7] = texture(textureImage, textureCoord + vec2(1, -1) * texelSize).r;
		arr_g[7] = texture(textureImage, textureCoord + vec2(1, -1) * texelSize).g;
		arr_b[7] = texture(textureImage, textureCoord + vec2(1, -1) * texelSize).b;

		arr_r[8] = texture(textureImage, textureCoord + vec2(-1, -1) * texelSize).r;
		arr_g[8] = texture(textureImage, textureCoord + vec2(-1, -1) * texelSize).g;
		arr_b[8] = texture(textureImage, textureCoord + vec2(-1, -1) * texelSize).b;

		float color_r = sort_and_get_median(arr_r);
		float color_g = sort_and_get_median(arr_g);
		float color_b = sort_and_get_median(arr_b);
		
		return vec4(color_r, color_g, color_b, 0);
	}


}


void main()
{
	switch (outputMode)
	{
		case 2:
		{
			out_color = grayscale();
			break;
		}

		case 3:
		{
			out_color = medie();
			break;
		}
		case 4:
		{
			out_color = gauss();
			break;
		}
		case 5:
		{
			out_color = median();
			break;
		}
		default:
			out_color = texture(textureImage, textureCoord);
			break;
	}
}