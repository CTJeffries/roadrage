//tga textures - tga file reading and useage
#include <iostream>
#include "tgaClass.h"

using namespace std;

void display();
void init();
void reshape(int w, int h);		//not using yet
void keyboard(unsigned char key, int x, int y);
void SetTextureParameters(int id);

int textureID = 0;		//obvious guy

//Display callback
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Display a quad texture to the screen
	glBegin(GL_QUADS);

		// glTexCoord2f() takes the X and Y offset (or U and V) into the bitmap.
		// Then, the next point sent to be rendered attaches that part of the bitmap
		// to itself.  The (U, V) coordinates range from (0, 0) being the top left corner
		// of the bitmap, to (1, 1) being the bottom left corner of the bitmap.
		// You can go above 1 but it just is wrapped around back to zero and repeats the texture.
		// Try setting the 1's to 2's and see what it does, then try setting them to 0.5's.
		// The higher the number, the more instances of the texture will appear on the square,
		// Where the lower the number, it stretches the incomplete texture over the surface of the square.
		// For every vertice we need a U V coordinate, as shown below.  You might have to play
		// around with the values to make it texture correctly, otherwise it will be flipped, upside down,
		// or skewed.  It also depends on where you are looking at it.  We are looking down the -Z axis.

		// Display the top left vertice
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1, 1, 0);

		// Display the bottom left vertice
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1, -1, 0);

		// Display the bottom right vertice
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1, -1, 0);

	// Display the top right vertice
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1, 1, 0);

	glEnd();											// Stop drawing QUADS

	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
#pragma unused(x,y)
    int w = glutGetWindow();
   switch (key) {
      case 27:

      	glutDestroyWindow(w);
        exit(0);  /*  Escape key  */
        break;
      default:
         break;
   }
}

//Init callback
void init(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, .5f, 150.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0,0.0,0.0,1.0);
	SetTextureParameters(textureID);
}

//sets up texture using TGA object - several options provided
void SetTextureParameters(int id)
{
	char buffer[30];	//filename holder
	int status = 0;		//error codes for file read
	TGA myTGAImage;

	sprintf (buffer, "playmat.tga");		//load buffer with filename
	status = myTGAImage.readTGA(buffer);// Load ""Image.tga"" into a TGA object
	if(!status)
		cout <<"Error occurred = " << status << endl;

	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glBindTexture (GL_TEXTURE_2D, id);

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	gluBuild2DMipmaps(GL_TEXTURE_2D, myTGAImage.format, myTGAImage.width,
					  myTGAImage.height, myTGAImage.format, GL_UNSIGNED_BYTE, myTGAImage.data);

	//or just build a single texture
	//glTexImage2D (GL_TEXTURE_2D, 0, myTGAImage.format, myTGAImage.width, myTGAImage.height, 0, myTGAImage.format, GL_UNSIGNED_BYTE, myTGAImage.data);
	//

	// Lastly, we need to tell OpenGL the quality of our texture map.  GL_LINEAR is the smoothest.
	// GL_NEAREST is faster than GL_LINEAR, but looks blochy and pixilated.  Good for slower computers though.

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	//other possible settings to play with
	// OpenGL texture variables which change for various apps

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glEnable(GL_TEXTURE_2D);

}


int main(int argc, char **argv)
{

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Texture");
	glutFullScreen();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
}
