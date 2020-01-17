#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);
void updateScrap();
void updateSpark();
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 mousePos(140.0f, -60.0f, 50.0f);
int step = 0;  // 0: normal, 1: ignite, 2: explode
vector<float> scrapX;
vector<float> scrapY;
vector<float> scrapVX;
vector<float> scrapVY;
const int SCRAP_NUM = 80;

float sparkX = 30.0f;
float sparkY = 40.0f;
float sparkVX = -4.0f;
float sparkVY = 0.0f;

int main()
{
    // glfw: initialize and configure ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation ----------------------------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bomb!", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_click_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers ---------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state --------------------------------
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // build and compile shaders -----------------------------------
    Shader ourShader("model_loading.vs", "model_loading.fs");
	Shader sparkShader("spark.vs", "model_loading.fs");
	Shader scrapShader("model_loading.vs", "scrap.fs");

    // load models -------------------------
	Model bomb("resources/objects/bomb/Bomb.obj");
	Model matchstick("resources/objects/matchstick/match.obj");

	// scrap init --------------------------
	float scrap[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};
	unsigned int scrapVAO, scrapVBO;
	glGenVertexArrays(1, &scrapVAO);
	glGenBuffers(1, &scrapVBO);

	glBindBuffer(GL_ARRAY_BUFFER, scrapVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scrap), scrap, GL_STATIC_DRAW);

	glBindVertexArray(scrapVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	srand(time(0));
	for (int i = 0; i < SCRAP_NUM; i++) {
		float x = (rand() % 25) - 12;
		float y = (rand() % 25) - 12;
		scrapX.push_back(x);
		scrapY.push_back(y);
		scrapVX.push_back(x * 4);
		scrapVY.push_back(y * 4);
	}

	// spark init --------------------------
	float spark[] = {
		0.0f, -0.3f, 0.0f, 0.5f, 0.0f,
		-0.2f, 0.2f, 0.0f, 0.3f, 0.5f,
		0.2f, 0.2f, 0.0f, 0.7f, 0.5f,
	};

	unsigned int sparkVAO, sparkVBO;
	glGenVertexArrays(1, &sparkVAO);
	glGenBuffers(1, &sparkVBO);

	glBindBuffer(GL_ARRAY_BUFFER, sparkVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spark), spark, GL_STATIC_DRAW);

	glBindVertexArray(sparkVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int sparkTexture = loadTexture("resources/textures/spark.png");
	
    // render loop ----------------------------
	while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic ---------------------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input --------------------------------
        processInput(window);

        // render --------------------------------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // view/projection transformations --------------------------
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
		
		scrapShader.use();
		scrapShader.setMat4("projection", projection);
		scrapShader.setMat4("view", view);

		sparkShader.use();
		sparkShader.setMat4("projection", projection);
		sparkShader.setMat4("view", view);
		
		ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
		
		// render spark ---------------------------
		glm::mat4 model = glm::mat4(1.0f);
		if (step == 1) {
			sparkShader.use();
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			glBindTexture(GL_TEXTURE_2D, sparkTexture);
			glBindVertexArray(sparkVAO);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(sparkX, sparkY, 50.0f) / 200.0f);
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
			sparkShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			updateSpark();
		}

		// render bomb ---------------------------
		model = glm::mat4(1.0f);
		if (step != 2) {
			ourShader.use();
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			bomb.Draw(ourShader);
		}

		// render matchstick ---------------------------
		ourShader.use();
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		model = glm::mat4(1.0f);
		model = glm::translate(model, mousePos / 200.0f);
		model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		matchstick.Draw(ourShader);

		// render scraps ---------------------------
		if (step == 2) {
			scrapShader.use();
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glBindVertexArray(scrapVAO);
			for (int i = 0; i < SCRAP_NUM; i++) {
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(scrapX[i], scrapY[i], 50.0f) / 200.0f);
				model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
				model = glm::rotate(model, (float)glfwGetTime()*(scrapVX[i]), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, (float)glfwGetTime()*(scrapVX[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, (float)glfwGetTime()*(scrapVX[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				scrapShader.setMat4("model", model);
				scrapShader.setVec4("ourColor", glm::vec4((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 1.0f));
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}
			updateScrap();
		}

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.) --------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources. ----------------
	glDeleteVertexArrays(1, &scrapVAO);
	glDeleteVertexArrays(1, &sparkVAO);
	glDeleteBuffers(1, &scrapVBO);
	glDeleteBuffers(1, &sparkVBO);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

	mousePos.x += xoffset;
	mousePos.y += yoffset;

    //camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == 0 && mousePos.x >= 64 && mousePos.x <= 70 && mousePos.y >= -17 && mousePos.y <= -10) {
		// if release mouse button and mouse position is within range of ignition
		step = 1;
	}
}

void updateSpark()
{
	sparkX += sparkVX * deltaTime;
	sparkY += sparkVY * deltaTime;
	if (sparkX < 22.0f) {
		sparkVX = -2.0f;
		sparkVY = -4.0f;
	}
	if (sparkX < 18.0f) {
		step = 2;
	}
}

void updateScrap()
{
	for (int i = 0; i < SCRAP_NUM; i++) {
		scrapX[i] = scrapX[i] + scrapVX[i] * deltaTime;
		scrapY[i] = scrapY[i] + scrapVY[i] * deltaTime;
		scrapVY[i] = scrapVY[i] - 30 * deltaTime;
	}
}

unsigned int loadTexture(const char *path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}