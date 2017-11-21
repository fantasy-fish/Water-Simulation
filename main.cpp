#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "camera.h"
#include "shader_s.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define N (128)
#define NN (N * N)
#define NUM_TRIANGLE ((N-1)*(N-1) * 2)

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const* path);
unsigned int loadTextureFromArray(float* arr);
inline int ARRAY(int i, int j) { return j* N + i; }
inline void printVec4(glm::vec4 v);

//camera

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseClick = false;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
bool lbutton_down = false;
bool mouse_is_dragging = false;

glm::mat4 waterModel = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// begin init here
	Shader ourShader("basic_shader.vs", "basic_shader.fs");
	Shader waterShader("water.vs", "water.fs");

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f, 0.0f, -1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f, 0.0f, -1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f, 0.0f, -1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	texture1 = loadTexture("wall.jpg");
	texture2 = loadTexture("brickwall.jpg");

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// init water vert

	float field_vertices[NN * 5];
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int dx = j * N + i;
			int tri_idx = dx * 5;
			field_vertices[tri_idx] = (float)i;
			field_vertices[tri_idx + 1] = 0.0f;
			field_vertices[tri_idx + 2] = (float)j;
			field_vertices[tri_idx + 3] = (float)i / (float)N;
			field_vertices[tri_idx + 4] = (float)j / (float)N;
		}
	}

	unsigned int indices[(N-1) * (N-1)* 6];
	for (int i = 0; i < N - 1; i++)
	{
		for (int j = 0; j < N - 1; j++)
		{
			unsigned int a = (j + 0) * N + (i + 0); // bottom left
			unsigned int b = (j + 0) * N + (i + 1); // bottom right
			unsigned int c = (j + 1) * N + (i + 1); // top right
			unsigned int d = (j + 1) * N + (i + 0); // top left
			int dx = j * (N-1) + i;
			indices[dx * 6 + 0] = a;
			indices[dx * 6 + 1] = b;
			indices[dx * 6 + 2] = c;

			indices[dx * 6 + 3] = a;
			indices[dx * 6 + 4] = c;
			indices[dx * 6 + 5] = d;
		}
	}

	// init u,v for water
	float u[N*N];
	float unew[N*N];
	float v[N][N];
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			int dx = j * N + i;
			u[dx] = sin((float)i / N * 4 * 3.1415) * cos((float)j / N * 4 * 3.1415);
			unew[dx] = u[dx];
			v[i][j] = 0.0f;
		}
	}

	unsigned int waterHeightMapTex = loadTextureFromArray(u);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterHeightMapTex);


	unsigned int waterVBO, waterVAO, waterEBO;
	glGenVertexArrays(1, &waterVAO);
	glGenBuffers(1, &waterVBO);
	glGenBuffers(1, &waterEBO);

	glBindVertexArray(waterVAO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(field_vertices) , field_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices , GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float) ));
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);	

	// active the shader
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	waterShader.use();
	waterShader.setInt("heightmapTex", 2);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		glClearColor(0.25f, 0.75f, 0.75f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
		ourShader.setVec3("camera_front", camera.Front);

		glm::mat4 model = glm::mat4(1.0f);
		ourShader.setMat4("model", model);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		waterModel = glm::mat4(1.0f);
		waterModel = glm::scale(waterModel, glm::vec3(1/(float)N, 1/(float)N, 1/(float)N));
		waterModel = glm::translate(waterModel, glm::vec3(-0.5 * N, -0.2 * N, -0.5 * N));
		waterShader.use();


#if 1
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				float left = (i > 0) ? u[ARRAY(i - 1, j)] : u[ARRAY(0, j)];
				float right = (i < (N-1)) ? u[ARRAY(i + 1, j)] : u[ARRAY(N-1, j)];
				float up = (j > 0) ? u[ARRAY(i, j-1)] : u[ARRAY(i, 0)];
				float down = (j < (N-1)) ? u[ARRAY(i, j+1)] : u[ARRAY(i, N-1)];
				
				float f = 2 * ((left + right + down + up) - 4 * u[i + j*N]);
				{
					float max = 0.3;
					if (f > max) { f = max; }
					else if (f < -max) { f = -max; }
				}
				v[i][j] = v[i][j] + f * 0.16;
				//v[i][j] *= 0.99f;
				int dx = j* N + i;
				unew[dx] = u[dx] + v[i][j] * 0.16;
			}
		}

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				int dx = j* N + i;
				u[dx] = unew[dx];
			}
		}
#endif
		waterHeightMapTex = loadTextureFromArray(u);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, waterHeightMapTex);

		waterShader.setMat4("view", view);
		waterShader.setMat4("projection", projection);
		waterShader.setMat4("model", waterModel);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(waterVAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(*indices), GL_UNSIGNED_INT, 0);



		glfwSwapBuffers(window);
		glfwPollEvents();
		glDeleteTextures(1, &waterHeightMapTex);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!mouse_is_dragging)
	{
		lastX = xpos;
		lastY = ypos;
	}

	if (mouse_is_dragging)
	{
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

inline void printVec4(glm::vec4 v)
{
	std::cout << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")" << std::endl;
}


// glfw: whenever the mouse click, this callback is called
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}

	if (lbutton_down) {
		if (mouse_is_dragging == false)
		{
			mouse_is_dragging = true;
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			std::cout << "x:" << xpos << " y:" << ypos << std::endl;
			glm::vec4 mousePos = glm::vec4(xpos, ypos, 0.0f, 1.0f);
			glm::vec4 waterSpace = glm::inverse(waterModel) * glm::inverse(camera.GetViewMatrix()) * glm::inverse(projection) * mousePos;
			auto tmp = projection * camera.GetViewMatrix() * waterModel * waterSpace;
			printVec4(tmp);
			auto testVa = projection * camera.GetViewMatrix() * waterModel * glm::vec4(1, 0, 1, 1);
			printVec4(testVa);
			std::cout << "mouse clicking pos is: (" << waterSpace.x << "," << waterSpace.y << ", " << waterSpace.z << ")" << std::endl;
		}
	}
	else
	{
		mouse_is_dragging = false;
	}
}

unsigned int loadTexture(char const* path)
{
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
											// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
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
	return texture1;
}

unsigned int loadTextureFromArray(float* arr)
{
	unsigned int texture1;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
											// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, N, N, 0, GL_RED, GL_UNSIGNED_BYTE, arr); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, N, N , 0, GL_RED, GL_FLOAT, arr);
	glGenerateMipmap(GL_TEXTURE_2D);
	return texture1;
}

//unsigned int renderTexture()
//{
//	GLuint fbo;
//	glGenFramebuffers(1, &fbo);
//	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//
//	GLuint texture;
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
//
//
//	glDeleteFramebuffers(1, &fbo);
//}