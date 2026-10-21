#pragma once

#include "glad/glad.h"
#include "stb/stb_image.h"

#include "Rendering/shaderClass.h"

namespace TWE
{

	class Texture
	{
		public:
			unsigned int ID;
			const char* type;
			unsigned int unit;
			
			Texture(const char* image, const char* texType, GLenum slot);

			void texUnit(TE::Shader &shader, const char* uniform, unsigned int unit);
			void Bind();
			void UnBind();
			void Delete();
	};

};