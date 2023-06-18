#include <stb_image.h>
#include <glad/gl.h>

#include <iostream>

#include "TextureLoader.h"
#include "Objects.h"
#include "Logger.h"
#include "Maths.h"


Texture TextureLoader::LoadTexture(const std::string& texture)
{
	Texture toReturn;
	toReturn.path = texture;

	// Create texture on gpu
	toReturn.gpu = p_renderer->CreateTexture(toReturn);

	Logger::Info("Loaded Texture : " + texture);
	toReturn.path = texture;
	return toReturn;
}

std::unique_ptr<float4[]> TextureLoader::ParseMap(const std::string& texture, unsigned int& w, unsigned int& h)
{
	// Desired channels
	int DesiredChannels = 0;
	int Channels = 0;
	if (IMG_FORCE_GREY)
	{
		DesiredChannels = STBI_grey;
		Channels = 1;
	}
	if (IMG_FORCE_GREY_ALPHA)
	{
		DesiredChannels = STBI_grey_alpha;
		Channels = 2;
	}
	if (IMG_FORCE_RGB)
	{
		DesiredChannels = STBI_rgb;
		Channels = 3;
	}
	if (IMG_FORCE_RGBA)
	{
		DesiredChannels = STBI_rgb_alpha;
		Channels = 4;
	}

	// Loading
	int Width = 0, Height = 0;
	float* Image = nullptr;
	Image = stbi_loadf(texture.c_str(), &Width, &Height, (DesiredChannels == 0) ? &Channels : nullptr, DesiredChannels);

	// Check error
	if (Image == nullptr || Width == 0 || Height == 0)
	{
		std::string std = texture;
		std = "Image Map loading failed on " + std;
		Logger::Error(std);
		return nullptr;
	}

	// TODO make a custom class
	// TODO make some function to get x y ellement (reuseable many times)
	// magic guillaume
	std::unique_ptr<float4[]> constantContainer = std::make_unique<float4[]>(Width * Height);

	if (Channels != 4)
		std::cout << "THERE IS ONLY RGB FORMAT" << std::endl;

	// Fill up map
	float4 Pixel;

	for (int j = 0; j < Height; j++)
	{
		for (int i = 0; i < Width; i++)
		{
			for (int k = 0; k < Channels; k++)
				Pixel.e[k] = Image[k + (j * Width + i) * sizeof(float)] *255;
			constantContainer[j * Width + i] = Pixel;
		}
	}

	// Free stbi_image alocators
	stbi_image_free(Image);

	// Set size and return data
	w = Width;
	h = Height;
	return constantContainer;
}


void UploadTexture(const char* Filename, int ImageFlags)
{
	// Flip
	stbi_set_flip_vertically_on_load((ImageFlags & IMG_FLIP) ? 1 : 0);

	// Desired channels
	int DesiredChannels = 0;
	int Channels = 0;
	if (ImageFlags & IMG_FORCE_GREY)
	{
		DesiredChannels = STBI_grey;
		Channels = 1;
	}
	if (ImageFlags & IMG_FORCE_GREY_ALPHA)
	{
		DesiredChannels = STBI_grey_alpha;
		Channels = 2;
	}
	if (ImageFlags & IMG_FORCE_RGB)
	{
		DesiredChannels = STBI_rgb;
		Channels = 3;
	}
	if (ImageFlags & IMG_FORCE_RGBA)
	{
		DesiredChannels = STBI_rgb_alpha;
		Channels = 4;
	}

	// Loading
	int Width = 0, Height = 0;
	void* Image = nullptr;

	if (ImageFlags & IMG_LINEAR)
		Image = stbi_loadf(Filename, &Width, &Height, (DesiredChannels == 0) ? &Channels : nullptr, DesiredChannels);
	else
		Image = stbi_load(Filename, &Width, &Height, (DesiredChannels == 0) ? &Channels : nullptr, DesiredChannels);

	if (Image == nullptr || Width == 0 || Height == 0)
	{
		std::string std = Filename;
		std = "Image loading failed on " + std;
		Logger::Error(std);
		return;
	}
	GLint GLImageFormat[] =
	{
		-1, // 0 Channels, unused
		GL_RED,
		GL_RG,
		GL_RGB,
		GL_RGBA,

		GL_R32F,
		GL_RG32F,
		GL_RGB32F,
		GL_RGBA32F
	};

	GLint format = GLImageFormat[Channels];
	glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, Image);

	// Free stbi_image alocators
	stbi_image_free(Image);

	stbi_set_flip_vertically_on_load(0); // Always reset to default value
}