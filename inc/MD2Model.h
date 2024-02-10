#pragma once

class CMD2Model
{
public:
	CMD2Model(void);
	~CMD2Model(void);

public:
	// md2 header
	struct Header
	{
		Header()
		{
			ZeroMemory(this, sizeof(Header));
		}

		UINT32     ident;              // magic number. must be equal to "IDP2"
		UINT32     version;            // md2 version. must be equal to 8

		UINT32     skinwidth;          // width of the texture
		UINT32     skinheight;         // height of the texture
		UINT32     framesize;          // size of one frame in bytes

		UINT32     num_skins;          // number of textures
		UINT32     num_xyz;            // number of vertices
		UINT32     num_st;             // number of texture coordinates
		UINT32     num_tris;           // number of triangles
		UINT32     num_glcmds;         // number of opengl commands
		UINT32     num_frames;         // total number of frames

		UINT32     ofs_skins;          // offset to skin names (64 bytes each)
		UINT32     ofs_st;             // offset to s-t texture coordinates
		UINT32     ofs_tris;           // offset to triangles
		UINT32     ofs_frames;         // offset to frame data
		UINT32     ofs_glcmds;         // offset to opengl commands
		UINT32     ofs_end;            // offset to end of file

	};
};
