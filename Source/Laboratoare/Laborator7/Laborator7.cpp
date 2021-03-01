#include "Laborator7.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

Laborator7::Laborator7()
{
	outputMode = 0;
	gpuProcessing = false;
	saveScreenToImage = false;
	window->SetSize(600, 600);
}

Laborator7::~Laborator7()
{
}

FrameBuffer *processed;

void Laborator7::Init()
{
	// Load default texture fore imagine processing 
	originalImage = TextureManager::LoadTexture("Source/Laboratoare/Laborator7/image2.jpg", nullptr, "image", true, true);
	processedImage = TextureManager::LoadTexture("Source/Laboratoare/Laborator7/image2.jpg", nullptr, "newImage", true, true);

	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	static std::string shaderPath = "Source/Laboratoare/Laborator7/Shaders/";

	// Create a shader program for particle system
	{
		Shader *shader = new Shader("ImageProcessing");
		shader->AddShader((shaderPath + "VertexShader.glsl").c_str(), GL_VERTEX_SHADER);
		shader->AddShader((shaderPath + "FragmentShader.glsl").c_str(), GL_FRAGMENT_SHADER);

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

void Laborator7::FrameStart()
{
}

void Laborator7::Update(float deltaTimeSeconds)
{
	ClearScreen();

	auto shader = shaders["ImageProcessing"];
	shader->Use();

	if (saveScreenToImage)
	{
		window->SetSize(originalImage->GetWidth(), originalImage->GetHeight());
	}

	int flip_loc = shader->GetUniformLocation("flipVertical");
	glUniform1i(flip_loc, saveScreenToImage ? 0 : 1);

	int screenSize_loc = shader->GetUniformLocation("screenSize");
	glm::ivec2 resolution = window->GetResolution();
	glUniform2i(screenSize_loc, resolution.x, resolution.y);

	int outputMode_loc = shader->GetUniformLocation("outputMode");
	glUniform1i(outputMode_loc, outputMode);

	int gpuProcessing_loc = shader->GetUniformLocation("outputMode");
	glUniform1i(outputMode_loc, outputMode);

	int locTexture = shader->GetUniformLocation("textureImage");
	glUniform1i(locTexture, 0);
	auto textureImage = (gpuProcessing == true) ? originalImage : processedImage;
	textureImage->BindToTextureUnit(GL_TEXTURE0);

	RenderMesh(meshes["quad"], shader, glm::mat4(1));

	if (saveScreenToImage)
	{
		saveScreenToImage = false;
		GLenum format = GL_RGB;
		if (originalImage->GetNrChannels() == 4)
		{
			format = GL_RGBA;
		}
		glReadPixels(0, 0, originalImage->GetWidth(), originalImage->GetHeight(), format, GL_UNSIGNED_BYTE, processedImage->GetImageData());
		processedImage->UploadNewData(processedImage->GetImageData());
		SaveImage("shader_processing_" + std::to_string(outputMode));

		float aspectRatio = static_cast<float>(originalImage->GetWidth()) / originalImage->GetHeight();
		window->SetSize(static_cast<int>(600 * aspectRatio), 600);
	}
}

void Laborator7::FrameEnd()
{
	DrawCoordinatSystem();
}

void Laborator7::OnFileSelected(std::string fileName)
{
	if (fileName.size())
	{
		std::cout << fileName << endl;
		originalImage = TextureManager::LoadTexture(fileName.c_str(), nullptr, "image", true, true);
		processedImage = TextureManager::LoadTexture(fileName.c_str(), nullptr, "newImage", true, true);

		float aspectRatio = static_cast<float>(originalImage->GetWidth()) / originalImage->GetHeight();
		window->SetSize(static_cast<int>(600 * aspectRatio), 600);
	}
}

void Laborator7::Medie() {
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	for (int i = 1; i < imageSize.y - 1; i++)
	{
		for (int j = 1; j < imageSize.x - 1; j++)
		{
			int offset = channels * (i * imageSize.x + j);

			// Reset save image data
			char value = static_cast<char>((data[offset + 3] + data[offset - 3] + data[offset + 3*imageSize.x] + data[offset + 3*imageSize.x + 3] + data[offset + 3*imageSize.x - 3] + data[offset - 3*imageSize.x + 3] + data[offset - 3*imageSize.x - 3] + data[offset - 3*imageSize.x]) / 8);
			memset(&newData[offset], value, 1);
			offset++;
			value = static_cast<char>((data[offset + 3] + data[offset - 3] + data[offset + 3 * imageSize.x] + data[offset + 3 * imageSize.x + 3] + data[offset + 3 * imageSize.x - 3] + data[offset - 3 * imageSize.x + 3] + data[offset - 3 * imageSize.x - 3] + data[offset - 3 * imageSize.x]) / 8);
			memset(&newData[offset], value, 1);
			offset++;
			value = static_cast<char>((data[offset + 3] + data[offset - 3] + data[offset + 3 * imageSize.x] + data[offset + 3 * imageSize.x + 3] + data[offset + 3 * imageSize.x - 3] + data[offset - 3 * imageSize.x + 3] + data[offset - 3 * imageSize.x - 3] + data[offset - 3 * imageSize.x]) / 8);
			memset(&newData[offset], value, 1);
		}
	}
	processedImage->UploadNewData(newData);
	
}


void Laborator7::Gaussian() {
	
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	for (int i = 1; i < imageSize.y - 1; i++)
	{
		for (int j = 1; j < imageSize.x - 1; j++)
		{
			int offset = channels * (i * imageSize.x + j);

			// Reset save image data
			char value = static_cast<char>((2 * data[offset + 3] + 2 * data[offset - 3] + 2 * data[offset + 3 * imageSize.x] + data[offset + 3 * imageSize.x + 3] + data[offset + 3 * imageSize.x - 3] + data[offset - 3 * imageSize.x + 3] + data[offset - 3 * imageSize.x - 3] + 2 * data[offset - 3 * imageSize.x] + 4 * data[offset]) / 16);
			memset(&newData[offset], value, 1);
			offset++;
			value = static_cast<char>((2 * data[offset + 3] + 2 * data[offset - 3] + 2 * data[offset + 3 * imageSize.x] + data[offset + 3 * imageSize.x + 3] + data[offset + 3 * imageSize.x - 3] + data[offset - 3 * imageSize.x + 3] + data[offset - 3 * imageSize.x - 3] + 2 * data[offset - 3 * imageSize.x] + 4 * data[offset]) / 16);
			memset(&newData[offset], value, 1);
			offset++;
			value = static_cast<char>((2 * data[offset + 3] + 2 * data[offset - 3] + 2 * data[offset + 3 * imageSize.x] + data[offset + 3 * imageSize.x + 3] + data[offset + 3 * imageSize.x - 3] + data[offset - 3 * imageSize.x + 3] + data[offset - 3 * imageSize.x - 3] + 2 * data[offset - 3 * imageSize.x] + 4 * data[offset]) / 16);
			memset(&newData[offset], value, 1);
		}
	}
	processedImage->UploadNewData(newData);
	
}

void Laborator7::InitialImage() {
	unsigned char* data = originalImage->GetImageData();
	processedImage->UploadNewData(data);
}

void Laborator7::Median() {

	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	vector<char> v;

	for (int i = 1; i < imageSize.y - 1; i++)
	{
		for (int j = 1; j < imageSize.x - 1; j++)
		{
			int offset = channels * (i * imageSize.x + j);
			char result;
			// Reset save image data

			v.clear();
			v.push_back(data[offset]);
			v.push_back(data[offset + 3]);
			v.push_back(data[offset - 3]);
			v.push_back(data[offset + 3 * imageSize.x]);
			v.push_back(data[offset + 3 * imageSize.x + 3]);
			v.push_back(data[offset + 3 * imageSize.x - 3]);
			v.push_back(data[offset - 3 * imageSize.x]);
			v.push_back(data[offset - 3 * imageSize.x + 3]);
			v.push_back(data[offset - 3 * imageSize.x - 3]);
			sort(v.begin(), v.end());
			result = v[4];
			memset(&newData[offset], result, 1);


			v.clear();
			offset++;
			v.push_back(data[offset]);
			v.push_back(data[offset + 3]);
			v.push_back(data[offset - 3]);
			v.push_back(data[offset + 3 * imageSize.x]);
			v.push_back(data[offset + 3 * imageSize.x + 3]);
			v.push_back(data[offset + 3 * imageSize.x - 3]);
			v.push_back(data[offset - 3 * imageSize.x]);
			v.push_back(data[offset - 3 * imageSize.x + 3]);
			v.push_back(data[offset - 3 * imageSize.x - 3]);
			sort(v.begin(), v.end());
			result = v[4];
			memset(&newData[offset], result, 1);


			v.clear();
			offset++;
			v.push_back(data[offset]);
			v.push_back(data[offset + 3]);
			v.push_back(data[offset - 3]);
			v.push_back(data[offset + 3 * imageSize.x]);
			v.push_back(data[offset + 3 * imageSize.x + 3]);
			v.push_back(data[offset + 3 * imageSize.x - 3]);
			v.push_back(data[offset - 3 * imageSize.x]);
			v.push_back(data[offset - 3 * imageSize.x + 3]);
			v.push_back(data[offset - 3 * imageSize.x - 3]);
			sort(v.begin(), v.end());
			result = v[4];
			memset(&newData[offset], result, 1);

		}
	}
	processedImage->UploadNewData(newData);

}

void Laborator7::GrayScale()
{
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (i * imageSize.x + j);

			// Reset save image data
			char value = static_cast<char>(data[offset + 0] * 0.2f + data[offset + 1] * 0.71f + data[offset + 2] * 0.07);
			memset(&newData[offset], value, 3);
		}
	}
	processedImage->UploadNewData(newData);
}


void Laborator7::Laplacian()
{
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());
	for (int i = 1; i < imageSize.y-1; i++)
	{
		for (int j = 1; j < imageSize.x-1; j++)
		{
			int offset = channels * (i * imageSize.x + j);
			// Reset save image data
			int value = static_cast<int>(data[offset + 3] + data[offset - 3] + data[offset + 3*imageSize.x] + data[offset - 3*imageSize.x] - 4 * data[offset]);
			char aux;
			if (value > 255) {
				aux = 255;
			}
			else if (value < 0) {
				aux = 0;
			}
			else {
				aux = value;
			}
			memset(&newData[offset], aux, 3);
		}
	}
	processedImage->UploadNewData(newData);
}

void Laborator7::Roberts()
{
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());
	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (i * imageSize.x + j);
			char d1 = static_cast<char>(data[offset + 3] - data[offset + 3 * imageSize.x]);
			char d2 = static_cast<char>(data[offset] - data[offset + 3 * imageSize.x + 3]);
			char value = abs(d1) + abs(d2);
			memset(&newData[offset], value, 3);
		}
	}
	processedImage->UploadNewData(newData);
}

void Laborator7::Sobel()
{
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());
	for (int i = 1; i < imageSize.y-1; i++)
	{
		for (int j = 1; j < imageSize.x-1; j++)
		{
			int offset = channels * (i * imageSize.x + j);
			char d1 = static_cast<char>(
				-2 * data[offset - 3] + 2 * data[offset + 3]
				- data[offset - 3 * imageSize.x - 3] + data[offset + 3 * imageSize.x + 3]
				- data[offset + 3 * imageSize.x - 3] + data[offset - 3 * imageSize.x + 3]
				);
			char d2 = static_cast<char>(
				2 * data[offset - 3 * imageSize.x] + data[offset - 3 * imageSize.x - 3]
				+ data[offset - 3 * imageSize.x + 3] - 2 * data[offset + 3 * imageSize.x]
				- data[offset + 3 * imageSize.x + 3] - data[offset + 3 * imageSize.x - 3]
				);
			char value = abs(d1) + abs(d2);
			memset(&newData[offset], value, 3);
		}
	}
	processedImage->UploadNewData(newData);
}


void Laborator7::Prewitt()
{
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());
	for (int i = 1; i < imageSize.y - 1; i++)
	{
		for (int j = 1; j < imageSize.x - 1; j++)
		{
			int offset = channels * (i * imageSize.x + j);
			char d1 = static_cast<char>(
				- data[offset - 3] + data[offset + 3]
				- data[offset - 3 * imageSize.x - 3] + data[offset + 3 * imageSize.x + 3]
				- data[offset + 3 * imageSize.x - 3] + data[offset - 3 * imageSize.x + 3]
				);
			char d2 = static_cast<char>(
				data[offset - 3 * imageSize.x] + data[offset - 3 * imageSize.x - 3]
				+ data[offset - 3 * imageSize.x + 3] - data[offset + 3 * imageSize.x]
				- data[offset + 3 * imageSize.x + 3] - data[offset + 3 * imageSize.x - 3]
				);
			char value = abs(d1) + abs(d2);
			memset(&newData[offset], value, 3);
		}
	}
	processedImage->UploadNewData(newData);
}

void Laborator7::SaveImage(std::string fileName)
{
	cout << "Saving image! ";
	processedImage->SaveToFile((fileName + ".png").c_str());
	cout << "[Done]" << endl;
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void Laborator7::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
};


// 1->imaginea initiala 2->grayscale 3->medie 4->gaussian 5->median
void Laborator7::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_F || key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE)
	{
		OpenDialogue();
	}

	
	if (key == GLFW_KEY_E)
	{
		gpuProcessing = !gpuProcessing;
		if (gpuProcessing == false)
		{
			outputMode = 0;
		}
		cout << "Processing on GPU: " << (gpuProcessing ? "true" : "false") << endl;
	}
	if (key - GLFW_KEY_2 == 0)
	{
		outputMode = 2;

		if (gpuProcessing == false)
		{
			cout << "Laplacian" << endl;
			outputMode = 0;
			Laplacian();
		}
	}
	if (key - GLFW_KEY_3 == 0)
	{
		outputMode = 3;

		if (gpuProcessing == false)
		{
			cout << "Roberts" << endl;
			outputMode = 0;
			Roberts();
		}
	}

	if (key - GLFW_KEY_1 == 0)
	{
		outputMode = 1;

		if (gpuProcessing == false)
		{
			cout << "Initial image" << endl;
			outputMode = 0;
			InitialImage();
		}
	}

	if (key - GLFW_KEY_4 == 0)
	{
		outputMode = 4;

		if (gpuProcessing == false)
		{
			cout << "Sobel" << endl;
			outputMode = 0;
			Sobel();
		}
	}
	if (key - GLFW_KEY_5 == 0)
	{
		outputMode = 5;

		if (gpuProcessing == false)
		{
			cout << "Prewitt" << endl;
			outputMode = 0;
			Prewitt();
		}
	}
	/*
	if (key - GLFW_KEY_2 == 0)
	{
		outputMode = 2;

		if (gpuProcessing == false)
		{
			outputMode = 0;
			GrayScale();
		}
	}
	if (key - GLFW_KEY_3 == 0)
	{
		outputMode =  3;

		if (gpuProcessing == false)
		{
			outputMode = 0;
			Medie();
		}
	}

	if (key - GLFW_KEY_1 == 0)
	{
		outputMode = 1;

		if (gpuProcessing == false)
		{
			outputMode = 0;
			InitialImage();
		}
	}

	if (key - GLFW_KEY_4 == 0)
	{
		outputMode = 4;

		if (gpuProcessing == false)
		{
			outputMode = 0;
			Gaussian();
		}
	}

	if (key - GLFW_KEY_5 == 0)
	{
		outputMode = 5;

		if (gpuProcessing == false)
		{
			outputMode = 0;
			Median();
		}
	}

	if (key == GLFW_KEY_S && mods & GLFW_MOD_CONTROL)
	{
		if (!gpuProcessing)
		{
			SaveImage("processCPU_" + std::to_string(outputMode));
		}
		else
		{
			saveScreenToImage = true;
		}
	}
	*/
};

void Laborator7::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Laborator7::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Laborator7::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Laborator7::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator7::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Laborator7::OnWindowResize(int width, int height)
{
	// treat window resize event
}
