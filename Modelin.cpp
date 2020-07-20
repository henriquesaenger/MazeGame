#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
using namespace std;




class Modelin {

public:
	float Tx;
	float Ty;
	float Tz;
	float R;
	int modelo;			//variável que vai definir qual animação será desenhada 
	float somax;
	float somaz;
	int contador;
	bool norte;
	bool sul;
	bool leste;
	bool oeste;


	Modelin::Modelin() {
		Tx = 0.0f;
		Ty = 0.0f;
		Tz = 0.0f;
		R = 0.0f;
		modelo = 0;
		somax = 0.0f;
		somaz = 0.0f;
		contador = 0;
		norte = false;
		sul = false;
		leste = false;
		oeste = false;
		}

	Modelin::Modelin(int model, float z, float x) {
		Tx = 0.0f;
		Ty = 0.0f;
		Tz = 0.0f;
		R = 0.0f;
		modelo = model;
		somax = x;
		somaz = z;
		contador = 0;
		norte = false;
		sul = false;
		leste = false;
		oeste = false;
	}
};