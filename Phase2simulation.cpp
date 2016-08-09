/*
	Ashkenazi, Nir
	Kim, Yun
	Wasifi, Walid

	Project Warbird Simulation
	Phase 2
	Nov 12, 2014
	Phase2simulation.cpp
*/

# define __Windows__ // define your target operating system
# include "../includes465/include465.hpp"  

# include "Shape3D.hpp"

// Shapes
const int nShapes = 8;
Shape3D * shape[nShapes];
// Model for shapes

char * modelFile[nShapes] = 
	{	"Ruber.tri", "Unum.tri", "Duo.tri", "Primus.tri", "Secundus.tri", "WarBird1.tri", "missile.tri", "launcher.tri"}; 
const GLuint nVertices[nShapes] = 
	{	792 * 3, 792 * 3, 792 * 3, 792 * 3, 792 * 3, 3560 * 3, 384 * 3, 144*3};
float boundingRadius[nShapes];  
int Index =  0;  
float modelSize[nShapes] = 
	{	2000.0f, 200.0f, 400.0f, 100.0f, 150.0f, 200.0f, 300.0f, 300.0f};		//100.0f
const int Ruber = 0, Unum = 1, Duo = 2, Primus = 3, Secundus = 4, Warbird = 5, Missile = 6;
const int frontview = 0, topview = 1, unumview = 2, duoview = 3, warbirdview = 4;

GLuint vao[nShapes];  // VertexArrayObject
GLuint buffer[nShapes];  // Vertex Buffer Object
GLuint shaderProgram; 
char * vertexShaderFile = "simpleVertex.glsl";
char * fragmentShaderFile = "simpleFragment.glsl";

// shader handles
GLuint MVP ;  // Model View Projection matrix's handle
GLuint Position[nShapes], Color[nShapes], Normal[nShapes];   // vPosition, vColor, vNormal handles for models

glm::mat4 projectionMatrix;     // set in reshape()
glm::mat4 modelMatrix;          // set in shape[i]-->updateDraw()
glm::mat4 viewMatrix;           // set in keyboard()
glm::mat4 ModelViewProjectionMatrix; // set in display();
glm::vec3 scale[nShapes]; 

// display state and "state strings" for title display
// window title strings
char baseStr[50] =    "465 Ruber system phase 2: {v, t, s} : ";
char viewStr[16] =    " Initial view |";
char timerStr[20] = " | Initial speed | ";
char speedStr[30] = " Initial ship speed | ";
char titleStr [100]; 
char fpsStr[15] = "";
int timerDelay = 40, frameCount = 0;
double currentTime, lastTime, timeInterval; 

// camera
// vectors and values for lookAt
glm::vec3 eye, at, up;
int currentCam = 0;
int numberCam = 5;

// vectors for "model"
glm::vec4 vertex[nShapes];
glm::vec3 normal[nShapes];
glm::vec4 diffuseColorMaterial[nShapes];

// rotation variables
glm::mat4 identity(1.0f); 
glm::mat4 rotation;
// A delay of 40 milliseconds is 25 updates / second

// warbird movement
enum move{none, forward, backward, left, right, moveup, down, rollR, rollL};
move WarbirdMovement=none;

// boolean variable for unum and duo view
bool FRONT=false;
bool TOP = false;
bool UNUM=false;
bool DUO=false;
bool WARBIRD=false;

// variables for Time Quantum
int numberLevel = 5;
int currentlevel = 0;
int ace = 40, pilot = 100, trainee = 250, debug = 500, master = 15;

//variables for ship speed
int currentshipspeed = 0;
int numberspeed = 3;
bool SLOW =false, MEDIUM = false, FAST = false;

bool GameOver = false;

// game state
enum gamestate{notFired, launched, tracking};
gamestate unumState = notFired;
gamestate duoState = notFired;
gamestate wState = notFired;

int fired_missile = 0;
int umissible = 0;
int dmissiles = 0;
int wlife = 0;
int ulife = 0;
int dlife = 0;

////////////////////////////////////////////////////////////////////////////////////////////
//Functions
void init (void);
void reshape (int, int);
void display(void);
void change_camera_view();
void dynamicview(unsigned char);
void update (int);
void updateTitle();
void keyboard (unsigned char, int, int);
void keyboardMovement(int, int, int);
void gamelevel();
void shipspeed();
bool collisionCheck(Shape3D*, Shape3D*, float);
////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
//	init function
/////////////////////////////////////////////////////////////////////////////////
void init (void) 
{
	shaderProgram = loadShaders(vertexShaderFile,fragmentShaderFile);
	glUseProgram(shaderProgram);
  
	// generate VAOs and VBOs
	glGenVertexArrays( nShapes, vao );
	glGenBuffers( nShapes, buffer );

	// load the buffers from the model files
	for (int i = 0; i < nShapes; i++) 
	{
		boundingRadius[i] = loadModelBuffer(modelFile[i], nVertices[i], vao[i], buffer[i], 
							shaderProgram, Position[i], Color[i], Normal[i], "vPosition", "vColor", "vNormal"); 
		scale[i] = glm::vec3( modelSize[i] * 1.0f  /boundingRadius[i]);		// set scale for models given bounding radius  
	}
    
	MVP = glGetUniformLocation(shaderProgram, "ModelViewProjection");

	// inital view
	eye = glm::vec3(0.0f, 30000.0f, 20000.0f);
	at = glm::vec3(0);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	viewMatrix = glm::lookAt(eye, at, up);
  
	// set render state values
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.02f, 0.02f, 0.025f, 1.0f); //RGBA

	// create shape
	for(int i = 0; i < nShapes; i++)
	{
		shape[i] = new Shape3D(i);
		shape[i]->setScale(scale[i]);
	}

	printf("%d Shapes created \n", nShapes);

	lastTime = glutGet(GLUT_ELAPSED_TIME);
}


/////////////////////////////////////////////////////////////////////////////////
//	reshape
/////////////////////////////////////////////////////////////////////////////////
void reshape(int width, int height) 
{
	glViewport(0, 0, width, height);
	projectionMatrix = glm::perspective(glm::radians(45.0f), (GLfloat) width /  (GLfloat) height, 1.0f, 100000.0f); 
}


/////////////////////////////////////////////////////////////////////////////////
//	display function
/////////////////////////////////////////////////////////////////////////////////
void display(void) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update model matrix, set MVP, draw
	for(int i = 0; i < nShapes; i++) 
	{ 
		if (i == 3)
			modelMatrix = shape[3]->getModelMatrix(shape[Duo]->getPosition());
		else if(i == 4)
			modelMatrix = shape[4]->getModelMatrix(shape[Duo]->getPosition());
		else 
			modelMatrix = shape[i]->getModelMatrix();
		ModelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix; 
		glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix)); 
		glBindVertexArray(vao[i]);
		glEnableVertexAttribArray(Position[i]);
		glEnableVertexAttribArray(Color[i]);
		glEnableVertexAttribArray(Normal[i]);
		glDrawArrays(GL_TRIANGLES, 0, nVertices[i]);
	}
	glutSwapBuffers();
	frameCount++;

	// see if a second has passed to set estimated fps information
	currentTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time
	timeInterval = currentTime - lastTime;
	if ( timeInterval >= 1000) 
	{
		sprintf(fpsStr, " fps %4d", (int) (frameCount / (timeInterval / 1000.0f)) );
		lastTime = currentTime;
		frameCount = 0;
		updateTitle();
	}

	if (FRONT == true)
	{
		TOP=false;
		UNUM=false;
		DUO=false;
		WARBIRD=false;
		eye = glm::vec3(0.0f, 10000.0f, 20000.0f);
		at  = glm::vec3(0.0f,	  0.0f,		0.0f);
		up  = glm::vec3(0.0f,	  1.0f,		0.0f);
		viewMatrix = glm::lookAt(eye, at, up);
	}

	if (TOP == true)
	{
		FRONT=false;
		UNUM=false;
		DUO=false;
		WARBIRD=false;
		eye = glm::vec3(0.0f, 25000.0f,  0.0f);  
		at  = glm::vec3(0.0f,     0.0f,  0.0f);    
		up  = glm::vec3(1.0f,     0.0f,  0.0f); 
		viewMatrix = glm::lookAt(eye, at, up);
	}
	
	if(UNUM == true)
	{
		FRONT=false;
		TOP=false;
		DUO=false;
		WARBIRD=false;
		glm::mat4 unum_loc = shape[Unum]->get_rotation() * shape[Unum]->get_translation();
		eye = glm::vec3(unum_loc[3].x, 5000.0f, unum_loc[3].z);
		at  = glm::vec3(unum_loc[3].x,    0.0f, unum_loc[3].z);
		up  = glm::vec3(0.0f, 0.0f, -1.0f);
		viewMatrix = glm::lookAt(eye, at, up);
	}

	if(DUO == true)
	{
		FRONT=false;
		TOP=false;
		UNUM=false;
		WARBIRD=false;
		glm::mat4 duo_loc = shape[Duo]->get_rotation() * shape[Duo]->get_translation();
		eye = glm::vec3(duo_loc[3].x, 5000.0f, duo_loc[3].z);
		at  = glm::vec3(duo_loc[3].x, duo_loc[3].y, duo_loc[3].z);
		up  = glm::vec3(1.0f, 0.0f, 0.0f);
		viewMatrix = glm::lookAt(eye, at, up);
	}

	if (WARBIRD == true)
	{
		FRONT=false;
		TOP=false;
		UNUM=false;
		DUO=false;
		glm::mat4 bird_loc = shape[Warbird]->get_translation() * shape[Warbird]->get_rotation();
		eye = glm::vec3((bird_loc[3].x), (bird_loc[3].y + 500.0f), (bird_loc[3].z - 700.0f));
		at  = glm::vec3(bird_loc[3].x, (bird_loc[3].y + 200.0f), bird_loc[3].z);
		up  = glm::vec3(0.0f, 100.0f, 80.0f);
		viewMatrix = glm::lookAt(eye, at, up);
	}

	
	/*
	if 
		(collisionCheck(shape[5], shape[0], 2000.0f + 200.0f))	// warbird & ruber
		(collisionCheck(shape[0], shape[1], 200.0f + 200.0f)) ||	// warbird & unum
		(collisionCheck(shape[5], shape[2], 400.0f + 200.0f)) ||	// warbird & duo
		(collisionCheck(shape[5], shape[3], 100.0f + 200.0f)) ||	// warbird & primus
		(collisionCheck(shape[5], shape[4], 150.0f + 200.0f)) ||	// warbird & secundus
		(collisionCheck(shape[5], shape[6], 50.0f + 200.0f)) ||		// warbird & missile
		(collisionCheck(shape[5], shape[7], 200.0f + 200.0f)))		// warbird & missile site
		{
			GameOver = true;
			printf("Game over");
			
		}
		*/
}


/////////////////////////////////////////////////////////////////////////////////
//	helper function for view when pressing "v"
/////////////////////////////////////////////////////////////////////////////////
void change_camera_view()
{
	if (currentCam == 0)		// front view
	{
		char key;
		key = 'f';
		dynamicview(key);
	}

	if (currentCam == 1)		// top view
	{
		char key;
		key = 't';
		dynamicview(key);
	}

	if (currentCam == 2)		// unum view
	{
		char key;
		key = 'u';
		dynamicview(key);
	}

	if (currentCam == 3)		// duo view
	{
		char key;
		key = 'd';
		dynamicview(key);
	}

	if (currentCam == 4)		// warbird view
	{
		char key;
		key = 'w';
		dynamicview(key);
	}

		/*
		eye = glm::vec3(5000.0f, 1100.0f, 4700.0f);  //5000, 900, 5500 
		at  = glm::vec3(5000.0f, 1100.0f, 5000.0f);//5000, 1100, 5000    
		up  = glm::vec3(0.0f,   1.0f,    0.0f);
		*/
	currentCam++;
}


/////////////////////////////////////////////////////////////////////////////////
//	helper function for view when pressing "v"
/////////////////////////////////////////////////////////////////////////////////
void dynamicview (unsigned char key)
{
	if (key == 'f')
	{
		FRONT=true;
		TOP=false;
		UNUM=false;
		DUO=false;
		WARBIRD=false;
		strcpy(viewStr, " Front view | ");
	}

	if (key == 't')
	{
		FRONT=false;
		TOP=true;
		UNUM=false;
		DUO=false;
		WARBIRD=false;
		strcpy(viewStr, " Top view | ");
	}

	if (key == 'u')
	{
		FRONT=false;
		TOP=false;
		UNUM=true;
		DUO=false;
		WARBIRD=false;
		strcpy(viewStr, "Unum view | ");
	}

	else if (key == 'd')
	{
		FRONT=false;
		TOP=false;
		UNUM=false;
		DUO=true;
		WARBIRD=false;
		strcpy(viewStr, "Duo view | ");
	}
	
	else if (key == 'w')
	{
		FRONT=false;
		TOP=false;
		UNUM=false;
		DUO=false;
		WARBIRD=true;
		strcpy(viewStr, "Warbird view | ");
	}
}


/////////////////////////////////////////////////////////////////////////////////
// Animate scene objects by updating their transformation matrices
// timerDelay = 40, or 25 updates / second
/////////////////////////////////////////////////////////////////////////////////
void update (int i) 
{ 
	glutTimerFunc(timerDelay, update, 1);
	for(int i = 0; i < nShapes; i++) 
		shape[i] -> update();
}


/////////////////////////////////////////////////////////////////////////////////
// update and display animation state in window title
/////////////////////////////////////////////////////////////////////////////////
void updateTitle() 
{
	strcpy(titleStr, baseStr);
	strcat(titleStr, viewStr);
	strcat(titleStr, fpsStr);
	strcat(titleStr, timerStr);
	strcat(titleStr, speedStr);
	glutSetWindowTitle(titleStr);
}


/////////////////////////////////////////////////////////////////////////////////
//	keyboard function for game menu
/////////////////////////////////////////////////////////////////////////////////
// Quit or set the view
void keyboard (unsigned char key, int x, int y) 
{
	switch(key) 
	{
		case 'q' :  case 'Q' : exit(EXIT_SUCCESS); 
			break;

		case 'v': case 'V' :
			if (currentCam >= numberCam)
				currentCam = 0;
			change_camera_view();
			break;

		case 't' : case 'T' : // increase speed
			if (currentlevel >= numberLevel)
				currentlevel = 0;
			gamelevel();
			break;
			
		case 's': case 'S' :
			if (currentshipspeed >= numberspeed)
				currentshipspeed = 0;
			shipspeed();
			break;


			
		case 'f' : case 'F' : if (!GameOver)
			if(wState == notFired && fired_missile < 10) 
			{
                wState = launched;
                glm::mat4 m = glm::mat4(glm::mat3(shape[5]->getRotationMatrix()));
                glm::vec3 v = shape[5]->getPos();
                m[3] = glm::vec4(v, 1.0f);
         //       shape[6]->reset(m);
                wlife = 0;
                fired_missile++;
				shape[6]->move(glm::vec3(0.0f, 0.0f, 100.0f));
				timerDelay =20;
				
                printf("fired missile number %d\n", fired_missile);
                }
            break;
			
            
	}
	viewMatrix = glm::lookAt(eye, at, up);
	updateTitle();
}


/////////////////////////////////////////////////////////////////////////////////
//	Keyboard function for warship movement
/////////////////////////////////////////////////////////////////////////////////
void keyboardMovement(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			if(glutGetModifiers() && GLUT_ACTIVE_CTRL)
			{
				WarbirdMovement=moveup;
				shape[5]->pitch(-0.02f);
				shape[6]->pitch(-0.02f);
			}
			else
			{
				WarbirdMovement=forward;
				if (SLOW == false && MEDIUM == false && FAST == false)
				{
					shape[5]->move(glm::vec3(0.0f, 0.0f, 10.0f));
					shape[6]->move(glm::vec3(0.0f, 0.0f, 10.0f));
				}
				if (SLOW == true && MEDIUM == false && FAST == false)
				{
					shape[5]->move(glm::vec3(0.0f, 0.0f, 10.0f));
					shape[6]->move(glm::vec3(0.0f, 0.0f, 10.0f));
				}
				if (SLOW == false && MEDIUM == true && FAST == false)
				{
					shape[5]->move(glm::vec3(0.0f, 0.0f, 50.0f));
					shape[6]->move(glm::vec3(0.0f, 0.0f, 50.0f));
				}
				if (SLOW == false && MEDIUM == false && FAST == true)
				{
					shape[5]->move(glm::vec3(0.0f, 0.0f, 200.0f));
					shape[6]->move(glm::vec3(0.0f, 0.0f, 200.0f));
				}
			}
		break;

		case GLUT_KEY_DOWN:
			if(glutGetModifiers() && GLUT_ACTIVE_CTRL)
			{
				WarbirdMovement=down;
				shape[5]->pitch(0.02f);
				shape[6]->pitch(0.02f);
			}
			else
			{
				WarbirdMovement=backward;
				if (SLOW == false && MEDIUM == false && FAST == false)
				{
					shape[5]->move(glm::vec3(0.0f, 0.0f, -10.0f));
					shape[6]->move(glm::vec3(0.0f, 0.0f, -10.0f));
				}
				if (SLOW == true && MEDIUM == false && FAST == false)
				{
					shape[5]->move(glm::vec3(0.0f, 0.0f, -10.0f));
					shape[6]->move(glm::vec3(0.0f, 0.0f, -10.0f));
				}
				if (SLOW == false && MEDIUM == true && FAST == false)
				{
					shape[5]->move(glm::vec3(0.0f, 0.0f, -50.0f));
					shape[6]->move(glm::vec3(0.0f, 0.0f, -50.0f));
				}
				if (SLOW == false && MEDIUM == false && FAST == true)
				{
					shape[5]->move(glm::vec3(0.0f, 0.0f, -200.0f));
					shape[6]->move(glm::vec3(0.0f, 0.0f, -200.0f));
				}
			}
		break;

		case GLUT_KEY_LEFT:
			if(glutGetModifiers() && GLUT_ACTIVE_CTRL)
			{
				WarbirdMovement=rollL;
				shape[5]->roll(-0.02f);
				shape[6]->roll(-0.02f);
			}
			else
			{
				WarbirdMovement=left;
				shape[5]->yaw(0.02f);
				shape[6]->yaw(0.02f);
			}
		break;

		case GLUT_KEY_RIGHT:
			if(glutGetModifiers() && GLUT_ACTIVE_CTRL)
			{
				WarbirdMovement=rollR;
				shape[5]->roll(0.02f);
				shape[6]->roll(0.02f);
			}
			else
			{
				WarbirdMovement=right;
				shape[5]->yaw(-0.02f);
				shape[6]->yaw(-0.02f);
			}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////////
//	change overall speed
/////////////////////////////////////////////////////////////////////////////////
void gamelevel ()
{
	if (currentlevel == 0)
	{
		timerDelay = ace;
		strcpy(timerStr, " | Ace speed | ");
	}
	
	if (currentlevel == 1)
	{
		timerDelay = pilot;
		strcpy(timerStr, " | Pilot speed | ");
	}

	if (currentlevel == 2)
	{
		timerDelay = trainee;
		strcpy(timerStr, " | Trainee speed | ");
	}

	if (currentlevel == 3)
	{
		timerDelay = debug;
		strcpy(timerStr, " | Debug speed | ");
	}

	if (currentlevel == 4)
	{
		timerDelay = master;
		strcpy(timerStr, " | Master speed | ");
	}
	currentlevel++;
}


/////////////////////////////////////////////////////////////////////////////////
//	change the ship speed (change status)
/////////////////////////////////////////////////////////////////////////////////
void shipspeed()
{
	if (currentshipspeed == 0)
	{
		SLOW = true;
		MEDIUM = false;
		FAST = false;
		strcpy(speedStr, " Slow ship speed | ");
	}

	if (currentshipspeed == 1)
	{
		SLOW = false;
		MEDIUM = true;
		FAST = false;
		strcpy(speedStr, " Medium ship speed | ");
	}

	if (currentshipspeed == 2)
	{
		SLOW = false;
		MEDIUM = false;
		FAST = true;
		strcpy(speedStr, " Fast ship speed | ");
	}

	currentshipspeed++;
}

/////////////////////////////////////////////////////////////////////////////////
//	Collision check between Warbird and other objects
/////////////////////////////////////////////////////////////////////////////////
/*
bool collisionCheck(Shape3D* obj1, Shape3D* obj2, float margin)
{
	glm::vec3 pos1 = obj1->getPos();
    glm::vec3 pos2 = obj2->getPos();

    return ( glm::distance(pos1, pos2) <= margin );
}
*/

/////////////////////////////////////////////////////////////////////////////////
//	main function
/////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("465 manyCubes Example {v, t, s} : Initial view | ");
	// initialize and verify glew
	glewExperimental = GL_TRUE;  // needed my home system 
	GLenum err = glewInit();  
	if (GLEW_OK != err) 
		printf("GLEW Error: %s \n", glewGetErrorString(err));      
	else 
	{
		printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
		printf("OpenGL %s, GLSL %s\n", 
		glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
	// initialize scene
	init();
	// set glut callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardMovement);
	glutTimerFunc(timerDelay, update, 1);
	glutIdleFunc(display);
	glutMainLoop();
	printf("done\n");
	
	return 0;
}