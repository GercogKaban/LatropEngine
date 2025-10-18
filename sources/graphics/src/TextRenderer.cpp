#include "pch.h"
#include <freetype/freetype.h>
#include "TextRenderer.h"

TextRenderer::TextRenderer()
{
	FT_Init_FreeType(&ftLib);
	assert(ftLib);
}

TextRenderer::~TextRenderer()
{
	FT_Done_FreeType(ftLib);
}

const TypographicFace* TextRenderer::getFace(const TypographicFace::Args&& args)
{
	if (auto face = faces.find(args.fontPath); face != faces.end())
	{
		return &face->second;
	}
	else
	{
		faces.emplace(args.fontPath, TypographicFace(ftLib, std::move(args)));
	}
}

TypographicFace::TypographicFace(class FT_LibraryRec_* ftLib, const TypographicFace::Args&& args)
{
	FT_New_Face(ftLib, args.fontPath.data(), args.faceIndex, &FT_Face);
	assert(FT_Face);
}

TypographicFace::~TypographicFace()
{
	FT_Done_Face(FT_Face);
}
