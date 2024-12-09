#include <GL/glew.h>
#include <GL/freeglut.h>

typedef struct Vector2D
{
	GLdouble x, y;
	GLdouble nx, ny;
	GLdouble u; // running distance
} Vector2D;

typedef struct Vector3D
{
	GLdouble x, y, z;
} Vector3D;

// Function prototypes
void mouseMotionHandler2D(int x, int y);
void worldToCameraCoordinates(GLdouble xWorld, GLdouble yWorld, GLdouble* xcam, GLdouble* ycam);
void screenToCameraCoordinates(int xScreen, int yScreen, GLdouble* xCamera, GLdouble* yCamera);
void cameraToWorldCoordinates(GLdouble xcam, GLdouble ycam, GLdouble* xw, GLdouble* yw);
void screenToWorldCoordinates(int xScreen, int yScreen, GLdouble* xw, GLdouble* yw);

// 3D Functions
void display3D();
void reshape3D(int w, int h);
void mouseButtonHandler3D(int button, int state, int x, int y);
void keyboardHandler3D(unsigned char key, int x, int y);

// Vector operations
Vector3D normalize(Vector3D a);
Vector3D add3DVectors(Vector3D a, Vector3D b);
Vector3D subtract3DVectors(Vector3D a, Vector3D b);
float magnitude(Vector3D a);

void drawGround();