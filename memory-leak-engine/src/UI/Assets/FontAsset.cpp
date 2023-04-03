#include "UI/Assets/FontAsset.h"
#include "freetype/freetype.h"
#include "Macros.h"

//TODO: Load font size
int fontSize = 16;

mlg::FontAsset::FontAsset(const std::string& path) : Asset(path) { }

//TODO: Implement me
mlg::FontAsset::~FontAsset() {
}

void mlg::FontAsset::Load() {
    FT_Library lib;
    FT_Error error;
    FT_Face face;
    FT_UInt glyphIndex;

    // init freetype
    error = FT_Init_FreeType( &lib );
    if ( error != FT_Err_Ok )
    {
        SPDLOG_ERROR("FT_Init_FreeType failed, error code: " + error);
        return;
    }

    // load font
    error = FT_New_Face( lib , path.c_str() , 0 , &face );
    if ( error == FT_Err_Unknown_File_Format )
    {
        SPDLOG_ERROR("failed to open file \"" + path + "\", unknown file format");
        return;
    }
    else if ( error )
    {
        SPDLOG_ERROR("failed to open file \"" + path + "\", error code: " /*+ error*/);
        return;
    }

    // set font size
    error = FT_Set_Pixel_Sizes( face , 0 , fontSize );
    if ( error )
    {
        SPDLOG_ERROR("failed to set font size, error code: " + error);
    }


}
