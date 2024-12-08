class Sackbot {
public:
	void drawRobot();
	void drawBody();
	void drawHead();
	void drawLeftArm();
	void drawRightArm();
	void drawLeftLeg();
	void drawRightLeg();
	void walkMotion();
	void rotateCannon();
	void position(float xPos, float yPos, float zPos);
	void scaleRobot(float scaleX, float scaleY, float scaleZ);
	void robotVelocity(float vx, float vy, float vz);
	void loadTexture(const char* filename);
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
		if (timeSinceLastShot >= shootInterval && !isFalling) {
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

	bool isFallComplete() const { 
		return fallComplete; 
	}

	void fallAnimation() {
		isFalling = true;
		fallRotation = 0.0f;
		fallComplete = false;
	}

	void stopWalkAnimation() {
		isWalking = false;
	}

	bool isCurrentlyFalling() const {
		return isFalling;
	}

	void updateFallAnimation() {
		if (isFalling) {
			fallRotation += fallSpeed;
			yPos -= 0.03 * fallSpeed;
			if (fallRotation >= 90.0f) {
				fallComplete = true;
			}
		}
	}

private:
	float xPos, yPos, zPos;
	float scaleX, scaleY, scaleZ;
	float vx, vy, vz;
	float timeSinceLastShot = 0.0f; //time tracker for shooting
	float shootInterval = 2.5f;     //interval at which this sackbot shoots
	bool isFalling = false;
	bool fallComplete = false;
	bool isWalking = true;
	float fallRotation = 0.0f;
	float fallSpeed = 2.0f;
	GLuint texture;
	
	// Control Robot body rotation on base
	float robotAngle = 0.0;

	// Control arm rotation
	float shoulderAngle = 90, elbowAngle = 0;

	// Control Cannon rotation
	float cannonAngle = 0.0;

	//Control leg angles of both thigh and calf
	float leftThighAngle = 90, leftCalfAngle = 0, rightThighAngle = 90, rightCalfAngle = 0;

	int direction = 1;

	float cannonRotation = -2.0f;
};
