
#include <GLES/gl.h>
#include <GLES/glext.h>

#include "GraphicsDriver.h"
#include "common.h"

static const char gVertexShader[] = "attribute vec4 a_position;   \n"
        "attribute vec2 a_texCoord;   \n"
        "varying vec2 v_texCoord;     \n"
        "uniform mat4 u_mvp;            \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = u_mvp * vec4(a_position.xyz, 1); \n"
        "   v_texCoord = a_texCoord;  \n"
        "}                            \n";

static const char gFragmentShader[] =
         "precision mediump float;                            \n"
         "varying vec2 v_texCoord;                            \n"
         "uniform sampler2D s_texture;                        \n"
         "uniform float alpha;                                \n"
         "void main()                                         \n"
         "{                                                   \n"
         "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
         "  gl_FragColor.a = gl_FragColor.a * alpha;            \n"
         "}                                                   \n";

static const char gEmuVertexShader[] = "attribute vec4 a_position;   \n"
        "attribute vec2 a_texCoord;   \n"
        "varying vec2 v_texCoord;     \n"
        "uniform mat4 u_mvp;            \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = u_mvp * vec4(a_position.xyz, 1); \n"
        "   v_texCoord = a_texCoord;  \n"
        "}                            \n";

static const char gEmuFragmentShader[] =
         "precision mediump float;                            \n"
         "varying vec2 v_texCoord;                            \n"
         "uniform sampler2D s_texture;                        \n"
         "void main()                                         \n"
         "{                                                   \n"
         "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
         "}                                                   \n";


static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}


GraphicsDriver::GraphicsDriver()
{
     _matrix = new float[16];
     _vertices = new GLfloat[4*3];
     _texCoords = new GLfloat[4*2];
     _guiAlpha = 0.65;

     _realHeight = 0;
     _realWidth = 0;
     _screenWidth = 0;
     _screenHeight = 0;
     _xOffset = 0;
     _yOffset = 0;

     _ratio = 0;
     _smooth = true;

     _shaderProgram = 0;
     _vMvpMatrix = -1;
     _vPositionHandle = -1;
     _vTexCoordHandle = -1;
     _vSamplerHandle = -1;
     _vAlphaHandle = -1;

     _EmuShaderProgram = 0;
     _vEmuMvpMatrix = -1;
     _vEmuPositionHandle = -1;
     _vEmuTexCoordHandle = -1;
     _vEmuSamplerHandle = -1;
     _vEmuTextureSizeXHandle = -1;
     _vEmuTextureSizeYHandle = -1;
     _vEmuPaletteSamplerHandle = -1;
}


GraphicsDriver::~GraphicsDriver()
{
     if (_matrix != NULL)
     {
          delete _matrix;
          _matrix = NULL;
     }

     if (_vertices != NULL)
     {
          delete _vertices;
          _vertices = NULL;
     }

     if (_texCoords != NULL)
     {
          delete _texCoords;
          _texCoords = NULL;
     }
}


int GraphicsDriver::InitEmuShader(const char* vertex, const char* frag)
{
     if (_EmuShaderProgram)
     {
          glDeleteProgram(_EmuShaderProgram);
     }
     _EmuShaderProgram = this->CreateProgram(vertex == NULL ? gEmuVertexShader : vertex,
                                             frag == NULL ? gEmuFragmentShader : frag);
     if (!_EmuShaderProgram) {
        LOGE("Could not create program.");
        return GRAPHICS_ERROR;
     }

     _vEmuPositionHandle = glGetAttribLocation(_EmuShaderProgram, "a_position");
     checkGlError("glGetAttribLocation(\"a_position\"");
     LOGI("glGetAttribLocation(\"a_position\") = %d\n", _vEmuPositionHandle);

     _vEmuTexCoordHandle = glGetAttribLocation(_EmuShaderProgram, "a_texCoord");
     checkGlError("glGetAttribLocation(\"a_texCoord\"");
     LOGI("glGetAttribLocation(\"a_texCoord\") = %d\n", _vEmuTexCoordHandle);

     _vEmuSamplerHandle = glGetUniformLocation(_EmuShaderProgram, "s_texture");
     checkGlError("glGetUniformLocation(\"s_texture\"");
     LOGI("glGetUniformLocation(\"s_texture\") = %d\n", _vEmuSamplerHandle);

     _vEmuTextureSizeXHandle = glGetUniformLocation(_EmuShaderProgram, "u_width");
     checkGlError("glGetUniformLocation(\"u_width\"");
     LOGI("glGetUniformLocation(\"u_width\") = %d\n", _vEmuTextureSizeXHandle);

     _vEmuTextureSizeYHandle = glGetUniformLocation(_EmuShaderProgram, "u_height");
     checkGlError("glGetUniformLocation(\"u_height\"");
     LOGI("glGetUniformLocation(\"u_height\") = %d\n", _vEmuTextureSizeYHandle);

     _vEmuMvpMatrix = glGetUniformLocation(_EmuShaderProgram, "u_mvp");
     checkGlError("glGetUniformLocation(\"u_mvp\"");
     LOGI("glGetUniformLocation(\"u_mvp\") = %d\n",
              _vEmuMvpMatrix);

     /*_vEmuPaletteSamplerHandle = glGetUniformLocation(_EmuShaderProgram, "s_palette");
     checkGlError("glGetUniformLocation");
     LOGI("glGetAttribLocation(\"s_palette\") = %d\n",
              _vEmuPaletteSamplerHandle);*/

     return GRAPHICS_OK;
}


int GraphicsDriver::Init(int maxEmuWidth, GLuint emuPixelFormat, GLuint emuPixelType)
{
     LOGD("GraphicsDriver.Init(%d, %d, %d)", maxEmuWidth, emuPixelFormat, emuPixelType);

     _emuMaxWidth = maxEmuWidth;
     _emuPixelFormat = emuPixelFormat;
     _emuPixelType = emuPixelType;

    /*LOGI("Version", GL_VERSION);
    LOGI("Vendor", GL_VENDOR);
    LOGI("Renderer", GL_RENDERER);
    LOGI("Extensions", GL_EXTENSIONS);*/

    // init the basic shaders, for gui stuff
    _shaderProgram = this->CreateProgram(gVertexShader, gFragmentShader);
    if (!_shaderProgram) {
        LOGE("Could not create program.");
        return GRAPHICS_ERROR;
    }

    _vPositionHandle = glGetAttribLocation(_shaderProgram, "a_position");
    checkGlError("glGetAttribLocation(\"a_position\")");
    LOGI("glGetAttribLocation(\"a_position\") = %d\n",
            _vPositionHandle);
    _vTexCoordHandle = glGetAttribLocation(_shaderProgram, "a_texCoord");
    checkGlError("glGetAttribLocation(\"a_texCoord\")");
    LOGI("glGetAttribLocation(\"a_texCoord\") = %d\n",
              _vTexCoordHandle);
    _vSamplerHandle = glGetUniformLocation(_shaderProgram, "s_texture");
    checkGlError("glGetUniformLocation(\"s_texture\")");
    LOGI("glGetUniformLocation(\"s_texture\") = %d\n",
              _vSamplerHandle);
    _vAlphaHandle = glGetUniformLocation(_shaderProgram, "alpha");
    checkGlError("glGetUniformLocation(\"alpha\")");
    LOGI("glGetUniformLocation(\"alpha\") = %d\n",
              _vAlphaHandle);
    _vMvpMatrix = glGetUniformLocation(_shaderProgram, "u_mvp");
    checkGlError("glGetUniformLocation(\"u_mvp\")");
    LOGI("glGetUniformLocation(\"u_mvp\") = %d\n",
              _vMvpMatrix);


    // get a texture id for this, create the texture internally so we can sub image to it later
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    // Use loosely packed data
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, _emuPixelFormat, maxEmuWidth, maxEmuWidth, 0, _emuPixelFormat, _emuPixelType, NULL);

    // disable some gl stuff we wont ever need
    glDisable(GL_DEPTH_TEST);

    // fbo
    /*glGenFramebuffers(1, &_fboId);
    checkGlError("glGenFramebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER_OES, _fboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, _textureId, 0);

    glBindFramebuffer(GL_FRAMEBUFFER_OES, 0);*/

    return GRAPHICS_OK;
}


void GraphicsDriver::SetSmooth(bool b)
{
     glBindTexture(GL_TEXTURE_2D, _textureId);

     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, b ? GL_LINEAR : GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, b ? GL_LINEAR : GL_NEAREST);
}


GLuint GraphicsDriver::LoadShader(GLenum shaderType, const char* pSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader)
    {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char* buf = (char*) malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}


GLuint GraphicsDriver::CreateProgram(const char* pVertexSource, const char* pFragmentSource)
{
    GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader)
    {
        return 0;
    }

    GLuint pixelShader = LoadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader)
    {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program)
    {
        glAttachShader(program, vertexShader);
        //checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        //checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*) malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}


GLint GraphicsDriver::GenTexture()
{
     GLuint texId = -1;
     glGenTextures(1, &texId);

     LOGD("GENERATED GL TEXTURE: %d", texId);

     return texId;
}


int GraphicsDriver::BlitToTexture(int texId, int width, int height, int bitDepth, void* imageData)
{
     // get a texture id for this, create the texture internally so we can sub image to it later
     LOGD("BLIT, w: %d, h: %d, depth: %d", width, height, bitDepth);

    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)imageData);

    return GRAPHICS_OK;
}


void GraphicsDriver::SetDimensions(int width, int height)
{
	LOGD("SetDimensions(%d, %d)", width, height);

	_screenWidth = width;
	_screenHeight = height;

	glViewport(0, 0, _screenWidth, _screenHeight);
	SetAspectRatio(_ratio);

    SetAspect();

	LOGD("RealWidth: %f", _realWidth);
	LOGD("RealHeight: %f", _realHeight);

	_setupScene();
}


void GraphicsDriver::ReshapeEmuTexture(int width, int height, float max)
{
	LOGD("ReshapeEmuTexture(%d, %d)", width, height);

     _emuWidth = width;
     _emuHeight = height;

	// texture coordinates
	float xMax = width / max;
	float yMax = height / max;

    _texCoords[0] = 0.0; _texCoords[1] =  yMax;
    _texCoords[2] = xMax; _texCoords[3] =  yMax;
    _texCoords[4] = 0.0; _texCoords[5] =  0.0;
    _texCoords[6] = xMax; _texCoords[7] =  0.0;
}


void GraphicsDriver::_setupScene()
{
     float xCenter = (_screenWidth / 2.0f) - (_realWidth / 2.0f);
     float yCenter = (_screenHeight / 2.0f) - (_realHeight / 2.0f);

    // generate the screen quad vertices
     _vertices[0] = xCenter;       _vertices[1] = yCenter + _realHeight;   _vertices[2] = 0.0;
     _vertices[3] = xCenter + _realWidth;    _vertices[4] = yCenter + _realHeight;   _vertices[5] = 0.0;
     _vertices[6] = xCenter;       _vertices[7] = yCenter;       _vertices[8] = 0.0;
     _vertices[9] = xCenter + _realWidth;    _vertices[10] = yCenter;      _vertices[11] = 0.0;

    // generate our matrix
    _matrix[0] = 2.0f / _screenWidth;  _matrix[4] = 0;  _matrix[8] = 0;  _matrix[12] = -1;
    _matrix[1] = 0;  _matrix[5] = 2.0f / -_screenHeight;  _matrix[9] = 0;  _matrix[13] = 1;
    _matrix[2] = 0;  _matrix[6] = 0;  _matrix[10] = -2; _matrix[14] = -1;
    _matrix[3] = 0; _matrix[7] = 0; _matrix[11] = 0; _matrix[15] = 1;
}


void GraphicsDriver::SetAspectRatio(float ratio)
{
     LOGD("Graphics::SetAspectRatio(%f)", ratio);

     _ratio = ratio;
}


void GraphicsDriver::SetOverscan(bool bOverscan, float amount)
{

}

//FIXME: Change name to SetViewports?
void GraphicsDriver::SetAspect()
{
     GLint width = _screenWidth;
     GLint height = _screenHeight;

     if (height == 0)
     {
          height = 1;
     }

     float device_aspect = ((float)width) / ((float)height);

     LOGD("SetAspect(%d, %d, %f)", width, height, device_aspect);

     // calculate the glOrtho matrix needed to transform the texture to the desired aspect ratio
     float desired_aspect = _ratio;

     _realWidth = width;
     _realHeight = height;

     // If the aspect ratios of screen and desired aspect ratio are sufficiently equal (floating point stuff),
     // assume they are actually equal.
     if ( (int)(device_aspect*1000) > (int)(desired_aspect*1000) )
     {
          float delta = (desired_aspect / device_aspect - 1.0) / 2.0 + 0.5;
          //glViewport(width * (0.5 - delta), 0, 2.0 * width * delta, height);
          _realWidth = (int)(2.0 * width * delta);
     }

     else if ( (int)(device_aspect*1000) < (int)(desired_aspect*1000) )
     {
          float delta = (device_aspect / desired_aspect - 1.0) / 2.0 + 0.5;
          //glViewport(0, height * (0.5 - delta), width, 2.0 * height * delta);
          _realHeight = (int)(2.0 * height * delta);
     }
     else
     {
          //glViewport(0, 0, width, height);
     }

     /* (m_overscan)
     {
          glOrthof(-m_overscan_amount/2, 1 + m_overscan_amount/2, -m_overscan_amount/2, 1 + m_overscan_amount/2, -1, 1);
     }
     else
     {
          glOrthof(0, 1, 0, 1, -1, 1);
     }*/
}


void GraphicsDriver::Clear() const
{
     //glClearColor(0, 0, 0.782f, 1);
     glClearColor(0, 0, 0, 1);
     glClear(GL_COLOR_BUFFER_BIT);
}


void GraphicsDriver::DrawQuad2D(Quad2D* quad) const
{
     glEnable( GL_BLEND );
     glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

     glUseProgram(_shaderProgram);

     glVertexAttribPointer(_vPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, quad->Vertices);
     //checkGlError("glVertexAttribPointer(_vPositionHandle)");
     glVertexAttribPointer(_vTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, quad->TexCoords);
     //checkGlError("glVertexAttribPointer(_vTexCoordHandle)");
     glEnableVertexAttribArray(_vPositionHandle);
     glEnableVertexAttribArray(_vTexCoordHandle);
     //checkGlError("glEnableVertexAttribArray");

     // Bind the texture
     glActiveTexture(GL_TEXTURE0);
     //checkGlError("glActiveTexture");

     glBindTexture(GL_TEXTURE_2D, quad->TextureId);
     //checkGlError("glBindTexture");

     // Set the sampler texture unit to 0
     glUniform1i(_vSamplerHandle, 0);
     //checkGlError("glUniform1i");

     // set the alpha value for the quads
     glUniform1f(_vAlphaHandle, _guiAlpha);

     glUniformMatrix4fv(_vMvpMatrix, 1, false, _matrix);
     //checkGlError("glUniformMatrix4fv");

     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
     //checkGlError("glDrawArrays");

     glDisable( GL_BLEND );
}


void GraphicsDriver::DrawEMU(const void* texture)
{
	DrawEMU(texture, _emuWidth, _emuHeight);
}


void GraphicsDriver::DrawEMU(const void *texture, int emuw, int emuh)
{
     //LOGD("DrawEMU(%p, %d, %d)", texture, emuw, emuh);

     //LOGD("BIND TEXTURE");
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, _textureId);

     glPixelStorei(GL_PACK_ALIGNMENT, 1);
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

     glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, emuw, emuh, _emuPixelFormat, _emuPixelType, texture);

     //LOGD("DONE DRAW NES");
}


void GraphicsDriver::Draw()
{
     // DRAW EMU TO FBO
    glUseProgram(_EmuShaderProgram);
    //glBindFramebuffer(GL_FRAMEBUFFER_OES, _fboId);
    //checkGlError("glUseProgram");

    glVertexAttribPointer(_vEmuPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, _vertices);
    //checkGlError("glVertexAttribPointer(_vPositionHandle)");
    glVertexAttribPointer(_vEmuTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, _texCoords);
    //checkGlError("glVertexAttribPointer(_vTexCoordHandle)");
    glEnableVertexAttribArray(_vEmuPositionHandle);
    //checkGlError("glEnableVertexAttribArray Position");
    glEnableVertexAttribArray(_vEmuTexCoordHandle);
    //checkGlError("glEnableVertexAttribArray Texture");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, _paletteTextureId);

    // Set the sampler texture unit to 0
    glUniform1i(_vEmuSamplerHandle, 0);
    //checkGlError("glUniform1i Emu");

    //glUniform1i(_vEmuPaletteSamplerHandle, 1);
    //checkGlError("glUniform1i Palette");

    glUniformMatrix4fv(_vEmuMvpMatrix, 1, false, _matrix);
    //checkGlError("glUniformMatrix4fv");

    if (_vEmuTextureSizeXHandle >= 0 && _vEmuTextureSizeYHandle >= 0)
    {
         glUniform1f(_vEmuTextureSizeXHandle, _emuWidth);
         glUniform1f(_vEmuTextureSizeYHandle, _emuHeight);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    /*glBindFramebuffer(GL_FRAMEBUFFER_OES, 0);

    // DRAW FBO TO SCREEN
    glUseProgram(_shaderProgram);

    glVertexAttribPointer(_vPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, _vertices);
    glVertexAttribPointer(_vTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, _texCoords);

    glEnableVertexAttribArray(_vPositionHandle);
    glEnableVertexAttribArray(_vTexCoordHandle);

    // Set the sampler texture unit to 0
    glUniform1i(_vSamplerHandle, 0);


    // set the alpha value for the quads
    glUniform1f(_vAlphaHandle, 1.0f);

    glUniformMatrix4fv(_vMvpMatrix, 1, false, _matrix);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //checkGlError("glDrawArrays");*/
}


static void png_zip_read(png_structp png, png_bytep data, png_size_t size)
{
     struct zip_file *zfp = (struct zip_file *) png->io_ptr;
     zip_fread(zfp, data, size);
}



int GraphicsDriver::loadPNGTexture(zip* apkArchive, const char* szFilename, int* outGLId)
{
     //header for testing if it is a png
     png_byte header[8];

     zip_file* file = zip_fopen(apkArchive, szFilename, 0);
     if (!file) {
         LOGE("Error opening %s from APK", szFilename);
         return GRAPHICS_ERROR;
     }

     //read the header
     zip_fread(file, header, 8);

     //test if png
     int is_png = !png_sig_cmp(header, 0, 8);
     if (!is_png)
     {
          zip_fclose(file);
          LOGE("Not a png file : %s", szFilename);
          return GRAPHICS_ERROR;
     }

     //create png struct
     png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
               NULL, NULL);
     if (!png_ptr)
     {
          zip_fclose(file);
          LOGE("Unable to create png struct : %s", szFilename);
          return GRAPHICS_ERROR;
     }

     //create png info struct
     png_infop info_ptr = png_create_info_struct(png_ptr);
     if (!info_ptr)
     {
          png_destroy_read_struct(&png_ptr, (png_infopp) NULL,
                    (png_infopp) NULL);
          LOGE("Unable to create png info : %s", szFilename);
          zip_fclose(file);
          return GRAPHICS_ERROR;
     }

     //create png info struct
     png_infop end_info = png_create_info_struct(png_ptr);
     if (!end_info)
     {
          png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
          LOGE("Unable to create png end info : %s", szFilename);
          zip_fclose(file);
          return GRAPHICS_ERROR;
     }

     //png error stuff, not sure libpng man suggests this.
     if (setjmp(png_jmpbuf(png_ptr)))
     {
          zip_fclose(file);
          LOGE("Error during setjmp : %s", szFilename);
          png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
          return GRAPHICS_ERROR;
     }

     //init png reading
     //png_init_io(png_ptr, );
     png_ptr->io_ptr = file;
     png_set_read_fn(png_ptr, file, png_zip_read);

     //let libpng know you already read the first 8 bytes
     png_set_sig_bytes(png_ptr, 8);

     // read all the info up to the image data
     png_read_info(png_ptr, info_ptr);

     //variables to pass to get info
     int bit_depth, color_type;
     png_uint_32 twidth, theight;

     // get info about png
     png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth,
               &color_type, NULL, NULL, NULL);

     //update width and height based on png info
     //[nTex] = twidth;
     //tex_h[nTex] = theight;

     // Update the png info struct.
     png_read_update_info(png_ptr, info_ptr);

     // Row size in bytes.
     int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

     // Allocate the image_data as a big block, to be given to opengl
     png_byte *image_data = new png_byte[rowbytes * theight];
     if (!image_data)
     {
          //clean up memory and close stuff
          png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
          LOGE("Unable to allocate image_data while loading %s ", szFilename);
          zip_fclose(file);
          return GRAPHICS_ERROR;
     }

     //row_pointers is for pointing to image_data for reading the png with libpng
     png_bytep *row_pointers = new png_bytep[theight];
     if (!row_pointers)
     {
          //clean up memory and close stuff
          png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
          delete[] image_data;
          LOGE("Unable to allocate row_pointer while loading %s ", szFilename);
          zip_fclose(file);
          return GRAPHICS_ERROR;
     }
     // set the individual row_pointers to point at the correct offsets of image_data
     for (int i = 0; i < theight; ++i)
     {
          row_pointers[theight - 1 - i] = image_data + i * rowbytes;
     }

     //read the png into image_data through row_pointers
     png_read_image(png_ptr, row_pointers);

     // dump to opengl
     GLint texId = this->GenTexture();
     if (texId < 0)
     {
          png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
          delete[] image_data;
          delete[] row_pointers;
          zip_fclose(file);

          return GRAPHICS_ERROR;
     }

     *outGLId = texId;

     this->BlitToTexture(texId, twidth, theight, bit_depth, (void*)image_data);

     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     delete[] image_data;
     delete[] row_pointers;
     zip_fclose(file);

     return GRAPHICS_OK;
}
