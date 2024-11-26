class Bullet {
public:
	void drawBullet();
	void position(float xPos, float yPos, float zPos);
	void scaleBullet(float scaleX, float scaleY, float scaleZ);
	void bulletVelocity(float vx, float vy, float vz);
	void moveBullet();
	void setBulletOrientation(float yaw, float pitch);
	float getBulletYaw() const 
	{ 
		return bulletYaw; 
	}
	float getBulletPitch() const { 
		return bulletPitch; 
	}
	float currentX() const {
		return xPos;
	}
	float currentY() const {
		return yPos;
	}
	float currentZ() const {
		return zPos;
	}

private:
	float xPos, yPos, zPos;
	float scaleX, scaleY, scaleZ;
	float vx, vy, vz;
	float bulletYaw, bulletPitch;
};