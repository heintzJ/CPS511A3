#include <stdio.h>
#include <windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <string.h>
#include "surfaceModeller.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "sackbot.h"
#include "bullet.h"
#include <random>

#define M_PI 3.14159265358979323846

std::vector<Sackbot> sackbots;
std::vector<Bullet> bullets;

// for some reason I can't put these in the header file
Vector3D add3DVectors(Vector3D a, Vector3D b);
Vector3D subtract3DVectors(Vector3D a, Vector3D b);
float magnitude(Vector3D a);
bool collided(Sackbot& sackbot, Bullet& bullet);

GLdouble worldLeft = -12;
GLdouble worldRight = 12;
GLdouble worldBottom = -9;
GLdouble worldTop = 9;
GLdouble worldCenterX = 0.0;
GLdouble worldCenterY = 0.0;
GLdouble wvLeft = -12;
GLdouble wvRight = 12;
GLdouble wvBottom = -9;
GLdouble wvTop = 9;

GLint glutWindowWidth = 800;
GLint glutWindowHeight = 600;
GLint viewportWidth = glutWindowWidth;
GLint viewportHeight = glutWindowHeight;

// screen window identifiers
int window3D;
int windowWidth = 800, windowHeight = 600;
GLdouble aspect = (GLdouble)windowWidth / windowHeight;

// Ground Mesh material
GLfloat groundMat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat groundMat_specular[] = { 0.01, 0.01, 0.01, 1.0 };
GLfloat groundMat_diffuse[] = { 0.4, 0.4, 0.7, 1.0 };
GLfloat groundMat_shininess[] = { 1.0 };

GLfloat light_position0[] = { 4.0, 8.0, 8.0, 1.0 };
GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light_position1[] = { -4.0, 8.0, 8.0, 1.0 };
GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

//
// Surface of Revolution consists of vertices and quads
//
// Set up lighting/shading and material properties for surface of revolution
GLfloat quadMat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat quadMat_specular[] = { 0.45, 0.55, 0.45, 1.0 };
GLfloat quadMat_diffuse[] = { 0.1, 0.35, 0.1, 1.0 };
GLfloat quadMat_shininess[] = { 10.0 };

// Quads and Vertices of the surface of revolution
typedef struct Vertex
{
	GLdouble x, y, z;
	Vector3D normal;
} Vertex;

class Mesh {
public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;

		setupMesh();
	}
	void setupMesh()
	{
		glGenVertexArrays(1, &VAOid);
		glGenBuffers(1, &VBOid);
		glGenBuffers(1, &IBOid);

		glBindVertexArray(VAOid);

		// vertice buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBOid);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		// index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOid);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (void*)0);

		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Render method
	void Draw() {
		glBindVertexArray(VAOid);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
private:
	unsigned int VAOid, VBOid, IBOid;
};

unsigned int VAOid;
unsigned int VBOid;
unsigned int IBOid;

bool firstMouse = true;

GLdouble fov = 80.0;

int lastMouseX;
int lastMouseY;

GLdouble eyeX = 0.0, eyeY = 0.0, eyeZ = 30.0;
GLdouble radius = eyeZ;
GLdouble zNear = 0.1, zFar = 60.0;
float cameraYaw = -90.0f;
float cameraPitch = 0.0f;

Vector3D cameraPos = { eyeX, eyeY, eyeZ };
Vector3D cameraFront = { 0.0, 0.0, -1.0 };
Vector3D cameraUp = { 0.0, 1.0, 0.0 };

float cannonX = 0.0f;
float cannonY = -2.0f;
float cannonZ = 27.0f;

Assimp::Importer importer;

void loadModel();
Mesh* loadedMesh = nullptr;

void init3DSurfaceWindow()
{
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear

	glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void reshape3D(int w, int h)
{
	glutWindowWidth = (GLsizei)w;
	glutWindowHeight = (GLsizei)h;
	glViewport(0, 0, glutWindowWidth, glutWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void display3D()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// cameraFront is the direction the camera is facing
	// adding that to the camera position gives us the location to look at
	Vector3D lookAt = add3DVectors(cameraPos, cameraFront);
	gluLookAt(
		cameraPos.x, cameraPos.y, cameraPos.z,
		lookAt.x, lookAt.y, lookAt.z,
		cameraUp.x, cameraUp.y, cameraUp.z
	);

	drawGround();

	glPushMatrix();
	glTranslatef(cannonX, cannonY, cannonZ);
	glScalef(0.66f, 0.66f, 0.66f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glRotatef(cameraYaw, 0.0f, 1.0f, 0.0f);
	glRotatef(cameraPitch, 1.0f, 0.0f, 0.0f);
	glRotatef(-70, 1.0f, 0.0f, 0.0f);
	
	loadModel();
	loadedMesh->Draw();
	glPopMatrix();

	for (auto& sackbot : sackbots)
	{
		sackbot.drawRobot();
	}
	for (auto& bullet : bullets)
	{
		bullet.drawBullet();
	}

	glutSwapBuffers();
}

void drawGround()
{
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-40.0f, -4.0f, -30.0f);
	glVertex3f(-22.0f, -4.0f, 30.0f);
	glVertex3f(40.0f, -4.0f, 30.0f);
	glVertex3f(22.0f, -4.0f, -30.0f);
	glEnd();
	glPopMatrix();
}

void loadModel()
{
	const aiScene* scene = importer.ReadFile("./ExportedMeshes/object.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	
	// load the mesh. We only have one mesh and it is in index 0
	aiMesh* mesh = scene->mMeshes[0];

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// get data for vertices and normals
	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		Vector3D vector;
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		vertices.push_back(vertex);
	}
	
	// get index data
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) { 
			indices.push_back(face.mIndices[j]);
		}
	}

	// get material data
	if (mesh->mMaterialIndex >= 0 && scene->mMaterials) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor3D ambient(0.0f, 0.0f, 0.0f);
		aiColor3D diffuse(0.0f, 0.0f, 0.0f);
		aiColor3D specular(0.0f, 0.0f, 0.0f);
		float shininess;

		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		material->Get(AI_MATKEY_SHININESS, shininess);

		GLfloat objectMat_ambient[] = { ambient.r, ambient.g, ambient.b, 1.0f };
		GLfloat objectMat_diffuse[] = { diffuse.r, diffuse.g, diffuse.b, 1.0f };
		GLfloat objectMat_specular[] = { specular.r, specular.g, specular.b, 1.0f };
		GLfloat objectMat_shininess[] = { shininess };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, objectMat_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, objectMat_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, objectMat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, objectMat_shininess);
	}

	loadedMesh = new Mesh(vertices, indices);
}

// this will initialize the variables needed by a sackbot object
// we then use these variables in display3D() to render the sackbot
void spawnSackbot()
{
	Sackbot sackbot;
	// get a random number between -10 and 10
	std::random_device seed;
	std::mt19937 gen{ seed() };
	std::uniform_int_distribution<> dist{ -10,10 };
	// spawn the robot somewhere on the x axis and 10 units away
	float x = dist(gen);
	float y = 0.0f;
	float z = -30.0f; // edge of ground
	sackbot.position(x, y, z);
	sackbot.robotVelocity(0.0f, 0.0f, 0.1f);
	sackbot.scaleRobot(0.5f, 0.5f, 0.5f);
	sackbots.push_back(sackbot);
	std::cout << "Spawned sackbot at (" << x << " " << y << " " << z << ")" << std::endl;

}

// this takes a sackbot from the sackbots vector and gives it the 
float spawnInterval = 2.0f;
float timeSinceLastSpawn = 0.0f;
float deltaTime = 0.016f;
void sackbotHandler()
{
	timeSinceLastSpawn += deltaTime;
	
	// if the timer interval is enough, spawn a new sackbot
	if (timeSinceLastSpawn >= spawnInterval) {
		spawnSackbot();
		timeSinceLastSpawn = 0.0f;
	}

	// always update the positions of current sackbots
	for (auto& sackbot : sackbots)
	{
		sackbot.move();
	}

	// remove robots that pass the camera
	sackbots.erase(
		std::remove_if(sackbots.begin(), sackbots.end(), [](Sackbot& sackbot) {
			return sackbot.currentZ() > 20.0f;
			}),
		sackbots.end()
	);

	glutPostRedisplay();
}

void bulletHandler()
{
	// update position of bullets
	for (auto& bullet : bullets) {
		bullet.moveBullet();
	}

	// remove a bullet if it gets too far away
	bullets.erase(
		std::remove_if(bullets.begin(), bullets.end(), [](Bullet& bullet) {
			return bullet.currentZ() < -50.0f;
			}),
		bullets.end()
	);

	glutPostRedisplay();
}

// create a bullet object and put it in the bullets vector
void createBullet()
{
	Bullet bullet;
	float bulletSpeed = 1.0f;

	// calculate the direction vector
	float dirX = cos(cameraYaw / 180.0f * M_PI) * cos(cameraPitch / 180.0f * M_PI);
	float dirY = sin(cameraPitch / 180.0f * M_PI);
	float dirZ = sin(cameraYaw / 180.0f * M_PI) * cos(cameraPitch / 180.0f * M_PI);

	float offset = 2.0f;

	// set the bullet's initial position at the end of the cannon
	float x = cannonX + dirX * offset;
	float y = cannonY + dirY * offset;
	float z = cannonZ + dirZ * offset;

	bullet.position(x, y, z);

	// set the bullet's velocity based on the cannon's direction
	bullet.bulletVelocity(
		dirX * bulletSpeed,
		dirY * bulletSpeed,
		dirZ * bulletSpeed
	);
	// this will angle the bullet correctly when it is fired
	bullet.setBulletOrientation(cameraYaw, cameraPitch);

	bullets.push_back(bullet);

	std::cout << "Bullet created:" << std::endl;
	std::cout << "  Yaw: " << cameraYaw << ", Pitch: " << cameraPitch << std::endl;
	std::cout << "  Position: " << x << ", " << y << ", " << z << std::endl;
	std::cout << "  Direction: " << dirX << ", " << dirY << ", " << dirZ << std::endl;
	std::cout << "  Velocity: "
		<< dirX * bulletSpeed << ", "
		<< dirY * bulletSpeed << ", "
		<< dirZ * bulletSpeed << std::endl;
}

//------------------------
//
// 
//will need more work and testing
void createEnemyBullet(Sackbot& bot)
{
	//used to generate a random yaw and pitch value
	std::random_device seed;
	std::mt19937 gen{ seed() };
	std::uniform_int_distribution<> yaw{ -130,-50 }; //used to get a random yaw value
	std::uniform_int_distribution<> pitch{ 0,50 }; //used to get a random pitch value
	size_t i = 1;

	//initialize bullet
	Bullet bullet;
	float bulletSpeed = 1.0f;

	float offset = 2.0f;

	//get current coordinate values of the current sackbot
	float botX = bot.currentX();
	float botY = bot.currentY();
	float botZ = bot.currentZ();

	//if i mod 5 is not 0 it will shoot a random direction, else it will shoot at cannon directly
	if (i % 5 != 0)
	{
		//generate random angles
		float randYaw = yaw(gen);
		float randPitch = pitch(gen);

		//create direction vector with these random angles
		float dirX = cos(randYaw / 180.0f * M_PI) * cos(randPitch / 180.0f * M_PI);
		float dirY = sin(randPitch / 180.0f * M_PI);
		float dirZ = sin(randYaw / 180.0f * M_PI) * cos(randPitch / 180.0f * M_PI);

		//set the bullet's initial position at the position of the current bot
		float x = botX + dirX * offset;
		float y = botY + dirY * offset;
		float z = botZ + dirZ * offset;
		bullet.position(x, y, z);

		//set the bullet's velocity to aim at the cannon
		bullet.bulletVelocity(dirX * bulletSpeed, dirY * bulletSpeed, dirZ * bulletSpeed);

		//set the bullet's orientation to the random agnles
		bullet.setBulletOrientation(randYaw, randPitch);

		//add the bullet to the vector
		bullets.push_back(bullet);

		i++;
	}
	else
	{
		//calculate direction to cannon
		float dirX = cannonX - botX;
		float dirY = cannonY - botY;
		float dirZ = cannonZ - botZ;

		//normalize the direction vector (did not use function to better access the variables)
		float magnitude = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
		dirX /= magnitude;
		dirY /= magnitude;
		dirZ /= magnitude;

		//set the bullet's initial position at the position of the current sackbot
		float x = botX + dirX * offset;
		float y = botY + dirY * offset;
		float z = botZ + dirZ * offset;
		bullet.position(x, y, z);

		//set the bullet's velocity to aim at the cannon
		bullet.bulletVelocity(dirX * bulletSpeed, dirY * bulletSpeed, dirZ * bulletSpeed);

		//add the bullet to the vector
		bullets.push_back(bullet);

		i++;
	}

	
}

bool collided(Sackbot& sackbot, Bullet& bullet) 
{
	// if the magnitude of the distance vector between the sackbot and bullet is < 1, return true
	// have to make the sackbot hitbox larger, so we can hit the head
	Vector3D sackbotPos = { sackbot.currentX(), sackbot.currentY(), sackbot.currentZ()};
	Vector3D bulletPos = { bullet.currentX(), bullet.currentY(), bullet.currentZ() };
	float distance = magnitude(subtract3DVectors(bulletPos, sackbotPos));
	return distance < 3.0f;
}

//------------------------
// 
// 
//will need testing
bool collidedCannon(Bullet& bullet)
{
	//if the magnitude of the distance vector between the cannon and enemy bullet is < 1, return true
	Vector3D cannonPos = { cannonX, cannonY, cannonZ };
	Vector3D bulletPos = { bullet.currentX(), bullet.currentY(), bullet.currentZ() };

	float dist = magnitude(subtract3DVectors(bulletPos, cannonPos));
	return dist < 3.0f;
	
}

// spawn a sack bot every 16ms
void gameLoop(int value) {
	sackbotHandler();
	bulletHandler();

	//----------------------
	// 
	// 
	//udate each sackbot's shooting timer and trigger shooting (will need testing)
	for (auto& sackbot : sackbots) {
		sackbot.updateTimer(deltaTime);

		if (sackbot.shouldShoot()) {
			createEnemyBullet(sackbot); //pass the sackbot to the shooting function
		}
	}

	// always check for colliding bullets and sackbots
	for (size_t i = 0; i < sackbots.size(); ) {
        bool removed = false;
        for (size_t j = 0; j < bullets.size(); ) {
            if (collided(sackbots[i], bullets[j])) {
                sackbots.erase(sackbots.begin() + i);
                bullets.erase(bullets.begin() + j);
                removed = true;
                break;
            } else {
                ++j;
            }
        }
        
        if (!removed) {
            ++i;
        }
    }

	//------------------------
	// 
	// 
	//check for bullets collding with cannon (will need testing)
	for (size_t i; i < bullets.size(); i++)
	{

		if (collidedCannon(bullets[i]))
		{
			bullets.erase(bullets.begin() + i);
		}
		else
		{
			i++;
		}
	}

	glutTimerFunc(16, gameLoop, 0);
}

void screenToWorldCoordinates(int xScreen, int yScreen, GLdouble* xw, GLdouble* yw)
{
	GLdouble xView, yView;
	screenToCameraCoordinates(xScreen, yScreen, &xView, &yView);
	cameraToWorldCoordinates(xView, yView, xw, yw);
}

void screenToCameraCoordinates(int xScreen, int yScreen, GLdouble* xCamera, GLdouble* yCamera)
{
	*xCamera = ((wvRight - wvLeft) / glutWindowWidth) * xScreen;
	*yCamera = ((wvTop - wvBottom) / glutWindowHeight) * (glutWindowHeight - yScreen);
}

void cameraToWorldCoordinates(GLdouble xcam, GLdouble ycam, GLdouble* xw, GLdouble* yw)
{
	*xw = xcam + wvLeft;
	*yw = ycam + wvBottom;
}

void worldToCameraCoordinates(GLdouble xWorld, GLdouble yWorld, GLdouble* xcam, GLdouble* ycam)
{
	double wvCenterX = wvLeft + (wvRight - wvLeft) / 2.0;
	double wvCenterY = wvBottom + (wvTop - wvBottom) / 2.0;
	*xcam = worldCenterX - wvCenterX + xWorld;
	*ycam = worldCenterY - wvCenterY + yWorld;
}

Vector3D add3DVectors(Vector3D a, Vector3D b)
{
	Vector3D result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

Vector3D subtract3DVectors(Vector3D a, Vector3D b)
{
	Vector3D result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

float magnitude(Vector3D a)
{
	return sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2));
}

Vector3D normalize(Vector3D a)
{
	GLdouble norm = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	Vector3D normalized;
	normalized.x = a.x / norm;
	normalized.y = a.y / norm;
	normalized.z = a.z / norm;
	return normalized;
}
void mouseMotionHandler2D(int xMouse, int yMouse)
{
	if (firstMouse)
	{
		lastMouseX = xMouse;
		lastMouseY = yMouse;
		firstMouse = false;
	}
	float sensitivity = 0.1f;

	// limit viewing angles
	if (cameraPitch > 50.0f) {
		cameraPitch = 50.0f;
	}
	if (cameraPitch < 0.0f) {
		cameraPitch = 0.0f;
	}
	if (cameraYaw > -50.0f) {
		cameraYaw = -50.0f;
	}
	if (cameraYaw < -130.0f) {
		cameraYaw = -130.0f;
	}
	
	cameraYaw += (xMouse - lastMouseX) * sensitivity;
	cameraPitch += (lastMouseY - yMouse) * sensitivity;

	lastMouseX = xMouse;
	lastMouseY = yMouse;
	
	Vector3D direction;
	direction.x = (cos(cameraYaw / 180.0f * M_PI)) * (cos(cameraPitch / 180.0f * M_PI));
	direction.y = sin(cameraPitch / 180.0f * M_PI);
	direction.z = (sin(cameraYaw / 180.0f * M_PI)) * (cos(cameraPitch / 180.0f * M_PI));
	cameraFront = normalize(direction);
	glutPostRedisplay();
}

void keyboardHandler3D(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
		exit(0);
		break;
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

int currentButton;

void mouseButtonHandler3D(int button, int state, int x, int y)
{
	currentButton = button;
	lastMouseX = x;
	lastMouseY = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			createBullet();
			glutPostRedisplay();
		}
		break;
	default:
		break;
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(glutWindowWidth, glutWindowHeight);
	//glutInitWindowPosition(50, 100);

	// The 3D Window
	window3D = glutCreateWindow("Sackbot Attack");
	//glutPositionWindow(900, 100);
	glutFullScreen();
	glewInit();
	glutDisplayFunc(display3D);
	glutReshapeFunc(reshape3D);
	glutKeyboardFunc(keyboardHandler3D);
	glutPassiveMotionFunc(mouseMotionHandler2D);
	glutMouseFunc(mouseButtonHandler3D);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(400, 300);
	// Initialize the 3D system
	init3DSurfaceWindow();
	
	glutTimerFunc(16, gameLoop, 0);
	// Annnd... ACTION!!
	glutMainLoop();

	return 0;
}
