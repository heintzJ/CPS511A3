class Sackbot {
public:
	void drawRobot();
	void position(float xPos, float yPos, float zPos);
	void scaleRobot(float scaleX, float scaleY, float scaleZ);
	void robotVelocity(float vx, float vy, float vz);

private:
	float xPos, yPos, zPos;
	float scaleX, scaleY, scaleZ;
	float vx, vy, vz;
};