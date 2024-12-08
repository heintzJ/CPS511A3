#define STB_IMAGE_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <gl/glut.h>
#include "sackbot.h"
#include "stb_image.h"

bool cannonRotation = false;
bool walking = false;

// Note how everything depends on robot body dimensions so that can scale entire robot proportionately
// just by changing robot body scale
float robotBodyWidth = 3.0;
float robotBodyLength = 5.0;
float robotBodyDepth = 2.0;
float headWidth = 2 * robotBodyWidth;
float headLength = headWidth;
float headDepth = headWidth;
float upperArmLength = robotBodyLength;
float upperArmWidth = 0.333 * robotBodyWidth;
float lowerArmLength = 0.4 * upperArmLength;
float jointRadius = 0.5 * upperArmWidth;
float legRadius = 1.5 * jointRadius;

// Lighting/shading and material properties for robot
GLfloat robot_mat_ambient[] = { 0.4f, 0.3f, 0.2f, 1.0f };
GLfloat robot_mat_specular[] = { 0.6f, 0.4f, 0.2f, 1.0f };
GLfloat robot_mat_diffuse[] = { 0.7f, 0.5f, 0.3f, 1.0f };
GLfloat robot_mat_shininess[] = { 32.0F };

GLfloat robotJoint_mat_diffuse[] = { 0.4f, 0.2f, 0.1f, 1.0f };

GLfloat cannonBody_mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat cannonBody_mat_diffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat cannonBody_mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat cannonBody_mat_shininess[] = { 40.0f };

GLfloat cannonEnd_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat cannonEnd_mat_diffuse[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat cannonEnd_mat_specular[] = { 0.0f, 0.0f, 0.f, 1.0f };

// Quadric for glu objects
GLUquadric* quadric = gluNewQuadric();

// Control Robot body rotation on base
float robotAngle = 0.0;

// Control arm rotation
float shoulderAngle = 90, elbowAngle = 0;

// Control Cannon rotation
float cannonAngle = 0.0;

//Control leg angles of both thigh and calf
float leftThighAngle = 90, leftCalfAngle = 0, rightThighAngle = 90, rightCalfAngle = 0;

void Sackbot::drawRobot()
{
	glPushMatrix();

	glTranslatef(xPos, yPos, zPos);
	glScalef(scaleX, scaleY, scaleZ);

	if (isFalling) {
		// Apply fall rotation around X axis (falling forward)
		glRotatef(fallRotation, 1.0f, 0.0f, 0.0f);
	}

	drawBody();
	drawHead();
	drawLeftArm();
	drawRightArm();
	drawLeftLeg();
	drawRightLeg();
	if (isWalking) {
		walkMotion(); //used to get the bot walking
	}
	rotateCannon();

	glPopMatrix();
}


void Sackbot::drawBody()
{

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);
	// front
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-robotBodyWidth / 2, -robotBodyLength / 2, robotBodyDepth / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(robotBodyWidth / 2, -robotBodyLength / 2, robotBodyDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(robotBodyWidth / 2, robotBodyLength / 2, robotBodyDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-robotBodyWidth / 2, robotBodyLength / 2, robotBodyDepth / 2);

	// back
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-robotBodyWidth / 2, -robotBodyLength / 2, -robotBodyDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-robotBodyWidth / 2, robotBodyLength / 2, -robotBodyDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(robotBodyWidth / 2, robotBodyLength / 2, -robotBodyDepth / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(robotBodyWidth / 2, -robotBodyLength / 2, -robotBodyDepth / 2);

	// top
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-robotBodyWidth / 2, robotBodyLength / 2, -robotBodyDepth / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-robotBodyWidth / 2, robotBodyLength / 2, robotBodyDepth / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(robotBodyWidth / 2, robotBodyLength / 2, robotBodyDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(robotBodyWidth / 2, robotBodyLength / 2, -robotBodyDepth / 2);

	// bottom
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-robotBodyWidth / 2, -robotBodyLength / 2, -robotBodyDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(robotBodyWidth / 2, -robotBodyLength / 2, -robotBodyDepth / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(robotBodyWidth / 2, -robotBodyLength / 2, robotBodyDepth / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-robotBodyWidth / 2, -robotBodyLength / 2, robotBodyDepth / 2);

	// right
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(robotBodyWidth / 2, -robotBodyLength / 2, -robotBodyDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(robotBodyWidth / 2, robotBodyLength / 2, -robotBodyDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(robotBodyWidth / 2, robotBodyLength / 2, robotBodyDepth / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(robotBodyWidth / 2, -robotBodyLength / 2, robotBodyDepth / 2);

	// left
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-robotBodyWidth / 2, -robotBodyLength / 2, -robotBodyDepth / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-robotBodyWidth / 2, -robotBodyLength / 2, robotBodyDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-robotBodyWidth / 2, robotBodyLength / 2, robotBodyDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-robotBodyWidth / 2, robotBodyLength / 2, -robotBodyDepth / 2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Sackbot::drawHead()
{
	// Set robot material properties per body part. Can have seperate material properties for each part
	glMaterialfv(GL_FRONT, GL_AMBIENT, robot_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robot_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robot_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robot_mat_shininess);

	glPushMatrix();
	// Position head with respect to parent (body)
	glTranslatef(0, 0.5 * robotBodyLength + 0.5 * headLength, 0); // this will be done last

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);
	// front
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-headWidth / 2, -headLength / 2, headDepth / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(headWidth / 2, -headLength / 2, headDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(headWidth / 2, headLength / 2, headDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-headWidth / 2, headLength / 2, headDepth / 2);

	// back
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-headWidth / 2, -headLength / 2, -headDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-headWidth / 2, headLength / 2, -headDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(headWidth / 2, headLength / 2, -headDepth / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(headWidth / 2, -headLength / 2, -headDepth / 2);

	// top
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-headWidth / 2, headLength / 2, -headDepth / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-headWidth / 2, headLength / 2, headDepth / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(headWidth / 2, headLength / 2, headDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(headWidth / 2, headLength / 2, -headDepth / 2);

	// bottom
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-headWidth / 2, -headLength / 2, -headDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(headWidth / 2, -headLength / 2, -headDepth / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(headWidth / 2, -headLength / 2, headDepth / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-headWidth / 2, -headLength / 2, headDepth / 2);

	// right
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(headWidth / 2, -headLength / 2, -headDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(headWidth / 2, headLength / 2, -headDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(headWidth / 2, headLength / 2, headDepth / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(headWidth / 2, -headLength / 2, headDepth / 2);

	// left
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-headWidth / 2, -headLength / 2, -headDepth / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-headWidth / 2, -headLength / 2, headDepth / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-headWidth / 2, headLength / 2, headDepth / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-headWidth / 2, headLength / 2, -headDepth / 2);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// make the chin/beard thing
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotJoint_mat_diffuse);
	glPushMatrix();
	glTranslatef(-(0.5 * headWidth), -(0.5 * headLength), 0.5 * headDepth);
	glRotatef(90, 0, 1.0, 0);
	gluCylinder(quadric, 0.5, 0.5, headWidth, 30.0, 30.0);

	// make right side of beard (from robot perspective)
	glPushMatrix();
	glRotatef(95, 0, 0, 1.0);
	glRotatef(90, 1.0, 0, 0);
	gluCylinder(quadric, 0.5, 1, 0.5 * headWidth, 30.0, 30.0);
	// close the back of the cone
	glPushMatrix();
	glTranslatef(0, 0, 0.5 * headWidth);
	gluDisk(quadric, 0, 1, 30, 30);
	glPopMatrix();
	glPopMatrix();

	// make the right side circle of the beard
	glPushMatrix();
	glutSolidSphere(0.5, 30.0, 30.0);
	glPopMatrix();

	// right side screw
	glPushMatrix();
	glTranslatef(0.6 * headDepth, 1, headWidth);
	gluCylinder(quadric, 1, 1, 1, 30, 30);
	// cover side
	glTranslatef(0, 0, 1.0);
	gluDisk(quadric, 0, 1, 30, 30);
	glPopMatrix();

	// make the left side of beard
	glPushMatrix();
	glTranslatef(0, 0, headWidth);
	glRotatef(95, 0, 0, 1.0);
	glRotatef(90, 1.0, 0, 0);
	gluCylinder(quadric, 0.5, 1, 0.5 * headWidth, 30.0, 30.0);
	// close the back of the cone
	glPushMatrix();
	glTranslatef(0, 0, 0.5 * headWidth);
	gluDisk(quadric, 0, 1, 30, 30);
	glPopMatrix();
	glPopMatrix();

	// make the left side circle of the beard
	glPushMatrix();
	glTranslatef(0, 0, headWidth);
	glutSolidSphere(0.5, 30.0, 30.0);
	glPopMatrix();

	// left side screw
	glPushMatrix();
	glTranslatef(0.6 * headDepth, 1, -1);
	gluCylinder(quadric, 1, 1, 1, 30, 30);
	// cover side
	glRotatef(180, 0, 1.0, 0);
	gluDisk(quadric, 0, 1, 30, 30);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}

void Sackbot::drawLeftArm()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robot_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robot_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robot_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robot_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(0.5 * robotBodyWidth + 0.5 * upperArmWidth, 1, 0.0); // this will be done last


	// build upper arm
	glPushMatrix();
	glRotatef(shoulderAngle, 1.0, 0.5, 0.0); // Rotate to make it vertical
	gluCylinder(quadric, 0.8 * legRadius, 0.8 * legRadius, 0.4 * upperArmLength, 30, 1);
	gluDisk(quadric, 0, 0.8 * legRadius, 30, 1); // disk at the top
	glTranslatef(0, 0, 0.5 * upperArmLength); // move disk to the bottom
	gluDisk(quadric, 0, 0.8 * legRadius, 30, 1); // disk at bottom

	// build lower arm
	glPushMatrix();
	glRotatef(elbowAngle, 1.0, 0.0, 0.0); // Rotate to make it vertical
	gluCylinder(quadric, 0.8 * legRadius, 0.8 * legRadius, lowerArmLength, 30, 1);
	gluDisk(quadric, 0, 0.8 * legRadius, 30, 1); // top
	glTranslatef(0, 0, 0.5 * upperArmLength);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotJoint_mat_diffuse);
	glutSolidSphere(1.5 * jointRadius, 30, 30);
	glPopMatrix();

	// build joints
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotJoint_mat_diffuse);

	// shoulder joint
	glPushMatrix();
	glTranslatef(0, 0, -2.6);
	glutSolidSphere(jointRadius, 30.0, 30.0);
	glPopMatrix();

	// elbow joint
	glPushMatrix();
	glutSolidSphere(jointRadius, 30.0, 30.0);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}

void Sackbot::drawRightArm()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robot_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robot_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robot_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robot_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(-0.5 * robotBodyWidth + -0.5 * upperArmWidth, 1, 0); // this will be done last

	// build upper arm
	glPushMatrix();
	glRotatef(shoulderAngle - 40, 1.0, 0.0, 0.0);
	gluCylinder(quadric, 0.8 * legRadius, 0.8 * legRadius, 0.4 * upperArmLength, 30, 1);
	gluDisk(quadric, 0, 0.8 * legRadius, 30, 1); // disk at the top
	glTranslatef(0, 0, 0.5 * upperArmLength); // move disk to the bottom
	gluDisk(quadric, 0, 0.8 * legRadius, 30, 1); // disk at bottom

	// build lower arm
	glPushMatrix();
	glRotatef(elbowAngle - 40, 1.0, 0.0, 0.0);
	gluCylinder(quadric, 0.8 * legRadius, 0.8 * legRadius, lowerArmLength, 30, 1);
	gluDisk(quadric, 0, 0.8 * legRadius, 30, 1); // top
	glTranslatef(0, 0, 0.5 * upperArmLength);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotJoint_mat_diffuse);
	glutSolidSphere(1.5 * jointRadius, 30, 30);

	// build the cannon
	glMaterialfv(GL_FRONT, GL_AMBIENT, cannonBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, cannonBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cannonBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, cannonBody_mat_shininess);

	glPushMatrix();
	glTranslatef(-upperArmWidth, 0, -lowerArmLength);
	glRotatef(cannonAngle, 0, 0, 1.0);
	gluCylinder(quadric, 0.5 * upperArmWidth, 0.5 * upperArmWidth, 0.3 * upperArmLength, 30, 1);
	gluDisk(quadric, 0, 0.5 * upperArmWidth, 30, 1);
	glTranslatef(0, 0, 0.3 * upperArmLength);
	gluDisk(quadric, 0, 0.5 * upperArmWidth, 30, 1);

	// front part to cannon
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, cannonEnd_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, cannonEnd_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cannonEnd_mat_diffuse);
	glPushMatrix();
	glTranslatef(0.3, 0, 0);
	glutSolidCube(0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.3, 0, 0);
	glutSolidCube(0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0.3, 0);
	glutSolidCube(0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -0.3, 0);
	glutSolidCube(0.2);
	glPopMatrix();

	glPopMatrix();
	glPopMatrix();

	glPopMatrix();

	// build joints
	glMaterialfv(GL_FRONT, GL_AMBIENT, robot_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robot_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotJoint_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robot_mat_shininess);

	// shoulder joint
	glPushMatrix();
	glTranslatef(0, 0, -2.6);
	glutSolidSphere(jointRadius, 30.0, 30.0);
	glPopMatrix();

	// elbow joint
	glPushMatrix();
	glutSolidSphere(jointRadius, 30.0, 30.0);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}

void Sackbot::drawLeftLeg()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robot_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robot_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robot_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robot_mat_shininess);

	glPushMatrix();
	// Position leg with respect to parent body
	glTranslatef(0.5 * robotBodyWidth + 0.5 * jointRadius, -(0.5 * robotBodyLength) - 0.5 * jointRadius, 0);

	// build thigh
	glPushMatrix();
	glRotatef(leftThighAngle, 1.0, 0.0, 0.0); // Rotate to make it vertical
	gluCylinder(quadric, legRadius, legRadius, 0.5 * upperArmLength, 30, 1);
	gluDisk(quadric, 0, legRadius, 30, 1); // disk at the top
	glTranslatef(0, 0, 0.5 * upperArmLength); // move disk to the bottom
	gluDisk(quadric, 0, legRadius, 30, 1); // disk at bottom

	// build calf
	glPushMatrix();
	glTranslatef(0, 0, 0.5);
	glRotatef(leftCalfAngle, 1.0, 0.0, 0.0); // Rotate to make it vertical
	gluCylinder(quadric, legRadius, legRadius, 0.5 * upperArmLength, 30, 1);
	gluDisk(quadric, 0, legRadius, 30, 1); // top
	glTranslatef(0, 0, 0.5 * upperArmLength);
	gluDisk(quadric, 0, 1.5 * legRadius, 30, 1); // bottom
	glPopMatrix();

	// build joints
	glMaterialfv(GL_FRONT, GL_AMBIENT, robot_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robot_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotJoint_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robot_mat_shininess);

	// hip joint
	glPushMatrix();
	glTranslatef(0, 0, -2.6);
	glutSolidSphere(jointRadius, 30.0, 30.0);
	glPopMatrix();

	// knee joint
	glPushMatrix();
	glutSolidSphere(jointRadius, 30.0, 30.0);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}

void Sackbot::drawRightLeg()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robot_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robot_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robot_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robot_mat_shininess);

	glPushMatrix();
	// Position leg with respect to parent body
	glTranslatef(-(0.5 * robotBodyWidth + 0.5 * jointRadius), -(0.5 * robotBodyLength) - 0.5 * jointRadius, 0);

	// build thigh
	glPushMatrix();
	glRotatef(rightThighAngle, 1.0, 0.0, 0.0); // Rotate to make it vertical
	gluCylinder(quadric, legRadius, legRadius, 0.5 * upperArmLength, 30, 1);
	gluDisk(quadric, 0, legRadius, 30, 1); // disk at the top
	glTranslatef(0, 0, 0.5 * upperArmLength); // move disk to the bottom
	gluDisk(quadric, 0, legRadius, 30, 1); // disk at bottom

	// build calf
	glPushMatrix();
	glTranslatef(0, 0, 0.5);
	glRotatef(rightCalfAngle, 1.0, 0.0, 0.0); // Rotate to make it vertical
	gluCylinder(quadric, legRadius, legRadius, 0.5 * upperArmLength, 30, 1);
	gluDisk(quadric, 0, legRadius, 30, 1); // top
	glTranslatef(0, 0, 0.5 * upperArmLength);
	gluDisk(quadric, 0, 1.5 * legRadius, 30, 1); // bottom
	glPopMatrix();

	// build joints
	glMaterialfv(GL_FRONT, GL_AMBIENT, robot_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robot_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotJoint_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robot_mat_shininess);

	// hip joint
	glPushMatrix();
	glTranslatef(0, 0, -2.6);
	glutSolidSphere(jointRadius, 30.0, 30.0);
	glPopMatrix();

	// knee joint
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glutSolidSphere(jointRadius, 30.0, 30.0);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}

void Sackbot::position(float x, float y, float z)
{
	xPos = x;
	yPos = y;
	zPos = z;
}

void Sackbot::scaleRobot(float x, float y, float z)
{
	scaleX = x;
	scaleY = y;
	scaleZ = z;
}

void Sackbot::robotVelocity(float x, float y, float z)
{
	vx = x;
	vy = y;
	vz = z;
}

void Sackbot::move() {
	xPos += vx;
	yPos += vy;
	zPos += vz;
}

void Sackbot::rotateCannon()
{
	cannonAngle -= 5.0;
}

void Sackbot::walkMotion()
{
	rightThighAngle += 6.0 * direction;
	leftThighAngle -= 6.0 * direction;

	rightCalfAngle -= 3.0 * direction;
	if (rightCalfAngle < 0)
	{
		rightCalfAngle = 0;
	}
	else if (rightCalfAngle > 35)
	{
		rightCalfAngle = 35;
	}

	leftCalfAngle += 3.0 * direction;
	if (leftCalfAngle > 35)
	{
		leftCalfAngle = 35;
	}
	else if (leftCalfAngle < 0)
	{
		leftCalfAngle = 0;
	}

	if (rightThighAngle >= 150 || rightThighAngle <= 30)
	{
		direction *= -1;
	}

}

void Sackbot::loadTexture(const char* filename) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		printf("Failed to load texture: %s\n", filename);
	}

	stbi_image_free(data);
}
