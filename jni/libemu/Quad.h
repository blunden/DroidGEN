
#ifndef QUAD_H_
#define QUAD_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

/*typedef struct _Quad2D
{
     float x;
     float y;
     float w;
     float h;

     GLfloat* vertices;
     GLfloat* texCoords;
} Quad2D;*/

class Quad2D
{
public:
     Quad2D();
     Quad2D(float x, float y, float width, float height);
     ~Quad2D();

     void SetDimensions(float x, float y, float width, float height);

     float X;
     float Y;
     float Width;
     float Height;

     GLfloat* Vertices;
     GLfloat* TexCoords;

     GLint TextureId;
     bool Visible;
};


#endif /* QUAD_H_ */
