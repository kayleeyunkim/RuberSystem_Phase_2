/*
	Ashkenazi, Nir
	Kim, Yun
	Wasifi, Walid

	Project Warbird Simulation
	Phase 2
	Nov 12, 2014
	Shape3D.hpp
*/

# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

class Shape3D {
	private :
		int id, planet;
		glm::mat4 rotationMatrix;
		glm::mat4 scaleMatrix;
		glm::mat4 translationMatrix;
		glm::vec3 rotationAxis; 
		glm::mat4 modelMatrix;
		float radians; 
		bool orbital;

	public:


		Shape3D(int number) 
		{
			id = number;  // for debugging
			switch(id) 
			{
				case 0: // Ruber : red
					translationMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));
					rotationAxis = glm::vec3(0, 1, 0);
					radians = glm::radians(0.11f);
					orbital = false;
					planet = 0;
					printf("Created Ruber\n");
				break;

				case 1:  // Unum : blue
					translationMatrix = glm::translate(glm::mat4(), glm::vec3(4000, 0, 0));		//4000
					rotationAxis = glm::vec3(0, 1, 0);
					radians = glm::radians(0.4f); //Rotate around Ruber
					orbital = true;
					planet = 0;
					printf("Created Unum\n");
				break;

				case 2:  // Duo : green
					translationMatrix = glm::translate(glm::mat4(), glm::vec3(-9000, 0, 0));
					rotationAxis = glm::vec3(0, 1, 0);
					radians = glm::radians(0.2f); //Rotate around Ruber
					orbital = true;
					planet = 0;
					printf("Created Duo\n");
				break;

				case 3:  // Primus (pink)
					translationMatrix = glm::translate(glm::mat4(), glm::vec3(900, 0, 0));
					rotationAxis = glm::vec3(0,  1, 0);
					radians = glm::radians(0.9f); //Rotate around Duo
					orbital = true;
					planet = 2;
					printf("Created Primus\n");
				break;

				case 4:  // Secundus (yellow)
					translationMatrix = glm::translate(glm::mat4(), glm::vec3(1750, 0, 0));
					rotationAxis = glm::vec3(0,  1, 0);
					radians = glm::radians(0.7f); //Rotate around Duo
					orbital = true;
					planet = 2;
					printf("Created Secundus\n");
				break;

				case 5: // warbird
					translationMatrix = glm::translate(glm::mat4(), glm::vec3(5000, 1000, 5000));
					rotationAxis = glm::vec3(0,  1, 0);
					radians = glm::radians(0.0f); //No Rotation
					orbital = false;
					printf("Created Warbird\n");
				break;

				case 6:	// missile
					translationMatrix = glm::translate(glm::mat4(), glm::vec3(5000, 900, 5000));
					rotationAxis = glm::vec3(0,  1, 0);
					radians = glm::radians(0.0f); //No rotation
					orbital = false;
					printf("Created Missile\n");
				break;

				case 7:	// missile site
					translationMatrix = glm::translate(glm::mat4(), glm::vec3(4700, 1000, 4900));
					rotationAxis = glm::vec3(0,  1, 0);
					radians = glm::radians(0.0f); //No rotation
					orbital = false;
					printf("Created Missile\n");
				break;
				}
			rotationMatrix = glm::mat4();  // no initial orientation
		}

		glm::mat4 get_rotation()
		{
			return rotationMatrix;
		}

		glm::mat4 get_translation()
		{
			return translationMatrix;
		}


		glm::mat4 getPosition()
		{
			if (orbital)
			{
				if (orbital)
					return rotationMatrix * translationMatrix;
				else
					return translationMatrix * rotationMatrix;
			}
		}

		glm::mat4 getModelMatrix() 
		{
			if (orbital) // orbital rotation
				return(rotationMatrix * translationMatrix * scaleMatrix);
			else  // center rotation
				return(translationMatrix * rotationMatrix * scaleMatrix);
		}

		glm::mat4 getModelMatrix(glm::mat4 planet) 
		{
			if (orbital)
				if (orbital)
					return (planet * rotationMatrix * translationMatrix * scaleMatrix);
				else
					return (planet * translationMatrix * rotationMatrix * scaleMatrix);
		}

		void setScale (glm::vec3 matrix)
		{
			scaleMatrix = glm::scale(glm::mat4(), matrix);
		}


		void update() 
		{
			rotationMatrix = glm::rotate(rotationMatrix, radians, rotationAxis);
		}   


		void move(glm::vec3 dist)
		{
			rotationMatrix = glm::translate(rotationMatrix, dist);
		}

		void pitch(float radians)
		{
			rotationMatrix = glm::rotate(rotationMatrix, radians, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		void yaw(float radians)
		{
			rotationMatrix = glm::rotate(rotationMatrix, radians, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		void roll(float radians)
		{
			rotationMatrix = glm::rotate(rotationMatrix, radians, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		glm::vec3 Shape3D::getPos() 
		{
			 return glm::vec3(modelMatrix[3]);
		}

		glm::mat4 Shape3D::getRotationMatrix()
		{
			return rotationMatrix;
		}




};