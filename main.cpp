
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/spline.hpp>           //biblioteca adicionada para fazer o spline
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <string.h>
#include <stb_image.h>
#include <iostream>
#include "C:\Users\Henrique\Desktop\faculdade\2019-1\CG\Pojetin\Modelin.cpp"  //chamada da classe Modelin usada para definir o modelo
#include <vector>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
bool CheckCollision(glm::vec3 one, glm::vec3 two);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// camera
Camera camera = glm::vec3(0.0f, 0.65f, 0.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float y;									//para armazenar as coordenadas da célula que será adicionada
float z;
int atual = 0;								//armazenar em qual corredor a câmera está



//variáveis
int modelos = 0;
std::vector<Modelin> P(1, Modelin(0,-4.0f, 0.0f));       //crio um vetor de structs modelin e dou um tamanho do numero de modelos
float zoom = 25.0f;
float scala = 0.0f;
int pivot = 0;                              //variável usada para realizar ações referentes a um modelo específico no loop



int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
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
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
		// -----------------------------
			//
	glEnable(GL_DEPTH_TEST);


	// build and compile shaders
	// -------------------------
	Shader ourShader(FileSystem::getPath("resources/cg_ufpel.vs").c_str(), FileSystem::getPath("resources/cg_ufpel.fs").c_str());

	// build and compile our shader zprogram
	// ------------------------------------
	

	// load models
	// -----------
	Model Fim(FileSystem::getPath("resources/objects/corridor/Hallway_End.obj"));
	Model Reto(FileSystem::getPath("resources/objects/corridor/Hallway_Straight.obj"));
	Model Quad(FileSystem::getPath("resources/objects/corridor/Hallway_Cross.obj"));
	Model Porta(FileSystem::getPath("resources/objects/corridor/Hallway_Door.obj"));

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/container2.png").c_str());
	unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/container2_specular.png").c_str());

	


	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);					//usei a variável zoom para ampliar zoom e reduzir zoom
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		
		std::cout << pivot << std::endl;				//printar o pivot no terminal


		// render the loaded model
		for (int i = 0; i < P.size(); i++) {
			glm::mat4 model;
			

			
			
			
			model = glm::translate(model, glm::vec3(P[i].Tx + P[i].somax, P[i].Ty , P[i].Tz + P[i].somaz)); // translate it down so it's at the center of the scene
			if(P[i].R!=0)
				model = glm::rotate(model, glm::radians(P[i].R), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.4f + scala, 0.4f + scala, 0.4f + scala)); // it's a bit too big for our scene, so scale it down



			if (P[i].modelo == 0) {
				ourShader.setMat4("model", model);			//1-Corredor Reto
				Reto.Draw(ourShader);
			}
			if (P[i].modelo == 1) {
				ourShader.setMat4("model", model);			//2-Corredor em Cruz
				Quad.Draw(ourShader);
			}
			if (P[i].modelo == 2) {							//3-Fim de corredor
				ourShader.setMat4("model", model);
				Fim.Draw(ourShader);
			}
			if (P[i].modelo == 3) {							//4-Porta final
				ourShader.setMat4("model", model);			
				Porta.Draw(ourShader);
			}
			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------

		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}






	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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


	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {         //adiciona corredor reto em cima
		if (P[pivot].norte == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == 0 || P[pivot].modelo == 1)) {		//para poder adicionar ao norte, o corredor ao sul não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 0;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[pivot].norte = true;
			P[modelos].sul = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz - 3.57f;
			}
			else {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz - 4.0f;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {         //adiciona corredor em cruz em cima
		if (P[pivot].norte == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == 0 || P[pivot].modelo == 1)) {		//para poder adicionar ao norte, o corredor ao sul não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 1;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[pivot].norte = true;
			P[modelos].sul = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz - 7.2f;
			}
			else {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz - 7.6f;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {			//adiciona um fim de corredor em cima
		if (P[pivot].norte == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == 0 || P[pivot].modelo==1)) {		//para poder adicionar ao norte, o corredor ao sul não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 2;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[pivot].norte = true;
			P[modelos].sul = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz - 3.6f;
			}
			else {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz - 4.0f;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {			//adiciona uma porta em cima
		if (P[pivot].norte == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == 0 || P[pivot].modelo == 1)) {		//para poder adicionar ao norte, o corredor ao sul não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 3;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[pivot].norte = true;
			P[modelos].sul = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz - 4.0f;
			}
			else {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz - 4.0f;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {			//soma no pivot(que escolherá em qual corredor adicionar)
		if(pivot!=0)
			pivot -= 1;
		Sleep(300.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {			//subtrai do pivot(que escolherá em qual corredor adicionar)
		if (pivot < modelos)
			pivot += 1;
		Sleep(300.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {			//adiciona um corredor reto à direita
		if (P[pivot].leste == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == 90 || P[pivot].modelo == 1)) {		//para poder adicionar ao leste, o corredor ao oeste não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 0;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[modelos].R = 90;
			P[pivot].leste = true;
			P[modelos].oeste = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax + 7.5f;
				P[modelos].somaz = P[pivot].somaz;
			}
			else {
				P[modelos].somax = P[pivot].somax + 4.0f;
				P[modelos].somaz = P[pivot].somaz;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {			//adiciona um corredor em cruz à direita
		if (P[pivot].leste == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == 90 || P[pivot].modelo == 1)) {		//para poder adicionar ao leste, o corredor ao oeste não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 1;
			P[modelos].contador = P[modelos-1].contador + 1;
			P[modelos].R = 90;
			P[pivot].leste = true;
			P[modelos].oeste = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax + 7.2f;
				P[modelos].somaz = P[pivot].somaz;
			}
			else {
				P[modelos].somax = P[pivot].somax + 3.5f;
				P[modelos].somaz = P[pivot].somaz;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {			//adiciona um fim de corredor à direita
		if (P[pivot].leste == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == 90 || P[pivot].modelo == 1)) {		//para poder adicionar ao leste, o corredor ao oeste não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 2;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[modelos].R = -90;
			P[pivot].leste = true;
			P[modelos].oeste = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax +3.5f;
				P[modelos].somaz = P[pivot].somaz;
			}
			else {
				P[modelos].somax = P[pivot].somax +0.0f;
				P[modelos].somaz = P[pivot].somaz;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {			//adiciona uma porta à direita
		if (P[pivot].leste == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == 90 || P[pivot].modelo == 1)) {		//para poder adicionar ao leste, o corredor ao oeste não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 3;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[modelos].R = 90;
			P[pivot].leste = true;
			P[modelos].oeste = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax + 5.475f;
				P[modelos].somaz = P[pivot].somaz-0.05;
			}
			else {
				P[modelos].somax = P[pivot].somax + 0.0f;
				P[modelos].somaz = P[pivot].somaz;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {			//adiciona um corredor reto à esquerda
		if (P[pivot].oeste == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == -90 || P[pivot].modelo == 1)) {		//para poder adicionar ao oeste, o corredor ao leste não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 0;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[modelos].R = -90;
			P[pivot].oeste = true;
			P[modelos].leste = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax - 7.6f;
				P[modelos].somaz = P[pivot].somaz;
			}
			else {
				P[modelos].somax = P[pivot].somax - 4.0f;
				P[modelos].somaz = P[pivot].somaz;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {			//adiciona um corredor em cruz à esquerda
		if (P[pivot].oeste == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == -90 || P[pivot].modelo == 1)) {		//para poder adicionar ao oeste, o corredor ao leste não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 1;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[modelos].R = -90;
			P[pivot].oeste = true;
			P[modelos].leste = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax - 7.2f;
				P[modelos].somaz = P[pivot].somaz;
			}
			else {
				P[modelos].somax = P[pivot].somax - 3.5f;
				P[modelos].somaz = P[pivot].somaz;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {			//adiciona um fim de corredor à esquerda
		if (P[pivot].oeste == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == -90 || P[pivot].modelo == 1)) {		//para poder adicionar ao oeste, o corredor ao leste não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 2;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[modelos].R = 90;
			P[pivot].oeste = true;
			P[modelos].leste = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax - 3.6f;
				P[modelos].somaz = P[pivot].somaz;
			}
			else {
				P[modelos].somax = P[pivot].somax - 0.0f;
				P[modelos].somaz = P[pivot].somaz;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {			//adiciona uma porta reto à esquerda
		if (P[pivot].oeste == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == -90 || P[pivot].modelo == 1)) {		//para poder adicionar ao oeste, o corredor ao leste não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 3;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[modelos].R = -90;
			P[pivot].oeste = true;
			P[modelos].leste = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax - 5.5f;
				P[modelos].somaz = P[pivot].somaz+0.05f;
			}
			else {
				P[modelos].somax = P[pivot].somax - 1.89f;
				P[modelos].somaz = P[pivot].somaz+0.1f;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {         //adiciona corredor reto em baixo
		if (P[pivot].sul == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3)) {		//para poder adicionar ao sul, o corredor ao norte não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 0;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[pivot].sul = true;
			P[modelos].norte = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz + 7.6f;
			}
			else {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz + 4.0f;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {         //adiciona corredor em cruz em baixo
		if (P[pivot].sul == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3)) {		//para poder adicionar ao sul, o corredor ao norte não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 1;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[pivot].sul = true;
			P[modelos].norte = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz + 7.2f;
			}
			else {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz + 3.6f;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {			//adiciona um fim de corredor em baixo
		if (P[pivot].sul == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3)) {		//para poder adicionar ao sul, o corredor ao norte não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 2;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[pivot].sul = true;
			P[modelos].norte = true;
			P[modelos].R = 180;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz + 3.6f;
			}
			else {
				P[modelos].somax = P[pivot].somax;
				P[modelos].somaz = P[pivot].somaz + 0.0f;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {			//adiciona uma porta em baixo
		if (P[pivot].sul == false && (P[pivot].modelo != 2 && P[pivot].modelo != 3) && (P[pivot].R == 180 || P[pivot].modelo == 1)) {		//para poder adicionar ao sul, o corredor ao norte não pode ser fim de corredor ou porta
			modelos++;
			P.resize(modelos);
			P.push_back(Modelin());
			P[modelos].modelo = 3;
			P[modelos].contador = P[modelos - 1].contador + 1;
			P[pivot].sul = true;
			P[modelos].norte = true;
			if (P[pivot].modelo == 1) {
				P[modelos].somax = P[pivot].somax+0.1f;
				P[modelos].somaz = P[pivot].somaz + 5.48f;
			}
			else {
				P[modelos].somax = P[pivot].somax+0.08f;
				P[modelos].somaz = P[pivot].somaz + 1.89f;
			}
			Sleep(300.0f);
			pivot = modelos;
		}
	}
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
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









