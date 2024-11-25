class Sackbot {
public:
	void drawRobot();
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

private:
	float xPos, yPos, zPos;
	float scaleX, scaleY, scaleZ;
	float vx, vy, vz;
};