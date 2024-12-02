class Sackbot {
public:
	void drawRobot();
	void drawLeftArm();
	void drawRightArm();
	void drawLeftLeg();
	void drawRightLeg();
	void walkMotion();
	void position(float xPos, float yPos, float zPos);
	void scaleRobot(float scaleX, float scaleY, float scaleZ);
	void robotVelocity(float vx, float vy, float vz);
	void move();
	float currentX() const {
		return xPos;
	}
	float currentY() const {
		return yPos;
	}
	float currentZ() const {
		return zPos;
	}

	//update the timer of when to shoot
	void updateTimer(float deltaTime) {
		timeSinceLastShot += deltaTime;
	}

	//check if the sackbot should shoot
	bool shouldShoot() {
		if (timeSinceLastShot >= shootInterval) {
			timeSinceLastShot = 0.0f; //reset the timer
			return true;
		}
		return false;
	}
	float getTimeSinceLastShot() const {
		return timeSinceLastShot;
	}

	float getShootInterval() const {
		return shootInterval;
	}

private:
	float xPos, yPos, zPos;
	float scaleX, scaleY, scaleZ;
	float vx, vy, vz;
	float timeSinceLastShot = 0.0f; //time tracker for shooting
	float shootInterval = 2.5f;     //interval at which this sackbot shoots
	
	// Control Robot body rotation on base
	float robotAngle = 0.0;

	// Control arm rotation
	float shoulderAngle = 90, elbowAngle = 0;

	// Control Cannon rotation
	float cannonAngle = 0.0;

	//Control leg angles of both thigh and calf
	float leftThighAngle = 90, leftCalfAngle = 0, rightThighAngle = 90, rightCalfAngle = 0;

	int direction = 1;
};
