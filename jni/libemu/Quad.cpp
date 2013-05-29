

#include "Quad.h"

Quad2D::Quad2D()
{
     Visible = false;
     Vertices = new GLfloat[4*3];
     TexCoords = new GLfloat[4*2];

     TextureId = -1;

     SetDimensions(0, 0, 0, 0);

     TexCoords[0] = 0.0f;    TexCoords[1] = 0.0f;
     TexCoords[2] = 1.0f;    TexCoords[3] = 0.0f;
     TexCoords[4] = 0.0f;    TexCoords[5] = 1.0f;
     TexCoords[6] = 1.0f;    TexCoords[7] = 1.0f;
}

Quad2D::Quad2D(float x, float y, float width, float height)
{
     Visible = false;
     Vertices = new GLfloat[4*3];
     TexCoords = new GLfloat[4*2];

     TextureId = -1;

     SetDimensions(x, y, width, height);

     TexCoords[0] = 0.0f;    TexCoords[1] = 1.0f;
     TexCoords[2] = 1.0f;    TexCoords[3] = 1.0f;
     TexCoords[4] = 0.0f;    TexCoords[5] = 0.0f;
     TexCoords[6] = 1.0f;    TexCoords[7] = 0.0f;
}

Quad2D::~Quad2D()
{
     delete Vertices;
     Vertices = NULL;

     delete TexCoords;
     TexCoords = NULL;
}


void Quad2D::SetDimensions(float x, float y, float width, float height)
{
     X = x;
     Y = y;
     Width = width;
     Height = height;

     // generate the screen quad vertices
     Vertices[0] = x;        Vertices[1] = y+height;     Vertices[2] = 0.0;
     Vertices[3] = x+width;  Vertices[4] = y+height;     Vertices[5] = 0.0;
     Vertices[6] = x;        Vertices[7] = y;            Vertices[8] = 0.0;
     Vertices[9] = x+width;  Vertices[10] = y;           Vertices[11] = 0.0;
}

