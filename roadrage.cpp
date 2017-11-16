
// Colby Jeffries & Tommy Bacher
// roadrage.cpp

// This is a game! Drive a car, run over all of the bike men to win!
// Trees are obstacles, but you can knock them down!

// Controls:
//  w: Accelerator
//  s: Brake/Reverse
//  a: Turn Left
//  d: Turn Right
//  v: Swap between 1st and 3rd person.
//  h: Toggles horror mode. (THICK FOG).

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

// Contains the relevant information to generate a tree, animate it and detect
// collisions with it.
struct treeObj {
  float centerX, centerY, centerZ, rotation, scaleX, scaleY, scaleZ, fall;
  float fallAngle;
} typedef treeObj;

// Same as the tree object, but for the bike man.
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
static float cameraY = 40.0;
static float cameraZ = 0.0;
static int win = 0;
static GLMmodel* tree;
static GLMmodel* steeringWheel;
static int view = 0;
static int horror = 0;
static GLfloat carWheelRotation = 0;
static float carWidth = 20; // From center of car.
static float carLength = 30; // Same.
static int numBikeMen = 5;
static int numTree = 50;

// Global data structures
vector<treeObj> treeList;
vector<bikeMan> bikeList;
int keyboard[256] = {0};
// Save the calculations for all bike men in one place to avoid repitition.
angles rightMan;
angles leftMan;

// Texture Ids
static GLuint floorId;
static GLuint wallId;
static GLuint woodId;
static GLuint velourId;

// Window Height and Width Respectively
static GLsizei wh = 1000, ww = 1000;

// Globals materials
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

GLfloat carPaintMatAmb[] = {0.9, 0.1, 0.9, 1.0};
GLfloat carPaintMatDif[] = {0.95, 0.1, 0.95, 1.0};
GLfloat carPaintMatSpec[] = {1.0, 1.0 ,1.0, 1.0};
GLfloat carPaintMatShin[] = {100.0};

GLfloat carInteriorPaintMatAmb[] = {0.7, 0.7, 0.7, 1.0};
GLfloat carInteriorPaintMatDif[] = {0.72, 0.72, 0.72, 1.0};
GLfloat carInteriorPaintMatSpec[] = {1.0, 1.0 ,1.0, 1.0};
GLfloat carInteriorPaintMatShin[] = {20.0};

GLfloat fogColor[] = {0.7, 0.7, 0.7, 1.0};

// Function prototypes
// Its probably about time for a header file.
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
int checkTreeCollisions(float, float, float, float);
int checkBikeCollisions(float, float, float, float);
void knockDownTrees(void);
void generateBikeMen(int);
void checkWin(void);
void reset(void);

// Resets the game.
void reset(void) {
  wheelAngle = 0; bikeAngle = 0; speed = 0.0; turn = 0.0; heading = 0.0;
  cameraX = 0.0; cameraY = 40.0; cameraZ = 0.0; win = 0; view = 0; horror = 0;
  carWheelRotation = 0;
  treeList.clear(); bikeList.clear();

  generateTrees(numTree);
  generateBikeMen(numBikeMen);
}

// Checks if the player wins, basically if all the bike men are dead.
void checkWin(void) {
  if (bikeList.empty()) {
    win = 1;
  }
}

// Generates a specified number of bike men, ensures they are not placed on
// trees or other bike men.
void generateBikeMen(int numMen) {
  int i = 0;
  while (i < numMen) {
    bikeMan temp;
    temp.scale = rand()%2 + 3;
    temp.x = rand()%1950 - 975; temp.y = 2.0*temp.scale;
    temp.z = rand()%1950 - 975; temp.t = rand()%70 + 5;
    temp.r = rand()%10 + 10; temp.dead = 0;
    int check = checkTreeCollisions(temp.x, temp.y, temp.r + temp.scale, temp.r * 1.5);
    int check2 = checkBikeCollisions(temp.x, temp.y, temp.r + temp.scale, temp.r * 1.5);
    if ((check < 0) && (check2 < 0)) {
      bikeList.push_back(temp);
      i++;
    }
  }
}

// Controls the animation for knocking down the tree. If the tree has been hit,
// slowly knock it down. Once it is all the way down, slowly slide into the
// ground. Once it is a certain distance down, remove it.
void knockDownTrees(void) {
  for(int i=0; i<treeList.size(); i++) {
    if ((treeList[i].fall > 0) && (treeList[i].fall < 90)) {
      treeList[i].fall = treeList[i].fall + 1;
    }
    if (treeList[i].fall >= 90) {
      treeList[i].centerY = treeList[i].centerY - 0.5;
    }
    if (treeList[i].centerY < -100) {
      treeList.erase(treeList.begin() + i);
    }
  }
}

// Controls the animation for sending bike men to heaven. If they have been hit,
// send them into the sky. Once they reach a certain height, remove them.
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

// Checks collisions of a specified rectangle with the trees. Parameters are
// the center of the rectangle and the distance from the center to the edges.
int checkTreeCollisions(float x, float z, float disx, float disz) {
  for(int i=0; i<treeList.size(); i++) {
    int check = 0;
    if (treeList[i].fall < 45) {
      check = 1;
      float dis = treeList[i].scaleX * 2;
      if ((treeList[i].centerX + dis < x - disx) || (x + disx < treeList[i].centerX - dis)) {
        check = 0;
      }
      if ((treeList[i].centerZ + dis < z - disz) || (z + disz < treeList[i].centerZ - dis)) {
        check = 0;
      }
    }
    if (check == 1) {
      return i;
    }
  }
  return -1;
}

// Same thing as tree collisions but with the bike men.
int checkBikeCollisions(float x, float z, float disx, float disz) {
  for(int i=0; i<bikeList.size(); i++) {
    float dis = bikeList[i].r * 1.5;
    int check = 1;
    if ((bikeList[i].x + dis < x - disx) || (x + disx < bikeList[i].x - dis)) {
      check = 0;
    }
    if ((bikeList[i].z + dis < z - disz) || (z + disz < bikeList[i].z - dis)) {
      check = 0;
    }
    if (check == 1) {
      return i;
    }
  }
  return -1;
}

// Generates trees. Ensures they are not on top of other trees. Since this is
// done before bike men are generated, they do not need to be checked.
void generateTrees(int numTrees) {
  int i = 0;
  while (i < numTrees) {
    treeObj temp;
    temp.scaleX = rand()%50 + 5; temp.scaleY = rand()%20 + 40;
    temp.scaleZ = rand()%5 + 2;
    temp.centerX = rand()%1980 - 990; temp.centerY = 0;
    temp.centerZ = rand()%1980 - 990; temp.rotation = rand()%180;
    temp.fall = 0; temp.fallAngle = 0;
    int check = checkTreeCollisions(temp.centerX, temp.centerZ, temp.scaleX*2, temp.scaleX*2);
    if (check < 0) {
      treeList.push_back(temp);
      i++;
    }
  }
}

// Loads the models in. Only done once.
void loadModels(void) {
  tree = (GLMmodel*)malloc(sizeof(GLMmodel));
  tree = glmReadOBJ("tree-matted.obj");
  steeringWheel = (GLMmodel*)malloc(sizeof(GLMmodel));
  steeringWheel = glmReadOBJ("wheel.obj");
}

// Loads a specified texture.
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

// Loads all of the textures. Only done once.
void loadTextures(void) {
  glGenTextures(1, &floorId);
  setTextureParameters(floorId, "ground.tga");
  glGenTextures(1, &wallId);
  setTextureParameters(wallId, "distant-forest.tga");
  glGenTextures(1, &woodId);
  setTextureParameters(woodId, "download.tga");
  glGenTextures(1, &velourId);
  setTextureParameters(velourId, "velour.tga");
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

    // Left Leg
    glPushMatrix();
      glTranslatef(-3.0, 5.0, -1.5);
      glPushMatrix();
        glRotatef(-leftMan.a1, 0, 0, 1);
        glPushMatrix();
          glTranslatef(2.0, 0.0, 0.0);
          glScalef(4.0, 0.75, 0.75);
          glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
          glTranslatef(4.0, 0.0, 0.0);
          glPushMatrix();
            glRotatef(-leftMan.a2, 0, 0, 1);
            glPushMatrix();
              glTranslatef(2.0, 0.0, 0.0);
              glScalef(4.0, 0.75, 0.75);
              glutSolidCube(1.0);
            glPopMatrix();
            // Foot (Attached to pedal).
            glPushMatrix();
              glTranslatef(3.7, 0.5, 0.0);
              glPushMatrix();
                glRotatef(leftMan.a2, 0, 0, 1);
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
        glRotatef(-rightMan.a1, 0, 0, 1);
        glPushMatrix();
          glTranslatef(2.0, 0.0, 0.0);
          glScalef(4.0, 0.75, 0.75);
          glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
          glTranslatef(4.0, 0.0, 0.0);
          glPushMatrix();
            glRotatef(-rightMan.a2, 0, 0, 1);
            glPushMatrix();
              glTranslatef(2.0, 0.0, 0.0);
              glScalef(4.0, 0.75, 0.75);
              glutSolidCube(1.0);
            glPopMatrix();
            // Foot (Attached to pedal).
            glPushMatrix();
              glTranslatef(3.7, 0.5, 0.0);
              glPushMatrix();
                glRotatef(rightMan.a2, 0, 0, 1);
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
    for(int i=0; i<30; i++) {
      glPushMatrix();
        glTranslatef(-cos((wheelAngle + 6.0 * i) * 2 * (1 / RADIANS_TO_DEGREES)) * 3.3 - 2.7,
                     sin((wheelAngle + 6.0 * i) * 2 * (1 / RADIANS_TO_DEGREES)) * 1.7 + 0.4,
                     0.6);
        glutSolidSphere(0.1, 3, 3);
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
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 5.9, 10, 10);
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
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 2.0, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 2.0);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
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
      gluCylinder(gluNewQuadric(), 0.15, 0.15, 1.0, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 1.0);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Steering Cross
  glPushMatrix();
    glTranslatef(2.6, 6.0, -2.0);
    glPushMatrix();
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 4.0, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 4.0);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Handlebars
  glPushMatrix();
    glTranslatef(2.6, 6.0, -2.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(-160, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 2.0, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 2.0);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
      glPushMatrix();
        resetMats();
        glMaterialfv(GL_FRONT, GL_AMBIENT, rubberMatAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, rubberMatDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, rubberMatSpec);
        glMaterialfv(GL_FRONT, GL_SHININESS, rubberMatShin);
        glTranslatef(0.0, 0.0, 1.0);
        gluCylinder(gluNewQuadric(), 0.3, 0.3, 2.0, 10, 10);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
        glPushMatrix();
          glTranslatef(0.0, 0.0, 2.0);
          glutSolidSphere(SPHERE_SIZE, 10, 10);
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
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 2.0, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 2.0);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
      glPushMatrix();
        resetMats();
        glMaterialfv(GL_FRONT, GL_AMBIENT, rubberMatAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, rubberMatDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, rubberMatSpec);
        glMaterialfv(GL_FRONT, GL_SHININESS, rubberMatShin);
        glTranslatef(0.0, 0.0, 1.0);
        gluCylinder(gluNewQuadric(), 0.3, 0.3, 2.0, 10, 10);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
        glPushMatrix();
          glTranslatef(0.0, 0.0, 2.0);
          glutSolidSphere(SPHERE_SIZE, 10, 10);
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
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 1.5, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 1.5);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
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
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 3.25);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();
  glPushMatrix();
    glTranslatef(3.7, 3.0, 0.75);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(67.5, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 3.25, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 3.25);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Rear Bar
  glPushMatrix();
    glTranslatef(-2.5, 4.0, 0.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(130, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 1.5, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 1.5);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Rear Cross
  glPushMatrix();
    glTranslatef(-3.5, 2.8, -0.75);
    glPushMatrix();
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 1.5, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 1.5);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Rear Wheel Posts
  glPushMatrix();
    glTranslatef(-3.5, 2.8, -0.75);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(117.5, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 3.25, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 3.25);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();
  glPushMatrix();
    glTranslatef(-3.5, 2.8, 0.75);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(117.5, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 3.25, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 3.25);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Seat Post
  glPushMatrix();
    glTranslatef(-3.0, 5.0, 0.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(60, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 5.0, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 5.0);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Front Control
  glPushMatrix();
    glTranslatef(3.25, 4.0, 0.0);
    glPushMatrix();
      glRotatef(90, 0.0, 1.0, 0.0);
      glRotatef(135, 1.0, 0.0, 0.0);
      gluCylinder(gluNewQuadric(), 0.2, 0.2, 5.0, 10, 10);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 5.0);
        glutSolidSphere(SPHERE_SIZE, 10, 10);
      glPopMatrix();
    glPopMatrix();
  glPopMatrix();

  // Pedal Assembly
  glPushMatrix();
    glTranslatef(-0.45, 0.35, -0.5);
    gluCylinder(gluNewQuadric(), 0.3, 0.3, 1.0, 10, 10);
    glutSolidSphere(SPHERE_SIZE, 10, 10);
    glPushMatrix();
      glTranslatef(0.0, 0.0, 1.0);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
    glPopMatrix();
    glPushMatrix();
      resetMats();
      glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
      glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
      glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
      glRotatef(wheelAngle, 0.0, 0.0, -1.0);
      glTranslatef(0.0, 0.0, -0.5);
      gluCylinder(gluNewQuadric(), 0.25, 0.25, 2.0, 10, 10);
      glutSolidSphere(0.05 + SPHERE_SIZE, 10, 10);
      glPushMatrix();
        glTranslatef(0.0, 0.0, 2.0);
        glutSolidSphere(0.05 + SPHERE_SIZE, 10, 10);
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
  glutSolidTorus(0.5, 2.0, 10, 10);

  // Rim
  glMaterialfv(GL_FRONT, GL_AMBIENT, steelMatAmb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, steelMatDif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, steelMatSpec);
  glMaterialfv(GL_FRONT, GL_SHININESS, steelMatShin);
  glutSolidTorus(0.45, 1.9, 10, 10);

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
        gluCylinder(gluNewQuadric(), 0.05, 0.05, 2.0, 3, 3);
      glPopMatrix();
    glPopMatrix();
  }

  // Axel
  glPushMatrix();
    glTranslatef(0.0, 0.0, -1.0);
    gluCylinder(gluNewQuadric(), 0.2, 0.2, 2.0, 10, 10);
    glutSolidSphere(SPHERE_SIZE, 10, 10);
    glPushMatrix();
      glTranslatef(0.0, 0.0, 2.0);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
    glPopMatrix();
  glPopMatrix();
  resetMats();
}

// Makes a car wheel. Based on the bike wheel. Modified a bit.
void makeCarWheel(void) {
  glRotatef(carWheelRotation, 0.0, 0.0, -1.0);
  glScalef(5.0, 5.0, 12.0);

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
        gluCylinder(gluNewQuadric(), 0.05, 0.05, 2.0, 3, 3);
      glPopMatrix();
    glPopMatrix();
  }

  // Axel
  glPushMatrix();
    glTranslatef(0.0, 0.0, -0.5);
    gluCylinder(gluNewQuadric(), 0.2, 0.2, 1.0, 10, 10);
    glutSolidSphere(SPHERE_SIZE, 10, 10);
    glPushMatrix();
      glTranslatef(0.0, 0.0, 1.0);
      glutSolidSphere(SPHERE_SIZE, 10, 10);
    glPopMatrix();
  glPopMatrix();
  resetMats();
}

// Generates a whole bike man based on parameters.
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

// Generates the car.
void makeCar() {
  glPushMatrix();
    glTranslatef(cameraX, 10.0, cameraZ);
    glRotatef(heading, 0, -1, 0);
    glPushMatrix();
      glRotatef(90, 0, 1, 0);
      glTranslatef(25.0, 0.0, 15.0);
      glPushMatrix();
        glRotatef(-turn * 3, 0, 1, 0);
        makeCarWheel();
      glPopMatrix();
    glPopMatrix();
    glPushMatrix();
      glRotatef(90, 0, 1, 0);
      glTranslatef(25.0, 0.0, -15.0);
      glPushMatrix();
        glRotatef(-turn * 3, 0, 1, 0);
        makeCarWheel();
      glPopMatrix();
    glPopMatrix();
    glPushMatrix();
      glRotatef(90, 0, 1, 0);
      glTranslatef(-25.0, 0.0, 15.0);
      makeCarWheel();
    glPopMatrix();
    glPushMatrix();
      glRotatef(90, 0, 1, 0);
      glTranslatef(-25.0, 0.0, -15.0);
      makeCarWheel();
    glPopMatrix();
    glPushMatrix();
      resetMats();
      glMaterialfv(GL_FRONT, GL_AMBIENT, carPaintMatAmb);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, carPaintMatDif);
      glMaterialfv(GL_FRONT, GL_SPECULAR, carPaintMatSpec);
      glMaterialfv(GL_FRONT, GL_SHININESS, carPaintMatShin);
      glPushMatrix();
        glTranslatef(0.0, 10.0, 0.0);
        glScalef(30.0, 20.0, 10.0);
        glutSolidCube(1.0);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(0.0, 10.0, 0.0);
        glScalef(15.0, 20.0, 60.0);
        glutSolidCube(1.0);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(0.0, 18.0, 0.0);
        glScalef(30.0, 10.0, 60.0);
        glutSolidCube(1.0);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(0.0, 20.0, -15.0);
        glScalef(30.0, 10.0, 5.0);
        glutSolidCube(1.0);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(14.0, 30.0, 8.0);
        glScalef(2.0, 15.0, 34.0);
        glutSolidCube(1.0);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(-14.0, 30.0, 8.0);
        glScalef(2.0, 15.0, 34.0);
        glutSolidCube(1.0);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(14.0, 30.0, -14.0);
        glScalef(2.0, 15.0, 2.0);
        glutSolidCube(1.0);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(-14.0, 30.0, -14.0);
        glScalef(2.0, 15.0, 2.0);
        glutSolidCube(1.0);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(0.0, 37.0, 8.0);
        glScalef(30.0, 2.0, 45.0);
        glutSolidCube(1.0);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(0.0, 30.0, 28.0);
        glScalef(30.0, 15.0, 5.0);
        glutSolidCube(1.0);
      glPopMatrix();
    glPopMatrix();
    glPushMatrix();
      resetMats();
      glMaterialfv(GL_FRONT, GL_AMBIENT, carInteriorPaintMatAmb);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, carInteriorPaintMatDif);
      glMaterialfv(GL_FRONT, GL_SPECULAR, carInteriorPaintMatSpec);
      glMaterialfv(GL_FRONT, GL_SHININESS, carInteriorPaintMatShin);
      glRotatef(45, 1, 0, 0);
      glPushMatrix();
        glTranslatef(0.0, 8.0, -22.0);
        glScalef(30.0, 3.0, 10.0);
        glutSolidCube(1.0);
      glPopMatrix();
    glPopMatrix();
    glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, velourId);
      glEnable(GL_TEXTURE_2D);
      glTranslatef(0.0, 25.0, -10.0);
      glPushMatrix();
        glRotatef(45, 1.0, 0.0, 0.0);
        glPushMatrix();
          glRotatef(-turn * 10, 0.0, 1.0, 0.0);
          glmDraw(steeringWheel, GLM_SMOOTH|GLM_TEXTURE|GLM_MATERIAL);
        glPopMatrix();
      glPopMatrix();
      glDisable(GL_TEXTURE_2D);
    glPopMatrix();
  glPopMatrix();
}

// Display callback.
void display(void) {
  resetMats();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glEnable(GL_LIGHT0);
  if (horror == 0) {
    glFogf(GL_FOG_DENSITY, 0.00005);
  }
  else {
    glFogf(GL_FOG_DENSITY, 0.005);
  }

  if (win == 0) {
    if (view == 0) {
      gluLookAt(cameraX, cameraY, cameraZ,
                cameraX + sin(heading/RADIANS_TO_DEGREES) * 1000, cameraY,
                cameraZ - cos(heading/RADIANS_TO_DEGREES) * 1000, 0, 1, 0);
      }
      else {
        gluLookAt(cameraX - sin(heading/RADIANS_TO_DEGREES) * 200,
                  75.0,
                  cameraZ + cos(heading/RADIANS_TO_DEGREES) * 200,
                  cameraX + sin(heading/RADIANS_TO_DEGREES) * 1000, - 100.0,
                  cameraZ - cos(heading/RADIANS_TO_DEGREES) * 1000, 0, 1, 0);
      }
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
          glScalef(treeList[i].scaleX, treeList[i].scaleY, treeList[i].scaleZ);
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

  makeCar();

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

// Unused.
static void mouse(int button, int state, int x, int y) {
  NULL;
}

// Unused.
static void motion(int x, int y) {
  NULL;
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

  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_EXP);
  glFogf(GL_FOG_DENSITY, 0.00005);
  glFogfv(GL_FOG_COLOR, fogColor);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  loadTextures();
  loadModels();
  generateTrees(numTree);
  generateBikeMen(numBikeMen);
}

// Updates keyboard state when a key is released.
void keyboardUp(unsigned char key, int x, int y) {
  keyboard[key] = 0;
}

// Updates keyboard state when a key is pressed. Also handles the toggles.
void keyboardDown(unsigned char key, int x, int y) {
  keyboard[key] = 1;
  if (key == 'v') {
    if (view == 1) {
      view = 0;
    }
    else {
      view = 1;
    }
  }
  if (key == 'h') {
    if (horror == 0) {
      horror = 1;
    }
    else {
      horror = 0;
    }
  }
}

// Does the actions of the keys.
void keyboardCheck(void) {
  if (keyboard['q'] == 1) {
    delete tree;
    exit(0);
  }
  if (keyboard['w'] == 1) {
    if (speed<6.0) {
      speed = speed + 1.0;
    }
  }
  if (keyboard['a'] == 1) {
    if (turn>-4.0) {
      turn = turn - 1.0;
    }
  }
  if (keyboard['s'] == 1) {
    if (speed>-2.0) {
      speed = speed - 0.5;
    }
  }
  if (keyboard['d'] == 1) {
    if (turn<4.0) {
      turn = turn + 1.0;
    }
  }
}

// Reshape callback that preserves aspect ratio. Also implements the zoom.
void myReshape(int w, int h) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, w, h);
  gluPerspective(75.0f, (GLfloat)w/(GLfloat)h, 0.1, 4000.0);
  glMatrixMode(GL_MODELVIEW);
  ww = w;
 	wh = h;
}

// Idle callback. Drives the game for the most part.
void idle() {
  checkWin();
  keyboardCheck();

  if (wheelAngle > 360.0)
  		wheelAngle = wheelAngle - 360.0;
  	wheelAngle += 5;

  rightMan = invert(wheelAngle + 105);
  leftMan = invert(wheelAngle - 75);

  if (carWheelRotation > 360.0)
  		carWheelRotation = carWheelRotation - 360.0;
  if (carWheelRotation < 0.0)
  		carWheelRotation = carWheelRotation + 360.0;
  	carWheelRotation += speed;

  if (bikeAngle > 360.0)
  		bikeAngle = bikeAngle - 360.0;
  	bikeAngle += 1;

  if ((speed < 0.1) && (speed > -0.1)) {
    speed = 0;
  }
  else if ((speed > 0) && (keyboard['w'] == 0)) {
    speed = speed - 0.005;
  }
  else if ((speed < 0) && (keyboard['s'] == 0)) {
    speed = speed + 0.005;
  }

  if ((turn < 0.05) && (turn > -0.05)) {
    turn = 0;
  }
  else if ((turn > 0) && (keyboard['d'] == 0)) {
    turn = turn - 0.5;
  }
  else if ((turn < 0) && (keyboard['a'] == 0)) {
    turn = turn + 0.5;
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
    int check = checkTreeCollisions(cameraX, cameraZ, carWidth, carLength);
    if (check > -1) {
      cameraX = prevX;
      cameraZ = prevZ;
      if (treeList[check].fall < 1) {
        treeList[check].fall = 1;
        treeList[check].fallAngle = heading + 180;
      }
    }
    check = checkBikeCollisions(cameraX, cameraZ, carWidth, carLength);
    if (check > -1) {
      bikeList[check].dead = 1;
    }
    knockDownTrees();
    killBikeMen();
    if (cameraX > 980) {
      cameraX = 980;
    }
    if (cameraX < -980) {
      cameraX = -980;
    }
    if (cameraZ > 980) {
      cameraZ = 980;
    }
    if (cameraZ < -980) {
      cameraZ = -980;
    }
  }
  else {
    knockDownTrees();
    killBikeMen();
    if (treeList.size() > 0) {
      int i = rand()%treeList.size();
      int j = rand()%100;
      if ((treeList[i].fall < 1) && (j < 5)) {
        treeList[i].fall = 1;
        treeList[i].fallAngle = rand()%360;
      }
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

// Number of bike men callback.
void bike_menu(int id) {
  if (id == 1) numBikeMen = 1;
  else if (id == 2) numBikeMen = 3;
  else if (id == 3) numBikeMen = 5;
  else if (id == 4) numBikeMen = 10;
  else if (id == 5) numBikeMen = 20;
  reset();
}

// Number of trees callback.
void tree_menu(int id) {
  if (id == 1) numTree = 5;
  else if (id == 2) numTree = 25;
  else if (id == 3) numTree = 50;
  else if (id == 4) numTree = 100;
  else if (id == 5) numTree = 200;
  reset();
}

// Main Menu callback.
void right_menu(int id) {
  if (id == 1) reset();
}

int main(int argc, char **argv) {
  srand(time(NULL));
  // Initialize OpenGL.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);

  // Initial window size and position.
  glutInitWindowSize(1500,750);
  glutInitWindowPosition(300,100);

  // Name the window.
  glutCreateWindow("OpenGL Road Rage: 0");

  // Set callbacks.
  glutIgnoreKeyRepeat(1);
  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboardDown);
  glutKeyboardUpFunc(keyboardUp);
  glutReshapeFunc(myReshape);
  glutIdleFunc(idle);

  // Initialization function.
  init();

  // Menus
  int b_menu, t_menu, r_menu;
  b_menu = glutCreateMenu(bike_menu);
  glutAddMenuEntry("1", 1);
  glutAddMenuEntry("3", 2);
  glutAddMenuEntry("5", 3);
  glutAddMenuEntry("10", 4);
  glutAddMenuEntry("20", 5);
  t_menu = glutCreateMenu(tree_menu);
  glutAddMenuEntry("5", 1);
  glutAddMenuEntry("25", 2);
  glutAddMenuEntry("50", 3);
  glutAddMenuEntry("100", 4);
  glutAddMenuEntry("200", 5);
  r_menu = glutCreateMenu(right_menu);
  glutAddMenuEntry("Reset", 1);
  glutAddSubMenu("Number of Trees", t_menu);
  glutAddSubMenu("Number of Bike Men", b_menu);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  // Mainloop
  glutMainLoop();

  return 0;
}
