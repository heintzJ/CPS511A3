#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include "bullet.h"

// Quadric for glu objects
GLUquadric* bulletQuadric = gluNewQuadric();

// bullet material
GLfloat bullet_mat_ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat bullet_mat_specular[] = { 1.0f, 0.5f, 0.5f, 1.0f };
GLfloat bullet_mat_diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat bullet_mat_shininess[] = { 64.0F };

void Bullet::drawBullet()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, bullet_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, bullet_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bullet_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, bullet_mat_shininess);
	
	glPushMatrix();
	glTranslatef(xPos, yPos, zPos);
		// these transformations line the bullet up with the cannon
		glRotatef(90, 0.0f, 1.0f, 0.0f);
		glRotatef(-bulletYaw, 0.0f, 1.0f, 0.0f);
		glRotatef(-bulletPitch, 1.0f, 0.0f, 0.0f);

		gluCylinder(bulletQuadric, 0.5, 0.5, 2.0, 30.0, 30.0);
		gluDisk(bulletQuadric, 0, 0.5, 30, 1);
		glTranslatef(0, 0, 2.0);
		gluDisk(bulletQuadric, 0, 0.5, 30, 1);
	glPopMatrix();
}

void Bullet::position(float x, float y, float z)
{
	xPos = x;
	yPos = y;
	zPos = z;
}

void Bullet::scaleBullet(float x, float y, float z)
{
	scaleX = x;
	scaleY = y;
	scaleZ = z;
}

void Bullet::bulletVelocity(float x, float y, float z)
{
	vx = x;
	vy = y;
	vz = z;
}

void Bullet::moveBullet() 
{
	xPos += vx;
	yPos += vy;
	zPos += vz;
}

void Bullet::setBulletOrientation(float yaw, float pitch)
{
	bulletYaw = yaw;
	bulletPitch = pitch;
}