//Chris Bowles
//Josh Nitzahn
//12/10/13
//Comp 465 Project

/*
warbird.cpp
*/

# define __Windows__

# include "C:\Users\Kaylee Kim\Desktop\465\project\includes465\include465.hpp"
//# include "..\includes465\glmUtils465.hpp"
# include "..\includes465\texture.hpp"  // freeTexture(...), loadRawTexture(...)
# define __INCLUDES465__

# include "3dobject.hpp"


# define DEFAULT_CAMERA_EYE glm::vec3(0.0f,0.0f,40000.0f)
# define DEFAULT_CAMERA_AT  glm::vec3(0.0f,0.0f,0.0f)
# define DEFAULT_CAMERA_UP  glm::vec3(0.0f,1.0f,0.0f)

# define CAMERAS 5
# define PLANETS 2
# define MOONS1 0
# define MOONS2 2
# define BODIES 5
# define GRAVITY 90000000

# define LAUNCHER_RADIUS 300.0f
# define WARBIRD_RADIUS 500.0f
# define MISSILE_RADIUS 300.0f
# define RUBER_RADIUS 2000.0f
# define UNUM_RADIUS 200.0f
# define DUO_RADIUS 400.0f
# define PRIMUS_RADIUS 100.0f
# define SEC_RADIUS 150.0f
# define MISSILE_OFFSET 2000.0f
# define DETECT_RADIUS 3000.0f


const int nModels = 4;
int currentCam = -1; //front view
bool gravityFlag = true;
bool gameOver = false;
int warp = 2;
Object3D * bodies[BODIES] = {NULL};
Object3D * uMissile = NULL;
Object3D * uLauncher = NULL;
Object3D * dMissile = NULL;
Object3D * dLauncher = NULL;
Object3D * wMissile = NULL;
Object3D * Ruber = NULL;
Object3D * Unum = NULL;
Object3D * Duo = NULL;
Object3D * Primus = NULL;
Object3D * Secundus = NULL;
Object3D * warbird = NULL;
Object3D * cameras[CAMERAS-1] = {NULL};
Object3D * topCam = NULL;
Object3D * UnumCam = NULL;
Object3D * DuoCam = NULL;
Object3D * warCam = NULL;

char * modelFile[nModels] = {"planet.tri", "WarBird1.tri", "missile.tri", "launcher.tri"};
float modelBoundingRadius[nModels];
float scaleValue[nModels];
const GLuint nVertices[nModels] = {2867 * 3, 3560 * 3, 384 * 3, 144 * 3};

char * vertexShaderFile = "vertexReview1.glsl";
char * fragmentShaderFile = "fragmentReview1.glsl";

GLuint shaderProgram;
GLuint VAO[nModels];
GLuint buffer[nModels];

GLuint modelview, project;
GLuint vPosition[nModels], vColor[nModels], vNormal[nModels];

glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelViewMatrix;
glm::mat4 projectionMatrix;

glm::mat4 frontCamMat = glm::lookAt(DEFAULT_CAMERA_EYE, DEFAULT_CAMERA_AT, DEFAULT_CAMERA_UP);

char titleString[256] = "465 Project";
char aboveUnum[] = "above Unum";
char aboveDuo[] = "above Duo";
char warbirdView[] = "warbird";
char topView[] = "top";
char frontView[] = "front";
char fpsStr[10] = "";
int rate = 40;
int rates[] = {400, 1000, 2500, 5000};
int timeq = rate;
int frames = 0;
int timerCalls = 0;
int Wmissiles = 0;
int Umissiles = 0;
int Dmissiles = 0;
int wLife = 0;
int uLife = 0;
int dLife = 0;
char *view;

enum movement {none, forward, backward, left, right, up, down, rollR, rollL};
enum mState {notFired, launched, tracking};

bool unumBase = true;
bool duoBase = true;

movement warMod = none;
mState unumState = notFired;
mState duoState = notFired;
mState wState = notFired;

// Texture and Model resources
int width = 1024, height = 1024;   // set texture's width and height values here
char * fileName = "stars.raw";  // set the texture file's name here
GLuint texture, Texture, showTexture;  // texture id and shader handle

void display();
void reshape(int width, int height);
void timer(int i);
void updateTitle();
void animate();
void keyboard(unsigned char key, int x, int y);
void keyboardSpec(int key, int x, int y);
void track(Object3D * target1, Object3D * target2, Object3D * missile);
bool proximityCheck(Object3D * obj1, Object3D * obj2, float margin);
void cleanUp (void);
void init();


int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    //glutInitContextVersion(3, 0);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("465 Warbird Project");

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
        printf("GLEW Error: %s \n", glewGetErrorString(err));
      else {
        printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
        printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
          glGetString(GL_SHADING_LANGUAGE_VERSION));
        }

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpec);
    glutTimerFunc(timeq, timer, 1);
	glutCloseFunc(cleanUp);  // freeglut only
    glutMainLoop();

    delete Ruber;

    printf("End of program.\n");
    return 0;
}

// free OpenGL resources
void cleanUp (void) {
  glDeleteBuffers(nModels, buffer);
  freeTexture(texture);   // in includes465/texture.hpp
  printf("cleaned up\n");
  }

void init() {
    shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
    glUseProgram(shaderProgram);

    glGenVertexArrays(nModels, VAO);
    glGenBuffers(nModels, buffer);

    for (int i = 0; i < nModels; i++) {
        modelBoundingRadius[i] = loadModelBuffer(modelFile[i], nVertices[i],
          VAO[i], buffer[i], shaderProgram, vPosition[i], vColor[i],
          vNormal[i], "vPosition", "vColor", "vNormal");
        }

    modelview = glGetUniformLocation(shaderProgram, "ModelView");
    project =   glGetUniformLocation(shaderProgram, "Projection");


    Ruber = new Object3D(PLANETS + 1);    //one camera satellite
    Ruber->setOrbit(glm::vec3(0.0f, 0.0f, 0.0f), 0);
    //Ruber->setRotate(glm::vec3(0.0f, 1.0f, 0.0f), 0);
    Ruber->setScale(glm::vec3(RUBER_RADIUS / modelBoundingRadius[0]));
    Ruber->setCamera(false);

    topCam = Ruber->getSatellite(0);
    topCam->setTranslation(glm::vec3(0.0f, 40000.0f, 0.0f));
    topCam->setOrbit(glm::vec3(0.0f, 0.0f, 0.0f), 0);
    //topCam->setRotate(glm::vec3(1.0f, 0.0f, 0.0f), PI / 2);

    Unum = Ruber->getSatellite(1);
    Unum->setTranslation(glm::vec3(4000.0f, 0.0f, 0.0f));
    Unum->setScale(glm::vec3(UNUM_RADIUS / modelBoundingRadius[0]));
    Unum->setOrbitAngle(0.004f);
    Unum->setCamera(false);

    Duo = Ruber->getSatellite(2);
    Duo->setTranslation(glm::vec3(-9000.0f, 0.0f, 0.0f));
    Duo->setScale(glm::vec3(DUO_RADIUS / modelBoundingRadius[0]));
    Duo->setOrbitAngle(0.002f);
    Duo->setCamera(false);

    Unum->makeSatellites(MOONS1 + 2);
    uLauncher = Unum->getSatellite(1);
    uLauncher->setTranslation(glm::vec3(0.0f, 215.0f, 0.0f));
    uLauncher->setScale(glm::vec3(LAUNCHER_RADIUS / modelBoundingRadius[3]));
    uLauncher->setCamera(false);

    UnumCam = Unum->getSatellite(0);
    UnumCam->setTranslation(glm::vec3(0.0f, 2000.0f, 0.0f));
    UnumCam->setOrbitAngle(0.0f);
    //UnumCam->setRotate(glm::vec3(0.0f, 0.0f, 1.0f), -PI / 2);

    Duo->makeSatellites(MOONS2 + 2);
    dLauncher = Duo->getSatellite(3);
    dLauncher->setTranslation(glm::vec3(0.0f, 415.0f, 0.0f));
    dLauncher->setScale(glm::vec3(LAUNCHER_RADIUS / modelBoundingRadius[3]));
    dLauncher->setCamera(false);

    DuoCam = Duo->getSatellite(0);
    DuoCam->setTranslation(glm::vec3(0.0f, 2000.0f, 0));
    DuoCam->setOrbitAngle(0.0f);
    //DuoCam->setRotate(glm::vec3(1.0f, 0.0f, 0.0f), PI / 2);

    Primus = Duo->getSatellite(1);
    Primus->setTranslation(glm::vec3(-900.0f, 0.0f, 0.0f));
    Primus->setScale(glm::vec3(SEC_RADIUS / modelBoundingRadius[0]));
    Primus->setOrbitAngle(0.004f);
    Primus->setCamera(false);

    Secundus = Duo->getSatellite(2);
    Secundus->setTranslation(glm::vec3(-1750.0f, 0.0f, 0.0f));
    Secundus->setScale(glm::vec3(SEC_RADIUS / modelBoundingRadius[0]));
    Secundus->setOrbitAngle(0.002f);
    Secundus->setCamera(false);

    warbird = new Object3D(1);
    warbird->setTranslation(glm::vec3(5000.0f, 1000.0f, 5000.0f));
    warbird->setScale(glm::vec3(WARBIRD_RADIUS / modelBoundingRadius[1]));
    warbird->setCamera(false);

    warCam = warbird->getSatellite(0);
    warCam->setTranslation(glm::vec3(0.0f, 500.0f, -1000.0f));
    warCam->setCameraOffset(glm::vec3(0.0f, 0.0f, 0.0f)); //y -> 250

    bodies[0] = Ruber;
    bodies[1] = Unum;
    bodies[2] = Duo;
    bodies[3] = Primus;
    bodies[4] = Secundus;

    cameras[0] = topCam;
    cameras[1] = warCam;

    cameras[2] = UnumCam;
    cameras[3] = DuoCam;


    uMissile = new Object3D;
    uMissile->setScale(glm::vec3(MISSILE_RADIUS / modelBoundingRadius[2]));
    uMissile->setCamera(false);

    dMissile = new Object3D;
    dMissile->setScale(glm::vec3(MISSILE_RADIUS / modelBoundingRadius[2]));
    dMissile->setCamera(false);

    wMissile = new Object3D;
    wMissile->setScale(glm::vec3(MISSILE_RADIUS / modelBoundingRadius[2]));
    wMissile->setCamera(false);


    viewMatrix = frontCamMat;

    view = frontView;
	
	
	// Uniforms
	showTexture = glGetUniformLocation(shaderProgram, "IsTexture");
  
	// load texture
	texture = loadRawTexture( texture, fileName, width, height);
	if (texture != 0) {
		printf("texture file, read, texture generated and bound.\n");
		Texture = glGetUniformLocation(shaderProgram, "Texture"); }
	else  // texture file loaded
		printf("Texture in file %s NOT LOADED !!! \n");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glBindVertexArray(VAO[0]);
    glEnableVertexAttribArray( vPosition[0] );
    glEnableVertexAttribArray( vColor[0] );
    glEnableVertexAttribArray( vNormal[0] );


    for (unsigned i = 0; i < BODIES; i++) {
        modelMatrix = bodies[i]->getModelMatrix();
        modelViewMatrix = viewMatrix * modelMatrix;
        glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
        glDrawArrays(GL_TRIANGLES, 0, nVertices[0]);
        }

    modelMatrix = warbird->getModelMatrix();
    modelViewMatrix = viewMatrix * modelMatrix;

    glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glBindVertexArray(VAO[1]);
    glEnableVertexAttribArray( vPosition[1] );
    glEnableVertexAttribArray( vColor[1] );
    glEnableVertexAttribArray( vNormal[1] );
    glDrawArrays(GL_TRIANGLES, 0, nVertices[1]);


    modelMatrix = uLauncher->getModelMatrix();
    modelViewMatrix = viewMatrix * modelMatrix;

    glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glBindVertexArray(VAO[3]);
    glEnableVertexAttribArray( vPosition[3] );
    glEnableVertexAttribArray( vColor[3] );
    glEnableVertexAttribArray( vNormal[3] );
    glDrawArrays(GL_TRIANGLES, 0, nVertices[3]);

    modelMatrix = dLauncher->getModelMatrix();
    modelViewMatrix = viewMatrix * modelMatrix;

    glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glDrawArrays(GL_TRIANGLES, 0, nVertices[3]);


    if(unumState == tracking || unumState == launched) {
        modelMatrix = uMissile->getModelMatrix();
        modelViewMatrix = viewMatrix * modelMatrix;

        glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glBindVertexArray(VAO[2]);
        glEnableVertexAttribArray( vPosition[2] );
        glEnableVertexAttribArray( vColor[2] );
        glEnableVertexAttribArray( vNormal[2] );
        glDrawArrays(GL_TRIANGLES, 0, nVertices[2]);
    }

    if(duoState == tracking || duoState == launched) {
        modelMatrix = dMissile->getModelMatrix();
        modelViewMatrix = viewMatrix * modelMatrix;

        glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glBindVertexArray(VAO[2]);
        glEnableVertexAttribArray( vPosition[2] );
        glEnableVertexAttribArray( vColor[2] );
        glEnableVertexAttribArray( vNormal[2] );
        glDrawArrays(GL_TRIANGLES, 0, nVertices[2]);
    }

    if(wState == tracking || wState == launched) {
        modelMatrix = wMissile->getModelMatrix();
        modelViewMatrix = viewMatrix * modelMatrix;

        glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glBindVertexArray(VAO[2]);
        glEnableVertexAttribArray( vPosition[2] );
        glEnableVertexAttribArray( vColor[2] );
        glEnableVertexAttribArray( vNormal[2] );
        glDrawArrays(GL_TRIANGLES, 0, nVertices[2]);
    }


    glutSwapBuffers();
    frames++;
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    projectionMatrix = glm::perspective(glm::radians(45.0f), (GLfloat) width /
        (GLfloat) height, 1.0f, 100000.0f);
}

void timer(int i) {
    glutTimerFunc(timeq, timer, 1);
    timerCalls++;
    if(timerCalls * timeq >= 1000) {
        sprintf(fpsStr, "U/S %2d   ", frames);

        timerCalls = frames = 0;
        updateTitle();
        }
    animate();
    }

void updateTitle() {
    sprintf(titleString, "465 Warbird Project:  Warbird %2d   Unum %1d"
            "   Duo %1d   %sView: %s", Wmissiles, Umissiles,
            Dmissiles, fpsStr, view);
    glutSetWindowTitle(titleString);
    }

void animate() {
    bodies[0]->update(); //This updates the planets

    //Default direction/pull values
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 pull = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rPosition = glm::vec3(warbird->getModelMatrix()[3]);
    float rDistance = glm::dot(rPosition, rPosition);
    float gConstant = GRAVITY / rDistance;


    //The lowest value rDistance can be is 1, but when it is, some funky things happen...

    if(gravityFlag && rDistance >= 4000000)
        pull = glm::normalize(rPosition) * -1.0f * gConstant;

    //The rotation needs to be fixed
    switch(warMod) {
    case forward:
        direction = glm::vec3(0.0f, 0.0f, 10.0f);
        break;
    case backward:
        direction = glm::vec3(0.0f, 0.0f, -10.0f);
        break;
    case left:
        warbird->yaw(0.02f);
        warCam->setOrbitAngle(0.02f);
        break;
    case right:
        warbird->yaw(-0.02f);
        warCam->setOrbitAngle(-0.02f);
        break;
    case up:
        warbird->pitch(-0.02f);
        warCam->setOrbitAxis(glm::vec3(1.0f, 0.0f, 0.0f));
        warCam->setOrbitAngle(-0.02f);
        break;
    case down:
        warbird->pitch(0.02f);
        warCam->setOrbitAxis(glm::vec3(1.0f, 0.0f, 0.0f));
        warCam->setOrbitAngle(0.02f);
        break;
    case rollR:
        warbird->roll(-0.02f);
        warCam->setOrbitAxis(glm::vec3(0.0f, 0.0f, 1.0f));
        warCam->setOrbitAngle(-0.02f);
        break;
    case rollL:
        warbird->roll(0.02f);
        warCam->setOrbitAxis(glm::vec3(0.0f, 0.0f, 1.0f));
        warCam->setOrbitAngle(0.02f);
        break;
    }
    //Move the warbird based on direction and pull
    warbird->move(direction);
    warbird->move2(pull);
    //The update applies to both warbird and warcam

    warbird->update();

    uMissile->update();
    dMissile->update();
    wMissile->update();

    //Reset the camera and warmod after the update
    warCam->setOrbitAxis(glm::vec3(0.0f, 1.0f, 0.0f));
    warCam->setOrbitAngle(0);
    warMod = none;

    if(unumState == tracking) {
        uLife++;
        track(warbird, NULL, uMissile);
        uMissile->move(glm::vec3(0.0f, 0.0f, 5.0f));
    }
    else if(unumState == launched) {
        uLife++;
        if(uLife >= 50)
            unumState = tracking;
        uMissile->move(glm::vec3(0.0f, 0.0f, 5.0f));
    }
    else if(proximityCheck(warbird, uLauncher, DETECT_RADIUS)) {
        if(Umissiles < 5 && unumBase == true) {
            printf("warbird is close to unum, so it fired missile #%d.\n", Umissiles);
            unumState = launched;
            Umissiles++;
            uLife = 0;
            glm::mat4 mod = uLauncher->getRotateMatrix();
            mod = glm::translate(mod, uLauncher->getPos()
                + glm::vec3(0.0f, 0.0f, LAUNCHER_RADIUS + MISSILE_RADIUS + 10));
            uMissile->reset(mod);
            uMissile->pitch(-PI/2);
            uMissile->move(glm::vec3(0.0f, 0.0f, 5.0f));
        }
    }

    if(duoState == tracking) {
        dLife++;
        track(warbird, NULL, dMissile);
        dMissile->move(glm::vec3(0.0f, 0.0f, 5.0f));
    }
    else if(duoState == launched) {
        dLife++;
        if(dLife >= 50)
            duoState = tracking;
        dMissile->move(glm::vec3(0.0f, 0.0f, 5.0f));
    }
    else if(proximityCheck(warbird, dLauncher, DETECT_RADIUS)) {
        if(Dmissiles < 5 && duoBase == true) {
            printf("warbird is close to duo, so it fired missile #%d.\n", Dmissiles);
            duoState = launched;
            Dmissiles++;
            dLife = 0;
            glm::mat4 mod = dLauncher->getRotateMatrix();
            mod = glm::translate(mod, dLauncher->getPos()
                + glm::vec3(0.0f, 0.0f, LAUNCHER_RADIUS + MISSILE_RADIUS + 10));
            dMissile->reset(mod);
            dMissile->pitch(-PI/2);
            dMissile->move(glm::vec3(0.0f, 0.0f, 5.0f));
        }
    }


    if(wState == tracking) {
        track(uLauncher, dLauncher, wMissile);
        wLife++;
        wMissile->move(glm::vec3(0.0f, 0.0f, 5.0f));
    }
    else if(wState == launched) {
        wLife++;
        if(wLife >= 50)
            wState = tracking;
        wMissile->move(glm::vec3(0.0f, 0.0f, 5.0f));
    }

    if(currentCam == -1)
        viewMatrix = frontCamMat;
    else
        viewMatrix = cameras[currentCam]->getView();

    if(proximityCheck(warbird, Ruber, RUBER_RADIUS + WARBIRD_RADIUS))
        gameOver = true;
    if(proximityCheck(warbird, Unum, UNUM_RADIUS + WARBIRD_RADIUS))
        gameOver = true;
    if(proximityCheck(warbird, Duo, DUO_RADIUS + WARBIRD_RADIUS))
        gameOver = true;
    if(proximityCheck(warbird, Secundus, SEC_RADIUS + WARBIRD_RADIUS))
        gameOver = true;
    if(proximityCheck(warbird, Primus, PRIMUS_RADIUS + WARBIRD_RADIUS))
        gameOver = true;

    if(proximityCheck(warbird, uMissile, MISSILE_RADIUS + WARBIRD_RADIUS
        + MISSILE_OFFSET)) {
            gameOver = true;
			uLife = 0;
            unumState = notFired;
    }
    if(proximityCheck(warbird, dMissile, MISSILE_RADIUS + WARBIRD_RADIUS
        + MISSILE_OFFSET)) {
            gameOver = true;
			dLife = 0;
            duoState = notFired;
    }
    if(proximityCheck(warbird, uLauncher, LAUNCHER_RADIUS + WARBIRD_RADIUS))
        gameOver = true;
    if(proximityCheck(warbird, dLauncher, LAUNCHER_RADIUS + WARBIRD_RADIUS))
        gameOver = true;

    if(unumBase && proximityCheck(wMissile, uLauncher, LAUNCHER_RADIUS + MISSILE_RADIUS
            + MISSILE_OFFSET)) {
        printf("Ship's missile %i hit Unum's launcher.\n", Wmissiles);
        wState = notFired;
        wLife = 0;
        unumBase = false;
    }
    if(duoBase && proximityCheck(wMissile, dLauncher, LAUNCHER_RADIUS + MISSILE_RADIUS
            + MISSILE_OFFSET)) {
        printf("Ship's missile %i hit Duo's launcher.\n", Wmissiles);
        wState = notFired;
        wLife = 0;
        duoBase = false;
    }

    if(uLife > 1000) {
        uLife = 0;
        unumState = notFired;
        printf("Unum's missile detonated and hit nothing.\n");
    }

    if(dLife > 2000) {
        dLife = 0;
        duoState = notFired;
        printf("Duo's missile detonated and hit nothing.\n");
    }

    if(wLife > 2000) {
        wLife = 0;
        wState = notFired;
        printf("Ship's missile detonated and hit nothing.\n");
    }

    if(unumBase == false && duoBase == false) {
        printf("YOU WIN\n");
        gameOver = true;
    }

    if(gameOver) {
        printf("GAME OVER\n");
        exit(EXIT_SUCCESS);
    }

    //The last line - redisplay the image
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': case 'Q': {
            exit(EXIT_SUCCESS);
            }
    case 'f': case 'F': if(!gameOver) {
            if(wState == notFired && Wmissiles < 10) {
                wState = launched;
                glm::mat4 m = glm::mat4(glm::mat3(warbird->getRotateMatrix()));
                glm::vec3 v = warbird->getPos();
                m[3] = glm::vec4(v, 1.0f);
                wMissile->reset(m);
                wLife = 0;
                Wmissiles++;
                wMissile->move(glm::vec3(0.0f, 0.0f, 5.0f));
                printf("fired missile number %d\n", Wmissiles);
                }
            break;
            }
    case 'v': case 'V': {
            currentCam = (currentCam == -1) ? 0 : ((currentCam == 0) ? 1 : -1);
            view = (currentCam == 0) ? topView :
                ((currentCam == 1) ? warbirdView : frontView);
            updateTitle();
            printf("camera changed.\n");
            }
            break;

    case 'p': case 'P': {
            currentCam = ( currentCam == 2 ) ? 3 : 2;
            view = (currentCam == 2) ? aboveUnum : aboveDuo;
            updateTitle();
            printf("camera changed.\n");
            }
            break;

    case 'g' : case 'G': {
            if(!gameOver) {
                gravityFlag = !gravityFlag;
                printf("gravity has been toggled to %d.\n", gravityFlag);
                }
            }
            break;

    case 'w': case 'W': if(!gameOver) {
            glm::mat4 mod = cameras[warp]->getRotateMatrix();
            mod = glm::translate(mod, cameras[warp]->getPos());
            warbird->reset(mod);
            warCam->reset(mod);
            warbird->pitch(PI/2);
            warCam->setOrbitAxis(glm::vec3(1.0f, 0.0f, 0.0f));
            warCam->setOrbitAngle(PI/2);
            printf("warped to planet %d.\n", (warp - 1));
            warp = (warp == 2) ? 3 : 2;
            }
            break;

    case 't': case 'T': if(!gameOver) {
            timeq = rates[rate = ++rate % 4];
            break;
        }

	case 'a': case 'A':
		timeq = 15;
		break;
    }
}

void keyboardSpec(int key, int x, int y) {
    if(gameOver)
        return;
    switch (key) {
    case GLUT_KEY_UP:
        if(glutGetModifiers() && GLUT_ACTIVE_CTRL)
            warMod = down;
        else
            warMod = forward;
        break;

    case GLUT_KEY_DOWN:
        if(glutGetModifiers() && GLUT_ACTIVE_CTRL)
            warMod = up;
        else
            warMod = backward;
        break;

    case GLUT_KEY_LEFT:
        if(glutGetModifiers() && GLUT_ACTIVE_CTRL)
            warMod = rollL;
        else
            warMod = left;
        break;

    case GLUT_KEY_RIGHT:
        if(glutGetModifiers() && GLUT_ACTIVE_CTRL)
            warMod = rollR;
        else
            warMod = right;
        break;
    }
}

bool proximityCheck(Object3D * obj1, Object3D * obj2, float margin)
{
    glm::vec3 pos1 = obj1->getPos();
    glm::vec3 pos2 = obj2->getPos();

    return ( glm::distance(pos1, pos2) <= margin );
}

void track(Object3D * target1, Object3D * target2, Object3D * missile)
{

    if(proximityCheck(missile, target1, DETECT_RADIUS)) {
        glm::vec3 targetVec = target1->getPos() - missile->getPos();
        glm::vec4 at2 = missile->getModelMatrix()[2];
        glm::vec3 at = glm::vec3(at2);
        if(!colinear(targetVec, at, 0.1f)){
            targetVec = glm::normalize(targetVec);
            at = glm::normalize(at);
            glm::vec3 axis = glm::normalize(glm::cross(targetVec, at));
            missile->orient(axis, targetVec);
            }
        }
    else if (target2 != NULL && proximityCheck(missile, target2, DETECT_RADIUS))
    {
        glm::vec3 targetVec = target2->getPos() - missile->getPos();
        glm::vec4 at2 = missile->getModelMatrix()[2];
        glm::vec3 at = glm::vec3(at2);
        if(!colinear(targetVec, at, 0.1f)){
            targetVec = glm::normalize(targetVec);
            at = glm::normalize(at);
            glm::vec3 axis = glm::normalize(glm::cross(targetVec, at));
            missile->orient(axis, targetVec);
        }
    }
}