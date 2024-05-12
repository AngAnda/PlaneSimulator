
	#include <Windows.h>
	#include <locale>
	#include <codecvt>

	#include <stdlib.h> // necesare pentru citirea shader-elor
	#include <stdio.h>
	#include <math.h> 

	#include <GL/glew.h>

	#include <GLM.hpp>
	#include <gtc/matrix_transform.hpp>
	#include <gtc/type_ptr.hpp>

	#include <glfw3.h>

	#include <iostream>
	#include <fstream>
	#include <sstream>
	#include "Shader.h"
	#include "Model.h"

	#pragma comment (lib, "glfw3dll.lib")
	#pragma comment (lib, "glew32.lib")
	#pragma comment (lib, "OpenGL32.lib")

	#include "stb_image.h"

	// settings
	const unsigned int SCR_WIDTH = 1800;
	const unsigned int SCR_HEIGHT = 1200;

	enum ECameraMovementType
	{
		UNKNOWN,
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		ROLL_LEFT,    // Roll the plane to the left
		ROLL_RIGHT,   // Roll the plane to the right
		PITCH_UP,     // Pitch the nose up
		PITCH_DOWN    // Pitch the nose down
	};


	float skyboxVertices[] =
	{
		//   Coordinates
		-1.0f, -1.0f,  1.0f,//        7--------6
		 1.0f, -1.0f,  1.0f,//       /|       /|
		 1.0f, -1.0f, -1.0f,//      4--------5 |
		-1.0f, -1.0f, -1.0f,//      | |      | |
		-1.0f,  1.0f,  1.0f,//      | 3------|-2
		 1.0f,  1.0f,  1.0f,//      |/       |/
		 1.0f,  1.0f, -1.0f,//      0--------1
		-1.0f,  1.0f, -1.0f
	};

	unsigned int skyboxIndices[] =
	{
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3
	};

	float planeVertices[] = {
	100.0f, -0.5f,  100.0f,  4.0f, 0.0f,   // Top Right of the plane
   -100.0f, -0.5f,  100.0f,  0.0f, 0.0f,   // Top Left of the plane
   -100.0f, -0.5f, -100.0f,  0.0f, 4.0f,   // Bottom Left of the plane

	100.0f, -0.5f,  100.0f,  4.0f, 0.0f,   // Top Right of the plane
   -100.0f, -0.5f, -100.0f,  0.0f, 4.0f,   // Bottom Left of the plane
	100.0f, -0.5f, -100.0f,  4.0f, 4.0f    // Bottom Right of the plane
	};


	std::vector<std::string> faces{
		"path/to/nx.jpg",
		"path/to/ny.jpg",
		"path/to/nz.jpg",
		"path/to/px.jpg",
		"path/to/py.jpg",
		"path/to/pz.jpg"
	};

	GLuint loadCubemap(std::vector<std::string> faces) {
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (GLuint i = 0; i < faces.size(); i++) {
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else {
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	unsigned int LoadSkybox(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		int width, height, nrChannels;
		for (unsigned int i{ 0 }; i < faces.size(); ++i)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		return textureID;
	}

	class Camera
	{
	private:
		// Default camera values
		const float zNEAR = 0.1f;
		const float zFAR = 500.f;
		const float YAW = -90.0f;
		const float PITCH = 0.0f;
		const float FOV = 45.0f;
		glm::vec3 startPosition;

	public:
		Camera(const int width, const int height, const glm::vec3& position)
		{
			startPosition = position;
			Set(width, height, position);
		}

		void Set(const int width, const int height, const glm::vec3& position)
		{
			this->isPerspective = true;
			this->yaw = YAW;
			this->pitch = PITCH;

			this->FoVy = FOV;
			this->width = width;
			this->height = height;
			this->zNear = zNEAR;
			this->zFar = zFAR;

			this->worldUp = glm::vec3(0, 1, 0);
			this->position = position;

			lastX = width / 2.0f;
			lastY = height / 2.0f;
			bFirstMouseMove = true;

			UpdateCameraVectors();
		}

		float GetRoll() const
		{
			return roll;
		}

		float GetPitch() const
		{
			return pitch;
		}

		float GetYaw() const
		{
			return yaw;
		}

		void SetPosition(const glm::vec3& pos)
		{
			position = pos;
		}


		void SetRoll(float r) { roll = r; }
		void SetPitch(float p) { pitch = p; }

		void Accelerate(float deltaTime) {
			speed += acceleration * deltaTime;
			if (speed > maxSpeed) speed = maxSpeed;
		}

		void Decelerate(float deltaTime) {
			speed -= acceleration * deltaTime;
			if (speed < minSpeed) speed = minSpeed;
		}

		float GetSpeed() const { return speed; }
		void SetSpeed(float s) { speed = s; }

		void Reset(const int width, const int height)
		{
			Set(width, height, startPosition);
		}

		void Reshape(int windowWidth, int windowHeight)
		{
			width = windowWidth;
			height = windowHeight;

			// define the viewport transformation
			glViewport(0, 0, windowWidth, windowHeight);
		}

		const glm::mat4 GetViewMatrix() const
		{
			// Returns the View Matrix
			return glm::lookAt(position, position + forward, up);
		}

		const glm::vec3 GetPosition() const
		{
			return position;
		}

		const glm::mat4 GetProjectionMatrix() const
		{
			glm::mat4 Proj = glm::mat4(1);
			if (isPerspective) {
				float aspectRatio = ((float)(width)) / height;
				Proj = glm::perspective(glm::radians(FoVy), aspectRatio, zNear, zFar);
			}
			else {
				float scaleFactor = 2000.f;
				Proj = glm::ortho<float>(
					-width / scaleFactor, width / scaleFactor,
					-height / scaleFactor, height / scaleFactor, -zFar, zFar);
			}
			return Proj;
		}

		void ProcessKeyboard(ECameraMovementType direction, float deltaTime) {
			float velocity = 20.5f * deltaTime;  // Exemplu de viteză de rotație

			switch (direction) {
			case FORWARD:
				StartFlying();
				Accelerate(deltaTime);
			//	position += forward * GetSpeed() * deltaTime;
				break;
			case BACKWARD:
				StopFlying();
				Decelerate(deltaTime);
			//	position -= forward * GetSpeed() * deltaTime;
				break;
			case LEFT:
				roll -= velocity * 10;  // Multiplicăm pentru a avea o rotație mai rapidă
				position -= right * velocity;
				break;
			case RIGHT:
				roll += velocity * 10;
				position += right * velocity;
				break;
			case UP:
				pitch += velocity * 10 * deltaTime; // Asigură-te că folosești deltaTime pentru a suaviza mișcarea
				break;
			case DOWN:
				pitch -= velocity * 10 * deltaTime;
				break;
			case ROLL_LEFT:
				roll -= velocity * 10;  // Multiplicăm pentru a avea o rotație mai rapidă
				break;
			case ROLL_RIGHT:
				roll += velocity * 10;
				break;
			case PITCH_UP:
				pitch += velocity * 10;
				break;
			case PITCH_DOWN:
				pitch -= velocity * 10;
				break;
			default:
				break;
			}

			position += forward * GetSpeed() * deltaTime * 10.0f;

		}


		void MouseControl(float xPos, float yPos)
		{
			if (bFirstMouseMove) {
				lastX = xPos;
				lastY = yPos;
				bFirstMouseMove = false;
			}

			float xChange = xPos - lastX;
			float yChange = lastY - yPos;
			lastX = xPos;
			lastY = yPos;

			if (fabs(xChange) <= 1e-6 && fabs(yChange) <= 1e-6) {
				return;
			}
			xChange *= mouseSensitivity;
			yChange *= mouseSensitivity;

			yaw += xChange;

			ProcessMouseMovement(xChange, yChange);
		}

		void ProcessMouseScroll(float yOffset)
		{
			if (FoVy >= 1.0f && FoVy <= 90.0f) {
				FoVy -= yOffset;
			}
			if (FoVy <= 1.0f)
				FoVy = 1.0f;
			if (FoVy >= 90.0f)
				FoVy = 90.0f;
		}

		glm::vec3 GetForward() const
		{
			return forward;
		}

		glm::vec3 GetUp() const { return up; }



		void StartFlying() {
			isFlying = true;
		}

		void UpdateFlight(float deltaTime) {
			if (isFlying) {
				speed += acceleration * deltaTime;
				if (speed > maxSpeed) speed = maxSpeed;

				// Modifică pitch-ul pentru a "ridica botul"
				pitch += pitchIncrement * deltaTime;
				if (pitch > 20.0f) pitch = 20.0f; // Limităm pitch-ul la 20 de grade pentru decolare
				glm::vec3 newPosition = position + forward * GetSpeed() * deltaTime * 10.0f;

				// Verifică dacă noua poziție este sub înălțimea minimă permisă
				if (newPosition.y < -0.5f) {
					newPosition.y = 0.0F; // Asigură-te că camera nu coboară sub plan
				}

				position = newPosition;
			}
		}

		void StopFlying() {
			isFlying = false;
			//speed = 0.0f;
		}

		//float GetSpeed() const { return speed; }
		bool IsFlying() const { return isFlying; }


	private:
		void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true)
		{
			yaw += xOffset;
			pitch += yOffset;

			//std::cout << "yaw = " << yaw << std::endl;
			//std::cout << "pitch = " << pitch << std::endl;

			// Avem grijã sã nu ne dãm peste cap
			if (constrainPitch) {
				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;
			}

			// Se modificã vectorii camerei pe baza unghiurilor Euler
			UpdateCameraVectors();
		}

		void UpdateCameraVectors()
		{
	
			//// Calculate the new forward vector
			this->forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			this->forward.y = sin(glm::radians(pitch));
			this->forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			this->forward = glm::normalize(this->forward);
			// Also re-calculate the Right and Up vector
			//right = glm::normalize(glm::cross(forward, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			//up = glm::normalize(glm::cross(right, forward));


			// Calculate new Right and Up vectors
			glm::vec3 newRight = glm::normalize(glm::cross(forward, worldUp));  // Adjust right vector
			up = glm::normalize(glm::cross(newRight, forward));  // Recompute up vector

			// Apply roll rotation around the forward axis
			glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), forward);
			//right = glm::vec3(rollMatrix * glm::vec4(newRight, 0.0f));
			//up = glm::vec3(rollMatrix * glm::vec4(up, 0.0f));
			right = glm::normalize(glm::cross(forward, worldUp));
			up = glm::normalize(glm::cross(right, forward));

		}

	protected:
		const float cameraSpeedFactor = 2.5f;
		//const float cameraSpeedFactor = 1.5f;
		const float mouseSensitivity = 0.1f;

		// Perspective properties
		float zNear;
		float zFar;
		float FoVy;
		int width;
		int height;
		bool isPerspective;
		float roll = 0.0f;  // Roll angle in degrees
		float speed = 0.2f;       // Viteza curentă a avionului
		float acceleration = 0.01f; // Accelerarea avionului
		float maxSpeed = 0.02f;     // Viteza maximă
		float minSpeed = 0.0f;     // Viteza minimă, poate fi zero pentru a opri complet
		bool isFlying = false;     // Dacă avionul este în aer sau nu
		float pitchIncrement = 0.1f;

		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
		glm::vec3 worldUp;

		// Euler Angles
		float yaw;
		float pitch;

		bool bFirstMouseMove = true;
		float lastX = 0.f, lastY = 0.f;
	};

	GLuint ProjMatrixLocation, ViewMatrixLocation, WorldMatrixLocation;
	Camera* pCamera = nullptr;

	void Cleanup()
	{
		delete pCamera;
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window);

	// timing
	double deltaTime = 0.0f;	// time between current frame and last frame
	double lastFrame = 0.0f;

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {

		}
	}

	int main()
	{
		// glfw: initialize and configure
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// glfw window creation
		GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 7", NULL, NULL);
		if (window == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetKeyCallback(window, key_callback);

		// tell GLFW to capture our mouse
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glewInit();

		glEnable(GL_DEPTH_TEST);

		// set up vertex data (and buffer(s)) and configure vertex attributes
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};
		// first, configure the cube's VAO (and VBO)
		unsigned int VBO, cubeVAO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
		unsigned int lightVAO;
		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// note that we update the lamp's position attribute's stride to reflect the updated buffer data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		// Încărcarea texturii de iarbă
		unsigned int grassTexture;
		glGenTextures(1, &grassTexture);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		// Setarea parametrilor texturii
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Încărcarea și generarea texturii
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true); // Pentru a asigura că textura nu este inversată
		unsigned char* data = stbi_load("resources/grass_texture.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

		// Crearea și configurarea bufferelor vertex și array pentru plan
		unsigned int planeVBO, planeVAO;
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);


		// Crearea și configurarea bufferelor vertex și array pentru tower
		unsigned int towerVBO, towerVAO;
		glGenVertexArrays(1, &towerVAO);
		glGenBuffers(1, &towerVBO);
		glBindVertexArray(towerVAO);
		glBindBuffer(GL_ARRAY_BUFFER, towerVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		// Crearea și configurarea bufferelor vertex și array pentru map
		unsigned int mapVBO, mapVAO;
		glGenVertexArrays(1, &mapVAO);
		glGenBuffers(1, &mapVBO);
		glBindVertexArray(mapVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mapVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/


		//GLuint skyboxVAO, skyboxVBO, skyboxEBO;
		// Create VAO, VBO, and EBO for the skybox
		unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glGenBuffers(1, &skyboxEBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Definirea și încărcarea shaderului pentru textura de iarbă
		//Shader textureShader("path/to/vertex_shader.vs", "path/to/fragment_shader.fs");

		glm::vec3 initialPosition(0.0f, 0.0f, 100.0f);

		// Create camera
		pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, initialPosition);

		glm::vec3 lightPos(0.0f, 2.0f, 1.0f);

		wchar_t buffer[MAX_PATH];
		GetCurrentDirectoryW(MAX_PATH, buffer);

		std::wstring executablePath(buffer);
		std::wstring wscurrentPath = executablePath.substr(0, executablePath.find_last_of(L"\\/"));

		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::string currentPath = converter.to_bytes(wscurrentPath);

		Shader lightingShader((currentPath + "\\Shaders\\PhongLight.vs").c_str(), (currentPath + "\\Shaders\\PhongLight.fs").c_str());
		Shader lampShader((currentPath + "\\Shaders\\Lamp.vs").c_str(), (currentPath + "\\Shaders\\Lamp.fs").c_str());
		Shader skyboxShader((currentPath + "\\PlaneSimulator\\skybox.vs").c_str(), (currentPath + "\\PlaneSimulator\\skybox.fs").c_str());

		// Load object model
		std::string objFileName = (currentPath + "\\Models\\CylinderProject.obj");
		Model objModel(objFileName, false);

		//Load skybox model 
		std::string skyBoxPath = currentPath + "\\Models\\skybox\\";

		std::vector<std::string> facesCubemap =
		{
			skyBoxPath + "px.png",
			skyBoxPath + "nx.png",
			skyBoxPath + "py.png",
			skyBoxPath + "ny.png",
			skyBoxPath + "pz.png",
			skyBoxPath + "nz.png"
		};

		unsigned int cubemapTexture = LoadSkybox(facesCubemap);

		// Load airplane model
		std::string airplaneObjFileName = (currentPath + "\\Models\\Airplane\\airplane.obj");
		Model airplaneObjModel(airplaneObjFileName, false);
		if (!airplaneObjModel.meshes.size()) {
			std::cerr << "Failed to load airplane model: " << airplaneObjFileName << std::endl;
		}

		// Load tower model
		std::string towerObjFileName = (currentPath + "\\Models\\Tower\\Tower_Control.obj");
		Model towerObjModel(towerObjFileName, false);
		if (!towerObjModel.meshes.size()) {
			std::cerr << "Failed to load tower model: " << towerObjFileName << std::endl;
		}

		// Load map model
		std::string mapObjFileName = (currentPath + "\\Models\\Map\\Map.obj");
		Model mapObjModel(mapObjFileName, false);
		if (!mapObjModel.meshes.size()) {
			std::cerr << "Failed to load map model: " << mapObjFileName << std::endl;
		}

		//// Load pirat model
		//std::string piratObjFileName = (currentPath + "\\Models\\Pirat\\Pirat.obj");
		//Model piratObjModel(piratObjFileName, false);

		// render loop
		while (!glfwWindowShouldClose(window)) {

			double currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			processInput(window);
			pCamera->UpdateFlight(deltaTime); // Actualizează zborul dacă este necesar

			if (pCamera->IsFlying()) {

				glm::vec3 newVector = pCamera->GetPosition()+ pCamera->GetForward() *  pCamera->GetSpeed();

				if(newVector.y < 0.0f)
					newVector.y = 0.0f;

				pCamera->SetPosition(newVector);
			}

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Camera transformations
			glm::vec3 cameraPosition = pCamera->GetPosition();
			glm::vec3 cameraForward = pCamera->GetForward();

			// Update light position over time
			//lightPos.x = 1.0 * cos(currentFrame);  // This can be adjusted
			//lightPos.z = 1.0 * sin(currentFrame);
			lightPos.x = 1.0;
			lightPos.y = 1.0;

			// Activate shader and set uniforms
			lightingShader.use();
			//glActiveTexture(GL_TEXTURE0); // Activate the texture unit first
			//glBindTexture(GL_TEXTURE_2D, textureID);
			//lightingShader.setInt("texture_diffuse1", 0); // This line sets the sampler uniform

			lightingShader.SetVec3("objectColor", 0.5f, 1.0f, 0.31f);
			lightingShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
			lightingShader.SetVec3("lightPos", lightPos);
			lightingShader.SetVec3("viewPos", pCamera->GetPosition());
			lightingShader.setMat4("projection", pCamera->GetProjectionMatrix());
			lightingShader.setMat4("view", pCamera->GetViewMatrix());


			glm::mat4 projection = pCamera->GetProjectionMatrix();
			glm::mat4 view = pCamera->GetViewMatrix();

			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// Render plane
			glBindVertexArray(planeVAO);
			glm::mat4 planeModel = glm::mat4(1.0);
			lightingShader.setMat4("model", planeModel);
			//glDrawArrays(GL_TRIANGLES, 0, 6);



			// render map
			glm::mat4 mapModel = glm::mat4(1.0f);
			mapModel = glm::translate(mapModel, initialPosition - glm::vec3(0, 120.0f, 0));
			mapModel = glm::scale(mapModel, glm::vec3(0.1f));
			lightingShader.setMat4("model", mapModel);
			mapObjModel.Draw(lightingShader);

			// Render tower model
			glm::mat4 towerModel = glm::mat4(1.0);
			towerModel = glm::translate(towerModel, initialPosition + glm::vec3(-2.0f, -2.0f, -15));
			towerModel = glm::scale(towerModel, glm::vec3(0.1f));
			lightingShader.setMat4("model", towerModel);
			towerObjModel.Draw(lightingShader);

			// Render airplane model
			glm::mat4 airplaneModel = glm::mat4(1.0);
			// Assume the functions GetRoll() and GetPitch() give us the current roll and pitch angles

			float rollAngle = pCamera->GetRoll();
			float pitchAngle = pCamera->GetPitch();
			float yawAngle = pCamera->GetYaw();
			//glm::vec3 cameraForward = pCamera->GetForward();
			glm::vec3 cameraUp = pCamera->GetUp();


			/*airplaneModel = glm::translate(airplaneModel, pCamera->GetPosition() + glm::vec3(0.0f, -0.3f, -1.0f));
			airplaneModel = glm::scale(airplaneModel, glm::vec3(0.0005f));
*/
			glm::vec3 airplanePosition = cameraPosition + cameraForward + glm::vec3(0.1f, 0.0f, 0.1f); // distanceFromCamera este distanța la care vrei să plasezi avionul în fața camerei
			airplaneModel = glm::translate(airplaneModel, airplanePosition);

			// Orientează avionul să fie perpendicular pe direcția de vizualizare a camerei
			glm::vec3 right = glm::normalize(glm::cross(cameraUp, cameraForward));
			glm::vec3 upward = glm::normalize(glm::cross(cameraForward, right));

			airplaneModel[0] = glm::vec4(-right, 0.0f);
			airplaneModel[1] = glm::vec4(upward, 0.0f);
			airplaneModel[2] = glm::vec4(-cameraForward, 0.0f); // Inversăm forward pentru a orienta avionul spre cameră
			airplaneModel = glm::scale(airplaneModel, glm::vec3(0.0005f));
			

			airplaneModel = glm::rotate(airplaneModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//airplaneModel = glm::rotate(airplaneModel, glm::radians(-rollAngle), glm::vec3(0.0f, 0.0f, 2.0f));
			airplaneModel = glm::rotate(airplaneModel, glm::radians(pCamera->GetPitch() + 10), glm::vec3(1.0f, 0.0f, 0.0f));
			//airplaneModel = glm::rotate(airplaneModel, glm::radians(pCamera->GetYaw()-270), glm::vec3(0.0f, 0.0f, -1.0f)); 
		
			//airplaneModel = glm::rotate(airplaneModel, glm::radians(pCamera->GetYaw()-90), glm::vec3(0.0f, 0.0f, -1.0f)); 

			lightingShader.setMat4("model", airplaneModel);
			airplaneObjModel.Draw(lightingShader);


			// Render lamp (light source visualization)
			lampShader.use();
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", view);
			glm::mat4 lightModel = glm::translate(glm::mat4(1.0), lightPos);
			lightModel = glm::scale(lightModel, glm::vec3(0.1f));  // Smaller cube for the light
			lampShader.setMat4("model", lightModel);

			//render skybox
			glDepthFunc(GL_LEQUAL);
			glUseProgram(skyboxShader.ID);
			projection = pCamera->GetProjectionMatrix();
			view = glm::mat4(glm::mat3(pCamera->GetViewMatrix()));
			skyboxShader.setMat4("projection", projection);
			skyboxShader.setMat4("view", view);




			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS);


			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		Cleanup();

		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteVertexArrays(1, &lightVAO);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &skyboxVAO);
		glDeleteBuffers(1, &skyboxVBO);

		// glfw: terminate, clearing all previously allocated GLFW resources
		glfwTerminate();
		return 0;
	}

	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			pCamera->ProcessKeyboard(ROLL_LEFT, (float)deltaTime);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			pCamera->ProcessKeyboard(ROLL_RIGHT, (float)deltaTime);
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		pCamera->Reshape(width, height);
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		pCamera->MouseControl((float)xpos, (float)ypos);
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
	{
		pCamera->ProcessMouseScroll((float)yOffset);
	}


