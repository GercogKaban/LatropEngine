#pragma once

#include <unordered_map>

class TypographicFace
{
public:

	struct Args
	{
		std::string fontPath;
		signed long faceIndex = 0;
	};

	TypographicFace(class FT_LibraryRec_* ftLib, const TypographicFace::Args&& args);
	~TypographicFace();

private:

	//TypographicFace(const TypographicFace&) = delete;
	//TypographicFace& operator=(const TypographicFace&) = delete;

	class FT_FaceRec_* FT_Face{};
};

class TextRenderer
{
public:

	TextRenderer();
	~TextRenderer();

	// 1st call can be slow, but faces are cached 
	const TypographicFace* getFace(const TypographicFace::Args&& args);

	// TODO: To be implemented
	//const TypographicFace* getFaceAsync(const TypographicFace::Args&& args);

private:

	TextRenderer(const TextRenderer&) = delete;
	TextRenderer& operator=(const TextRenderer&) = delete;

	class FT_LibraryRec_* ftLib{};
	std::unordered_map<std::string, TypographicFace> faces;
};