
// Colby Jeffries & Tommy Bacher
// roadrage.cpp

// INSERT DESCRIPTION HERE

// Controls:
//

// Modules
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <iostream>
#include "tgaClass.h"
#include <string.h>
#include "glm.h"
#include <vector>
#include <time.h>

using namespace std;

// OpenGL
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif

// Pi and related Numbers
#define PI 3.141592653
#define RADIANS_TO_DEGREES (180 / PI)

// Return value from inverse kinematics function.
struct angles {
  float a1, a2;
} typedef angles;

// Convenient way to store vertices.
struct vertex {
  GLfloat x, y, z;
} typedef vertex;

// Convenient way to store normals. (Not different that vertices, but provides
// clear code.)
struct normal {
  GLfloat x, y, z;
} typedef normal;

struct treeObj {
  float centerX, centerY, centerZ, rotation, scaleX, scaleY, scaleZ, fall;
  float fallAngle;
} typedef treeObj;

struct bikeMan {
  float x, y, z, r, t, scale;
  int dead;
} typedef bikeMan;

// Static variables
static float framesPerSecond = 0.0f;
static int lastTime	= 0;
static int frameCount = 0;
static char strFrameRate[20] = {0};
static GLfloat wheelAngle = 0;
static GLfloat bikeAngle = 0;
static float SPHERE_SIZE = 0.2;
static float speed = 0.0;
static float turn = 0.0;
static float heading = 0.0;
static float cameraX = 0.0;
static float cameraY = 10.0;
static float cameraZ = 0.0;
static int win = 0;
static GLMmodel* tree;
vector<treeObj> treeList;
vector<bikeMan> bikeList;

// Texture Ids
static GLuint floorId;
static GLuint wallId;
static GLuint woodId;

// Window Height and Width Respectively
static GLsizei wh = 1000, ww = 1000;

// Globals
GLfloat no_mat[] = {0.0, 0.0, 0.0, 1.0};
GLfloat no_shine[] = {0.0};

GLfloat light_pos[] = {0.0, 1.0, 0.0, 1.0};

GLfloat gridMatAmb[] = {0.2, 0.2, 1.0, 1.0};
GLfloat gridMatDif[] = {0.1, 0.1, 1.0, 1.0};

GLfloat rubberMatAmb[] = {0.05, 0.05, 0.05, 1.0};
GLfloat rubberMatDif[] = {0.1, 0.1, 0.1, 1.0};
GLfloat rubberMatSpec[] = {0.3, 0.3, 0.3, 1.0};
GLfloat rubberMatShin[] = {20.0};

GLfloat steelMatAmb[] = {0.5, 0.5, 0.5, 1.0};
GLfloat steelMatDif[] = {0.55, 0.55, 0.55, 1.0};
GLfloat steelMatSpec[] = {1.0, 1.0 ,1.0, 1.0};
GLfloat steelMatShin[] = {100.0};

GLfloat blackPlasticMatAmb[] = {0.05, 0.05, 0.05, 1.0};
GLfloat blackPlasticMatDif[] = {0.1, 0.1, 0.1, 1.0};
GLfloat blackPlasticMatSpec[] = {0.5, 0.5, 0.5, 1.0};
GLfloat blackPlasticMatShin[] = {20.0};

GLfloat bikePaintMatAmb[] = {0.9, 0.9, 0.1, 1.0};
GLfloat bikePaintMatDif[] = {0.95, 0.95, 0.1, 1.0};
GLfloat bikePaintMatSpec[] = {1.0, 1.0 ,1.0, 1.0};
GLfloat bikePaintMatShin[] = {100.0};

GLfloat guyMatAmb[] = {0.3, 0.5, 0.6, 1.0};
GLfloat guyMatDif[] = {0.3, 0.5, 0.6, 1.0};
GLfloat guyMatSpec[] = {1.0, 1.0 ,1.0, 1.0};
GLfloat guyMatShin[] = {100.0};

GLfloat worldMatAmb[] = {1.0, 1.0, 1.0, 1.0};
GLfloat worldMatDif[] = {1.0, 1.0, 1.0, 1.0};
GLfloat worldMatSpec[] = {1.0, 1.0 ,1.0, 1.0};
GLfloat worldMatShin[] = {100.0};

// Function prototypes
void display(void);
void makeVertex(vertex, normal);
static void mouse(int, int, int, int);
static void motion(int, int);
void init(void);
void resetMats(void);
void CalculateFrameRate(void);
void makeWheel(void);
void makeBike(void);
void makePerson(void);
void makeBikeEntity(float, float, float, float, float, float);
void setTextureParameters(GLuint, string);
angles invert(float);
void loadTextures(void);
void loadModels(void);
void generateTrees(int);
int checkTreeCollisions(float, float);
int checkBikeCollisions(float, float);
void knockDownTrees(void);
void generateBikeMen(int);
void checkWin(void);

void checkWin(void) {
  if (bikeList.empty()) {
    win = 1;
  }
}

void generateBikeMen(int numMen) {
  int i = 0;
  while (i < numMen) {
    bikeMan temp;
    temp.scale = rand()%3 + 3;
    temp.x = rand()%1950 - 975; temp.y = 2.0*temp.scale;
    temp.z = rand()%1950 - 975; temp.t = rand()%70 + 5;
    temp.r = rand()%100 + 10; temp.dead = 0;
    int check = checkTreeCollisions(temp.x, temp.y);
    int check2 = checkBikeCollisions(temp.x, temp.y);
    if ((check < 0) && (check2 < 0)) {
      bikeList.push_back(temp);
      i++;
    }
  }
}

void knockDownTrees(void) {
  for(int i=0; i<treeList.size(); i++) {
    if ((treeList[i].fall > 0) && (treeList[i].fall < 90)) {
      treeList[i].fall = treeList[i].fall + 1;
    }
    if (treeList[i].fall >= 90) {
      treeList[i].centerY = treeList[i].centerY - 0.1;
    }
    if (treeList[i].centerY < -100) {
      treeList.erase(treeList.begin() + i);
    }
  }
}

void killBikeMen(void) {
  for(int i=0; i<bikeList.size(); i++) {
    if (bikeList[i].dead > 0) {
      bikeList[i].y = bikeList[i].y + 2;
    }
    if (bikeList[i].y > 500) {
      bikeList.erase(bikeList.begin() + i);
    }
  }
}

int checkTreeCollisions(float x, float y) {
  for(int i=0; i<treeList.size(); i++) {
    if (treeList[i].fall < 45) {
      float dis = treeList[i].scaleX * 2;
      if ((treeList[i].centerX - dis < x) && (x < treeList[i].centerX + dis)) {
        if ((treeList[i].centerZ - dis < y) && (y < treeList[i].centerZ + dis)) {
          return i;
        }
      }
    }
  }
  return -1;
}

int checkBikeCollisions(float x, float y) {
  for(int i=0; i<bikeList.size(); i++) {
    float dis = bikeList[i].r;
    if ((bikeList[i].x - dis < x) && (x < bikeList[i].x + dis)) {
      if ((bikeList[i].z - dis < y) && (y < bikeList[i].z + dis)) {
        return i;
      }
    }
  }
  return -1;
}

void generateTrees(int numTrees) {
  int i = 0;
  while (i < numTrees) {
    treeObj temp;
    temp.scaleX = rand()%50 + 5; temp.scaleY = rand()%20 + 40;
    temp.scaleZ = rand()%5 + 2;
    temp.centerX = rand()%1900 - 950; temp.centerY = 0;
    temp.centerZ = rand()%1900 - 950; temp.rotation = rand()%180;
    temp.fall = 0; temp.fallAngle = 0;
    int check = checkTreeCollisions(temp.centerX, temp.centerZ);
    if (check < 0) {
      treeList.push_back(temp);
      i++;
    }
  }
}

void loadModels(void) {
  tree = (GLMmodel*)malloc(sizeof(GLMmodel));
  tree = glmReadOBJ("tree-matted.obj");
}

void setTextureParameters(GLuint id, string name)
{
	int status = 0;
	TGA myTGAImage;
  char *cstr = new char[name.length() + 1];
  strcpy(cstr, name.c_str());
	status = myTGAImage.readTGA(cstr);
  delete [] cstr;
	if(!status)
		cout <<"Error occurred = " << status << endl;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, id);
  gluBuild2DMipmaps(GL_TEXTURE_2D, myTGAImage.format, myTGAImage.width,
            myTGAImage.height, myTGAImage.format, GL_UNSIGNED_BYTE, myTGAImage.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void loadTextures(void) {
  glGenTextures(1, &floorId);
  setTextureParameters(floorId, "ground.tga");
  glGenTextures(1, &wallId);
  setTextureParameters(wallId, "distant-forest.tga");
  glGenTextures(1, &woodId);
  setTextureParameters(woodId, "download.tga");
}

// Inverse Kinematics Equation
angles invert(float angle) {
  // Target position.
  float x = cos((angle) * (1 / RADIANS_TO_DEGREES)) * 1.0 + 1.7;
  float y = sin((angle) * (1 / RADIANS_TO_DEGREES)) * 2.5 + 4.3;
  // Leg segment lengths.
  float l1 = 4.0;
  float l2 = 4.0;

  // Angle calculations.
  float sa2 = sqrt(1.0-pow((pow(x, 2.0) + pow(y, 2.0) - pow(l1, 2.0) - pow(l2, 2.0)) / (2.0 * l1 * l2), 2.0));
  float ca2 = (pow(x, 2.0) + pow(y, 2.0) - pow(l1, 2.0) - pow(l2, 2.0)) / (2.0 * l1 * l2);
  float a21 = atan2(sa2, ca2);
  float a22 = atan2(-sa2, ca2);
  float a11 = atan2(sqrt(1.0 - pow((x * (l1 + l2 * ca2) + y * l2 * sa2) / (pow(x, 2.0) + pow(y, 2.0)), 2.0)), (x * (l1 + l2 * ca2 + y * l2 * sa2) / (pow(x, 2.0) + pow(y, 2.0))));
  float a12 = atan2(-sqrt(1.0 - pow((x * (l1 + l2 * ca2) + y * l2 * sa2) / (pow(x, 2.0) + pow(y, 2.0)), 2.0)), (x * (l1 + l2 * ca2 + y * l2 * sa2) / (pow(x, 2.0) + pow(y, 2.0))));

  // Arrays of angle combinations.
  float a1[] = {a11, a11, a12, a12};
  float a2[] = {a21, a22, a21, a22};

  // Finds the closest solution.
  float upper = a11 * RADIANS_TO_DEGREES;
  float lower = a21 * RADIANS_TO_DEGREES;
  float minDist = 100.0;
  for(int i=0; i<4; i++) {
    float px = l1 * cos(a1[i]) + l2 * (cos(a1[i]) * cos(a2[i]) - sin(a1[i]) * sin(a2[i]));
    float py = l1 * sin(a1[i]) + l2 * (sin(a1[i]) * cos(a2[i]) + cos(a1[i]) * sin(a2[i]));
    float dist = sqrt(pow(x - px, 2) + pow(y - py, 2));
    if (dist < minDist) {
      upper = a1[i] * RADIANS_TO_DEGREES;
      lower = a2[i] * RADIANS_TO_DEGREES;
      minDist = dist;
    }
  }

  angles a; a.a1 = upper; a.a2 = lower;
  return a;
}

// Allows quick and easy vertex creation.
// Takes a vertex v and a normal n and draws a vertex at that location.
void makeVertex(vertex v, normal n) {
  glNormal3f(n.x, n.y, n.z);
  glVertex3f(v.x, v.y, v.z);
}

// Generates the person riding the bike.
void makePerson(void){
    resetMats();
    glMaterialfv(GL_FRONT, GL_AMBIENT, guyMatAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, guyMatDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, guyMatSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, guyMatShin);

    // Head
    glPushMatrix();
      glTranslatef(-3.0, 10.5, 0);
      glPushMatrix();
        glutSolidTeapot(1.5);
      glPopMatrix();
    glPopMatrix();

    // Torso
    glPushMatrix();
      glPushMatrix();
          glTranslatef(-3.0, 9.0, 0);
          glPushMatrix();
              glScalef(0.5, 0.5, 2.5);
              glutSolidCube(2.0);
          glPopMatrix();
      glPopMatrix();

      glPushMatrix();
          glTranslatef(-3.0, 8.0, 0);
          glPushMatrix();
              glScalef(0.45, 0.6, 1.75);
              glutSolidCube(2.0);
          glPopMatrix();
      glPopMatrix();

      glPushMatrix();
        glTranslatef(-3.0, 7.0, 0);
        glPushMatrix();
          glScalef(0.4, 0.6, 1.5);
          glutSolidCube(2.0);
        glPopMatrix();
      glPopMatrix();

      glPushMatrix();
        glTranslatef(-3.0, 6.0, 0);
        glPushMatrix();
          glScalef(0.4, 0.4, 1.5);
          glutSolidCube(2.0);
        glPopMatrix();
      glPopMatrix();

      glPushMatrix();
        glTranslatef(-3.0, 5.5, 0);
        glPushMatrix();
          glScalef(0.4, 0.4, 1.5);
          glutSolidCube(2.0);
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();

    // Arm 1
    glPushMatrix();
      glRotatef(-30, 0, 0, 1);
      glPushMatrix();
        glTranslatef(-5.4, 6.0, 2.0);
        glPushMatrix();
          glScalef(3.5, 0.75, 0.75);
          glutSolidCube(1.0);
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();

    // Arm 2
    glPushMatrix();
      glRotatef(-30, 0, 0, 1);
      glPushMatrix();
        glTranslatef(-5.4, 6.0, -2.0);
        glPushMatrix();
          glScalef(3.5, 0.75, 0.75);
          glutSolidCube(1.0);
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();

    // Leg angles.
    angles right = invert(wheelAngle + 105);
    angles left = invert(wheelAngle - 75);

    // Left Leg
    glPushMatrix();
      glTranslatef(-3.0, 5.0, -1.5);
      glPushMatrix();
        glRotatef(-left.a1, 0, 0, 1);
        glPushMatrix();
          glTranslatef(2.0, 0.0, 0.0);
          glScalef(4.0, 0.75, 0.75);
          glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
          glTranslatef(4.0, 0.0, 0.0);
          glPushMatrix();
            glRotatef(-left.a2, 0, 0, 1);
            glPushMatrix();
              glTranslatef(2.0, 0.0, 0.0);
              glScalef(4.0, 0.75, 0.75);
              glutSolidCube(1.0);
            glPopMatrix();
            // Foot (Attached to pedal).
            glPushMatrix();
              glTranslatef(3.7, 0.5, 0.0);
              glPushMatrix();
                glRotatef(left.a2, 0, 0, 1);
                glPushMatrix();
                  glScalef(2.0, 0.5, 1.0);
                  glutSolidCube(1.0);
                glPopMatrix();
              glPopMatrix();
            glPopMatrix();
          glPopMatrix();
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();

    // Right Leg
    glPushMatrix();
      glTranslatef(-3.0, 5.0, 1.5);
      glPushMatrix();
        glRotatef(-right.a1, 0, 0, 1);
        glPushMatrix();
          glTranslatef(2.0, 0.0, 0.0);
          glScalef(4.0, 0.75, 0.75);
          glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
          glTranslatef(4.0, 0.0, 0.0);
          glPushMatrix();
            glRotatef(-right.a2, 0, 0, 1);
            glPushMatrix();
              glTranslatef(2.0, 0.0, 0.0);
              glScalef(4.0, 0.75, 0.75);
              glutSolidCube(1.0);
            glPopMatrix();
            // Foot (Attached to pedal).
            glPushMatrix();
              glTranslatef(3.7, 0.5, 0.0);
              glPushMatrix();
                glRotatef(right.a2, 0, 0, 1);
                glPushMatrix();
                  glScalef(2.0, 0.5, 1.0);
                  glutSolidCube(1.0);
                glPopMatrix();
              glPopMatrix();
            glPopMatrix();
          glPopMatrix();
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();
}

// Generates the bike.
void makeBike(void) {

  // Front wheel.
  glPushMatrix();
    glTranslatef(5.0, 0.0, 0.0);
    makeWheel();
  glPopMatrix();

  // Back wheel.
  glPushMatrix();
    glTranslatef(-5.0, 0.0, 0.0);
    makeWheel();

    // Drive Gear
    glPushMatrix();
      resetMats();
      glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
      glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
      glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
      glTranslatef(0.0, 0.0, 0.6);
      for(int i=0; i < 60; i++) {
          glPushMatrix();
          glRotatef(i*3, 0.0, 0.0, 1.0);
          glPushMatrix();
            float off = 0.0;
            if ((i % 5) == 0) {
              off = 0.3;
            }
            glScalef(0.1, 2.0 + off, 0.05);
            glutSolidCube(1.0);
          glPopMatrix();
        glPopMatrix();
      }
    glPopMatrix();
  glPopMatrix();

  // Chain
  glPushMatrix();
  glRotatef(5, 0.0, 0.0, 1.0);
    for(int i=0; i<60; i++) {
      glPushMatrix();
        glTranslatef(-cos((wheelAngle + 3.0 * i) * 2 * (1 / RADIANS_TO_DEGREES)) * 3.3 - 2.7,
                     sin((wheelAngle + 3.0 * i) * 2 * (1 / RADIANS_TO_DEGREES)) * 1.7 + 0.4,
                     0.6);
        glutSolidSphere(0.1, 4, 4);
      glPopMatrix();
    }
  glPopMatrix();

  // Frame

  // Top Bar.
  resetMats();
  glMaterialfv(GL_FRONT, GL_AMBIENT, bikePaintMatAmb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, bikePaintMatDif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, bikePaintMatSpec);
  glMaterialfv(GL_FRONT, GL_SHININESS, bikePaintMatShin);
  glPushMatrix();
    glTranslatef(-2.5, 4.0, 0.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 5.9, 20, 20);
    glutSolidSphere(SPHERE_SIZE, 20, 20);
    glPopMatrix();
  glPopMatrix();

  // Seat
  resetMats();
  glMaterialfv(GL_FRONT, GL_AMBIENT, rubberMatAmb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, rubberMatDif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, rubberMatSpec);
  glMaterialfv(GL_FRONT, GL_SHININESS, rubberMatShin);
  glPushMatrix();
    glTranslatef(-2.8, 5.0, 0.0);
    glPushMatrix();
      glScalef(2.5, 0.5, 1.5);
      glutSolidCube(1.0);
    glPopMatrix();
  glPopMatrix();

  // Steering Column
  resetMats();
  glMaterialfv(GL_FRONT, GL_AMBIENT, bikePaintMatAmb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, bikePaintMatDif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, bikePaintMatSpec);
  glMaterialfv(GL_FRONT, GL_SHININESS, bikePaintMatShin);
  glPushMatrix();
    glTranslatef(3.0, 5.0, 0.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(70, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 2.0, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 2.0);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();
  glPushMatrix();
    resetMats();
    glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
    glTranslatef(3.0, 5.0, 0.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(-110, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.15, 0.15, 1.0, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 1.0);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Steering Cross
  glPushMatrix();
    glTranslatef(2.6, 6.0, -2.0);
    glPushMatrix();
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 4.0, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 4.0);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Handlebars
  glPushMatrix();
    glTranslatef(2.6, 6.0, -2.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(-160, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 2.0, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 2.0);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
      glPushMatrix();
        resetMats();
        glMaterialfv(GL_FRONT, GL_AMBIENT, rubberMatAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, rubberMatDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, rubberMatSpec);
        glMaterialfv(GL_FRONT, GL_SHININESS, rubberMatShin);
        glTranslatef(0.0, 0.0, 1.0);
        gluCylinder(gluNewQuadric(), 0.3, 0.3, 2.0, 20, 20);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
        glPushMatrix();
          glTranslatef(0.0, 0.0, 2.0);
          glutSolidSphere(SPHERE_SIZE, 20, 20);
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();
  glPushMatrix();
    resetMats();
    glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
    glTranslatef(2.6, 6.0, 2.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(-160, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 2.0, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 2.0);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
      glPushMatrix();
        resetMats();
        glMaterialfv(GL_FRONT, GL_AMBIENT, rubberMatAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, rubberMatDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, rubberMatSpec);
        glMaterialfv(GL_FRONT, GL_SHININESS, rubberMatShin);
        glTranslatef(0.0, 0.0, 1.0);
        gluCylinder(gluNewQuadric(), 0.3, 0.3, 2.0, 20, 20);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
        glPushMatrix();
          glTranslatef(0.0, 0.0, 2.0);
          glutSolidSphere(SPHERE_SIZE, 20, 20);
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Front Cross
  glMaterialfv(GL_FRONT, GL_AMBIENT, bikePaintMatAmb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, bikePaintMatDif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, bikePaintMatSpec);
  glMaterialfv(GL_FRONT, GL_SHININESS, bikePaintMatShin);
  glPushMatrix();
    glTranslatef(3.7, 3.0, -0.75);
    glPushMatrix();
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 1.5, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 1.5);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Front Wheel Posts
  glPushMatrix();
    glTranslatef(3.7, 3.0, -0.75);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(67.5, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 3.25, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 3.25);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();
  glPushMatrix();
    glTranslatef(3.7, 3.0, 0.75);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(67.5, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 3.25, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 3.25);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Rear Bar
  glPushMatrix();
    glTranslatef(-2.5, 4.0, 0.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(130, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 1.5, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 1.5);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Rear Cross
  glPushMatrix();
    glTranslatef(-3.5, 2.8, -0.75);
    glPushMatrix();
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 1.5, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 1.5);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Rear Wheel Posts
  glPushMatrix();
    glTranslatef(-3.5, 2.8, -0.75);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(117.5, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 3.25, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 3.25);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();
  glPushMatrix();
    glTranslatef(-3.5, 2.8, 0.75);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(117.5, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 3.25, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 3.25);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Seat Post
  glPushMatrix();
    glTranslatef(-3.0, 5.0, 0.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(60, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 5.0, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 5.0);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Front Control
  glPushMatrix();
    glTranslatef(3.25, 4.0, 0.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(135, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 5.0, 20, 20);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 5.0);
        glutSolidSphere(SPHERE_SIZE, 20, 20);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Pedal Assembly
  glPushMatrix();
    glTranslatef(-0.45, 0.35, -0.5);
    gluCylinder(gluNewQuadric(), 0.3, 0.3, 1.0, 20, 20);
    glutSolidSphere(SPHERE_SIZE, 20, 20);
    glPushMatrix();
      glTranslatef(0.0, 0.0, 1.0);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
    glPopMatrix();
    glPushMatrix();
      resetMats();
      glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
      glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
      glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
      glRotatef(wheelAngle, 0.0, 0.0, -1.0);
      glTranslatef(0.0, 0.0, -0.5);
      gluCylinder(gluNewQuadric(), 0.25, 0.25, 2.0, 20, 20);
      glutSolidSphere(0.05 + SPHERE_SIZE, 20, 20);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 2.0);
        glutSolidSphere(0.05 + SPHERE_SIZE, 20, 20);
      glPopMatrix();

      // Pedal Gear
      glPushMatrix();
        resetMats();
        glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
        glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
        glTranslatef(0.0, 0.0, 1.6);
        for(int i=0; i < 60; i++) {
            glPushMatrix();
            glRotatef(i*3, 0.0, 0.0, 1.0);
            glPushMatrix();
              float off = 0.0;
              if ((i % 5) == 0) {
                off = 0.3;
              }
              glScalef(0.1, 2.0 + off, 0.05);
              glutSolidCube(1.0);
            glPopMatrix();
          glPopMatrix();
        }
      glPopMatrix();

      // Left Pedal
      glPushMatrix();
        resetMats();
        glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
        glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
        glTranslatef(0.0, 1.0, 0.1);
        glPushMatrix();
          glScalef(0.3, 2.5, 0.3);
          glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
          glTranslatef(0.0, 1.0, -0.5);
          glPushMatrix();
            glRotatef(wheelAngle, 0.0, 0.0, 1.0);
            glPushMatrix();
              resetMats();
              glMaterialfv(GL_FRONT, GL_AMBIENT, blackPlasticMatAmb);
              glMaterialfv(GL_FRONT, GL_DIFFUSE, blackPlasticMatDif);
              glMaterialfv(GL_FRONT, GL_SPECULAR, blackPlasticMatSpec);
              glMaterialfv(GL_FRONT, GL_SHININESS, blackPlasticMatShin);
              glScalef(0.6, 0.2, 1.0);
              glutSolidCube(1.0);
            glPopMatrix();
          glPopMatrix();
        glPopMatrix();
      glPopMatrix();

      // Right Pedal
      glPushMatrix();
        resetMats();
        glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
        glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
        glTranslatef(0.0, -1.0, 1.9);
        glPushMatrix();
          glScalef(0.3, 2.5, 0.3);
          glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
          glTranslatef(0.0, -1.0, 0.5);
          glPushMatrix();
            glRotatef(wheelAngle, 0.0, 0.0, 1.0);
            glPushMatrix();
              resetMats();
              glMaterialfv(GL_FRONT, GL_AMBIENT, blackPlasticMatAmb);
              glMaterialfv(GL_FRONT, GL_DIFFUSE, blackPlasticMatDif);
              glMaterialfv(GL_FRONT, GL_SPECULAR, blackPlasticMatSpec);
              glMaterialfv(GL_FRONT, GL_SHININESS, blackPlasticMatShin);
              glScalef(0.6, 0.2, 1.0);
              glutSolidCube(1.0);
            glPopMatrix();
          glPopMatrix();
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();
}

// Generates a bicycle wheel at the origin.
void makeWheel(void) {
  glRotatef(wheelAngle, 0.0, 0.0, -1.0);

  // Tire
  glMaterialfv(GL_FRONT, GL_AMBIENT, rubberMatAmb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, rubberMatDif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, rubberMatSpec);
  glMaterialfv(GL_FRONT, GL_SHININESS, rubberMatShin);
  glutSolidTorus(0.5, 2.0, 20, 20);

  // Rim
  glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
  glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
  glutSolidTorus(0.45, 1.9, 20, 20);

  // Spokes
  for(int i=0; i<24; i++) {
    glPushMatrix();
      glRotatef(90, 1.0, 0.0, 0.0);
      if (i % 2 == 0) {
        glRotatef(3.0, 1.0, 0.0, 0.0);
      }
      else {
        glRotatef(-3.0, 1.0, 0.0, 0.0);
      }
      glPushMatrix();
        glRotatef(i * 15, 0.0, 1.0, 0.0);
        gluCylinder(gluNewQuadric(), 0.05, 0.05, 2.0, 5, 5);
      glPopMatrix();
    glPopMatrix();
  }

  // Axel
  glPushMatrix();
    glTranslatef(0.0, 0.0, -1.0);
    gluCylinder(gluNewQuadric(), 0.2, 0.2, 2.0, 20, 20);
    glutSolidSphere(SPHERE_SIZE, 20, 20);
    glPushMatrix();
      glTranslatef(0.0, 0.0, 2.0);
      glutSolidSphere(SPHERE_SIZE, 20, 20);
    glPopMatrix();
  glPopMatrix();
  resetMats();
}

void makeBikeEntity(float centerX, float centerY, float centerZ, float radius, float tilt, float scale) {
  glTranslatef(centerX, centerY, centerZ);
  glRotatef(bikeAngle, 0.0, 1.0, 0.0);
  glPushMatrix();
    glTranslatef(0.0, 0.0, radius);
    glRotatef(tilt, -1.0, 0.0, 0.0);
    glPushMatrix();
      glScalef(scale, scale, scale);
      makeBike();
      makePerson();
    glPopMatrix();
  glPopMatrix();
}

// Display callback.
// Generates building and applies the rotations.
void display(void) {
  resetMats();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glEnable(GL_LIGHT0);

  if (win == 0) {
    gluLookAt(cameraX, cameraY, cameraZ,
              cameraX + sin(heading/RADIANS_TO_DEGREES) * 1000, 0.0,
              cameraZ - cos(heading/RADIANS_TO_DEGREES) * 1000, 0, 1, 0);
  }
  else {
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0, 1, 0);
  }

  // Place the light.
  glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glPopMatrix();

  glBindTexture(GL_TEXTURE_2D, woodId);
  glEnable(GL_TEXTURE_2D);

  for(int i=0; i<treeList.size(); i++) {
    glPushMatrix();
      glTranslatef(treeList[i].centerX, treeList[i].centerY, treeList[i].centerZ);
      glRotatef(treeList[i].fallAngle, 0, -1, 0);
      glPushMatrix();
        glRotatef(treeList[i].fall, 1, 0, 0);
        glRotatef(treeList[i].rotation + treeList[i].fallAngle, 0, 1, 0);
        glPushMatrix();
          glScalef(treeList[i].scaleX, treeList[i].scaleY, treeList[i].scaleZ);
          glmDraw(tree, GLM_SMOOTH|GLM_MATERIAL|GLM_TEXTURE);
        glPopMatrix();
        glPushMatrix();
          glRotatef(-90, 0, 1, 0);
          glScalef(treeList[i].scaleY, treeList[i].scaleY, treeList[i].scaleZ);
          glmDraw(tree, GLM_SMOOTH|GLM_MATERIAL|GLM_TEXTURE);
        glPopMatrix();
      glPopMatrix();
    glPopMatrix();
  }

  glDisable(GL_TEXTURE_2D);

  for(int i=0; i<bikeList.size(); i++) {
    glPushMatrix();
      makeBikeEntity(bikeList[i].x, bikeList[i].y, bikeList[i].z, bikeList[i].r,
                    bikeList[i].t, bikeList[i].scale);
    glPopMatrix();
  }

  glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, floorId);
    glEnable(GL_TEXTURE_2D);
    resetMats();

    glMaterialfv(GL_FRONT, GL_AMBIENT, worldMatAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, worldMatDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, worldMatSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, worldMatShin);

    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 50.0f);
      glNormal3f(0.0, 1.0, 0.0);
      glVertex3f(-1000, 0, -1000);
      glTexCoord2f(0.0f, 0.0f);
      glNormal3f(0.0, 1.0, 0.0);
      glVertex3f(-1000, 0, 1000);
      glTexCoord2f(50.0f, 0.0f);
      glNormal3f(0.0, 1.0, 0.0);
      glVertex3f(1000, 0, 1000);
      glTexCoord2f(50.0f, 50.0f);
      glNormal3f(0.0, 1.0, 0.0);
      glVertex3f(1000, 0, -1000);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, wallId);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);
      glNormal3f(0.0, 0.0, 1.0);
      glVertex3f(-1000, 400, -1000);
      glTexCoord2f(0.0f, 0.10f);
      glNormal3f(0.0, 0.0, 1.0);
      glVertex3f(-1000, 0, -1000);
      glTexCoord2f(5.0f, 0.10f);
      glNormal3f(0.0, 0.0, 1.0);
      glVertex3f(1000, 0, -1000);
      glTexCoord2f(5.0f, 1.0f);
      glNormal3f(0.0, 0.0, 1.0);
      glVertex3f(1000, 400, -1000);
    glEnd();

    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);
      glNormal3f(0.0, 0.0, -1.0);
      glVertex3f(-1000, 400, 1000);
      glTexCoord2f(0.0f, 0.10f);
      glNormal3f(0.0, 0.0, -1.0);
      glVertex3f(-1000, 0, 1000);
      glTexCoord2f(5.0f, 0.10f);
      glNormal3f(0.0, 0.0, -1.0);
      glVertex3f(1000, 0, 1000);
      glTexCoord2f(5.0f, 1.0f);
      glNormal3f(0.0, 0.0, -1.0);
      glVertex3f(1000, 400, 1000);
    glEnd();

    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);
      glNormal3f(1.0, 0.0, 0.0);
      glVertex3f(-1000, 400, 1000);
      glTexCoord2f(0.0f, 0.10f);
      glNormal3f(1.0, 0.0, 0.0);
      glVertex3f(-1000, 0, 1000);
      glTexCoord2f(5.0f, 0.10f);
      glNormal3f(1.0, 0.0, 0.0);
      glVertex3f(-1000, 0, -1000);
      glTexCoord2f(5.0f, 1.0f);
      glNormal3f(1.0, 0.0, 0.0);
      glVertex3f(-1000, 400, -1000);
    glEnd();

    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);
      glNormal3f(-1.0, 0.0, 0.0);
      glVertex3f(1000, 400, 1000);
      glTexCoord2f(0.0f, 0.10f);
      glNormal3f(-1.0, 0.0, 0.0);
      glVertex3f(1000, 0, 1000);
      glTexCoord2f(5.0f, 0.10f);
      glNormal3f(-1.0, 0.0, 0.0);
      glVertex3f(1000, 0, -1000);
      glTexCoord2f(5.0f, 1.0f);
      glNormal3f(-1.0, 0.0, 0.0);
      glVertex3f(1000, 400, -1000);
    glEnd();
  glPopMatrix();

  glDisable(GL_TEXTURE_2D);
  resetMats();
  glutSwapBuffers();
	CalculateFrameRate();
}

// Mouse callback that allows the user to rotate the building.
static void mouse(int button, int state, int x, int y) {

}

// Motion callback that changes rotation angles.
static void motion(int x, int y) {

}

// Resets all materials to ensure no unintentional effects.
void resetMats(void) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, no_mat);
  glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
  glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
  glMaterialfv(GL_FRONT, GL_SHININESS, no_shine);
}

// Initialization function.
void init(void) {
  glClearColor(0.7, 0.7, 0.7, 1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glMatrixMode(GL_PROJECTION);
  loadTextures();
  loadModels();
  generateTrees(50);
  generateBikeMen(5);
}

// Keyboard callback that allows the user to quit, zoom, and toggle MSAA.
void keyboard(unsigned char key, int x, int y){
  switch (key) {
    case 'q':
      delete tree;
      exit(0);
      break;
    case 'w':
      if (speed<5.0) {
        speed = speed + 1.0;
      }
      break;
    case 'a':
      if (turn>-1.0) {
        turn = turn - 0.5;
      }
      break;
    case 's':
      if (speed>-2.0) {
        speed = speed - 0.5;
      }
      break;
    case 'd':
      if (turn<1.0) {
        turn = turn + 0.5;
      }
      break;
  }
}

// Reshape callback that preserves aspect ratio. Also implements the zoom.
void myReshape(int w, int h) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, w, h);
  gluPerspective(60.0f, (GLfloat)w/(GLfloat)h, 0.1, 4000.0);
  glMatrixMode(GL_MODELVIEW);
  ww = w;
 	wh = h;
}

// Idle callback that does a small z axis rotation and flashes the antenna light.
void idle() {
  checkWin();

  if (wheelAngle > 360.0)
  		wheelAngle = wheelAngle - 360.0;
  	wheelAngle += 5;

  if (bikeAngle > 360.0)
  		bikeAngle = bikeAngle - 360.0;
  	bikeAngle += 1;

  if ((speed < 0.1) && (speed > -0.1)) {
    speed = 0;
  }
  else if (speed > 0) {
    speed = speed - 0.005;
  }
  else if (speed < 0) {
    speed = speed + 0.005;
  }

  if ((turn < 0.05) && (turn > -0.05)) {
    turn = 0;
  }
  else if (turn > 0) {
    turn = turn - 0.005;
  }
  else if (turn < 0) {
    turn = turn + 0.005;
  }

  heading = heading + turn;
  if (heading > 360) {
    heading = heading - 360;
  }
  if (heading < 0) {
    heading = heading + 360;
  }

  if (win == 0) {
    float prevX = cameraX;
    float prevZ = cameraZ;
    cameraX = cameraX + sin(heading/RADIANS_TO_DEGREES)*speed;
    cameraZ = cameraZ - cos(heading/RADIANS_TO_DEGREES)*speed;
    int check = checkTreeCollisions(cameraX, cameraZ);
    if (check > -1) {
      cameraX = prevX;
      cameraZ = prevZ;
      if (treeList[check].fall < 1) {
        treeList[check].fall = 1;
        treeList[check].fallAngle = heading + 180;
      }
    }
    check = checkBikeCollisions(cameraX, cameraZ);
    if (check > -1) {
      bikeList[check].dead = 1;
    }
    knockDownTrees();
    killBikeMen();
    if (cameraX > 990) {
      cameraX = 990;
    }
    if (cameraX < -990) {
      cameraX = -990;
    }
    if (cameraZ > 990) {
      cameraZ = 990;
    }
    if (cameraZ < -990) {
      cameraZ = -990;
    }
  }
  else {
    knockDownTrees();
    killBikeMen();
    int i = rand()%treeList.size();
    int j = rand()%100;
    if ((treeList[i].fall < 1) && (j < 5)) {
      treeList[i].fall = 1;
      treeList[i].fallAngle = rand()%360;
    }
    if (cameraY < 4000) {
      cameraY = cameraY + 1;
    }
  }
  glutPostRedisplay();
}

// Gives us the framerate in the title.
void CalculateFrameRate(void) {
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	++frameCount;
	int interval = currentTime - lastTime;
	if((interval) > 1000) {
	  framesPerSecond = frameCount /((interval)/1000.0f);
	  lastTime = currentTime;
		sprintf(strFrameRate, "OpenGL Road Rage: %d", int(framesPerSecond));
		glutSetWindowTitle(strFrameRate);
	  frameCount = 0;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv) {
  srand(time(NULL));
  // Initialize OpenGL.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);

  // Initial window size and position.
  glutInitWindowSize(800,800);
  glutInitWindowPosition(300,100);

  // Name the window.
  glutCreateWindow("OpenGL Road Rage: 0");

  // Set callbacks.
  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(myReshape);
  glutIdleFunc(idle);

  // Initialization function.
  init();

  // Mainloop
  glutMainLoop();

  return 0;
}
