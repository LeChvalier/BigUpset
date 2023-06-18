#pragma once

#include <string>
#include <memory>

#include "Renderer.h"

struct float4;

enum class GroundType;

typedef unsigned int GLenum;
typedef unsigned int GLuint;


enum imageFlags
{
	IMG_FLIP = 1 << 0,
	IMG_FORCE_GREY = 1 << 1,
	IMG_FORCE_GREY_ALPHA = 1 << 2,
	IMG_FORCE_RGB = 1 << 3,
	IMG_FORCE_RGBA = 1 << 4,
	IMG_GEN_MIPMAPS = 1 << 5,
	IMG_LINEAR = 1 << 6,
};


// Load textures
class TextureLoader
{
public:
	TextureLoader(Renderer* renderer) { p_renderer = renderer; };
	~TextureLoader() {};

	// Load texture with stb_image
	Texture LoadTexture(const std::string& texture);

	std::unique_ptr<float4[]> ParseMap(const std::string& texture, unsigned int& w, unsigned int& h);

private:
	// Need to add texture
	Renderer* p_renderer;
	
};

// Load texeture with stb_image
// Fill up tex with loaded texture
void UploadTexture(const char* Filename, int ImageFlags);