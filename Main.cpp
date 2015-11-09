// ----------------------------------------------
// Informatique Graphique 3D & Réalité Virtuelle.
// Travaux Pratiques
// Introduction à OpenGL
// Copyright (C) 2015 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
// Modified by Artur Andrade Motta Sarlo
// November 2015
// ----------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <GL/glut.h>

using namespace std;

// Light Parameters
static int light0En = 1;
static int light1En = 1;

// Texture Parameters
#define TEXT_HEIGHT 8
#define TEXT_WIDTH 8
// Buffer to store the texture
static unsigned char image[TEXT_HEIGHT*TEXT_WIDTH*3];
// OpenGL identifier of the texture
static GLuint texture;

// App parameters
static const unsigned int DEFAULT_SCREENWIDTH = 1024;
static const unsigned int DEFAULT_SCREENHEIGHT = 768;
static string appTitle ("Informatique Graphique & Realite Virtuelle -Travaux Pratiques - Introduction a OpenGL");
static GLint window;
static unsigned int screenWidth;
static unsigned int screenHeight;

// Camera parameters
static float fovAngle;
static float aspectRatio;
static float nearPlane;
static float farPlane;
// Polar coordinates of the camera
// static float camPhi;
// static float camTheta;
// static float camDist2Target;
// Cartesian coordinates of the object that the camera is looking for
static float camTargetX;
static float camTargetY;
static float camTargetZ;

// Animation and Interaction Variables
static float beforeTime, currentTime = 0.0f;
static float angle = 0.0f;
static float speed = 0.0f;
static float acceleration = 0.0f;

// Camera Variables
// Cartesian coordinates of the camera
static float xci, yci, zci = 0.0f;
// Polar coordinates of the camera
static float phici, thetaci, rci = 0.0f;
// Cartesian coordinates of the mouse
static float xmi, ymi = 0.0f;
static int mouseClicked = 0;

/**
 * Convert polar notation (degres) to cartesian notation.
 *
 * @param phi Adjacent angle to y-axe
 * @param theta Adjacent angle to z-axe
 * @param d Distance from the origin
 * @param &x Buffer for the return of the cartesian coordinate x
 * @param &y Buffer for the return of the cartesian coordinate y
 * @param &z Buffer for the return of the cartesian coordinate z
 * @return void
 */
void polar2Cartesian (float phi,
		      float theta,
		      float d,
		      float &x,
		      float &y,
		      float &z
		      ){

  // Conversion from degres to radian
  float radian_phi = phi* 3.14f / 180.0f;
  float radian_theta = theta* 3.14f / 180.0f;

  // If we had a coordinates system like we usually draw at math courses
  // x = d*sin(radian_phi)*cos(radian_theta);
  // y = d*sin(radian_phi)*sin(radian_theta);
  // z = d*cos(radian_phi);

  // This one is the correct, based in the documentation of OpenGL
  x = d*sin(radian_phi)*sin(radian_theta);
  y = d*cos(radian_phi);
  z = d*sin(radian_phi)*cos(radian_theta);
}

/**
 * Program instruction to be printed for user
 *
 * @return void
 */
void printUsage () {
  std::cerr << std::endl // send a line break to the standard error output
	    << appTitle << std::endl
	    << "Author : Tamy Boubekeur" << std::endl << std::endl
	    << "Usage : ./main [<file.off>]" << std::endl
	    << "Cammandes clavier :" << std::endl
	    << "------------------" << std::endl
	    << " ?: Print help" << std::endl
	    << " w: Toggle wireframe mode" << std::endl
	    << " <drag>+<left button>: rotate model" << std::endl
	    << " <drag>+<right button>: move model" << std::endl
	    << " <drag>+<middle button>: zoom" << std::endl
	    << " q, <esc>: Quit" << std::endl << std::endl;
}

/**
 * Fill a texture buffer of unsigned chars with a white/blue checkerboard.
 * The buffer size must be width*height*3.
 *
 * @param width Width of the texture buffer
 * @param height Height of the texture buffer
 * @param *image Pointer to the texture buffer
 * @return void
 */
void genCheckerboard (unsigned int width,
		      unsigned int height,
		      unsigned char *image
		      ){

  for (unsigned int y=0; y<height; y++){
    for (unsigned int x=0; x<width*3; x+=3){
      if (((y <= height/2) && (x <= width*3/2)) || ((y > height/2) && (x > width*3/2))){
	image[y*width*3 +x] = 0xff;
	image[y*width*3 +x +1] = 0xff;
	image[y*width*3 +x +2] = 0xff;
      }
      else{
	image[y*width*3 +x] = 0x00;
	image[y*width*3 +x +1] = 0x00;
	image[y*width*3 +x +2] = 0xff;
      }
    }
  }
}

/**
 * Initialization of the program. Principal function to be called at the
 * begining of main.
 *
 * @return void
 */
void init () {

  // OpenGL initialization
  // Specifies the faces to cull (here the ones pointing away from the camera)
  glCullFace (GL_BACK);
  // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
  glEnable (GL_CULL_FACE);
  // Specify the depth test for the z-buffer
  glDepthFunc (GL_LESS);
  // Enable the z-buffer in the rasterization
  glEnable (GL_DEPTH_TEST);
  // Set the width of edges in GL_LINE polygon mode
  glLineWidth (2.0);
  // Background color
  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
  glEnable (GL_LIGHTING);

  // Light Initialization
  GLfloat light_position0[4] = {10.0f, 10.0f, 10.0f, 1.0f};
  GLfloat light_position1[4] = {-10.0f, 0.0f, -1.0f, 1.0f};
  GLfloat light_position2[4] = {0.0f, 0.0f, 0.0f, 5.0f};
  GLfloat color0[4] = {1.0f, 0.9f, 0.8f, 1.0f};
  GLfloat color1[4] = {0.0f, 0.1f, 0.3f, 1.0f};
  GLfloat color2[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  glLightfv (GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv (GL_LIGHT1, GL_POSITION, light_position1);
  glLightfv (GL_LIGHT1, GL_POSITION, light_position2);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, color0);
  glLightfv (GL_LIGHT1, GL_DIFFUSE, color1);
  glLightfv (GL_LIGHT1, GL_DIFFUSE, color2);
  glLightfv (GL_LIGHT0, GL_SPECULAR, color0);
  glLightfv (GL_LIGHT1, GL_SPECULAR, color1);
  glLightfv (GL_LIGHT1, GL_SPECULAR, color2);
  glEnable (GL_LIGHT0);
  glEnable (GL_LIGHT1);
  glEnable (GL_LIGHT2);

  // Fill texture image
  genCheckerboard(TEXT_WIDTH, TEXT_HEIGHT, image);

  // Texture Parameters
  glEnable (GL_TEXTURE_2D);
  glGenTextures (1, &texture);
  glBindTexture (GL_TEXTURE_2D, texture);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Fill the texture with the information contained at the txture buffer
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, TEXT_WIDTH, TEXT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

  // Camera initialization
  fovAngle = 45.f;
  nearPlane = 0.01;
  farPlane = 10.0;
  camTargetX = 0.0;
  camTargetY = 0.0;
  camTargetZ = 0.0;

  phici = 90.0f;
  thetaci = 180.0f;
  rci = 5.0f;
}

/**
 * Function to be called many times inside display function.
 * Set a position to the camera based in the camera variables
 *
 * @return void
 */
void setupCamera () {

  // Set the projection matrix as current.
  // All upcoming matrix manipulations will affect it.
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  // Set the current projection matrix with the camera intrinsics
  gluPerspective (fovAngle, aspectRatio, nearPlane, farPlane);

  // Set the modelview matrix as current.
  // All upcoming matrix manipulations will affect it.
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  // Resposition the camera
  polar2Cartesian (phici, thetaci, rci, xci, yci, zci);
  xci += camTargetX;
  yci += camTargetY;
  zci += camTargetZ;
  // Set up the current modelview matrix with camera transform
  gluLookAt (xci, yci, zci, camTargetX, camTargetY, camTargetZ, 0.0, 1.0, 0.0);
}

/**
 * Function to be called many times inside motion  function.
 * Update the polar coordinates of the camera based on the passed offsets.
 *
 * @param deltaX Offset representing the mouse movement at the x-axe
 * @param deltaY Offset representing the mouse movement at the y-axe
 * @return void
 */
void updateCamera (float deltaX, float deltaY){

  if ((phici + deltaY) >= 180.0f)
    phici = 180.0f;
  else if ((phici + deltaY) <= 0.0f)
    phici = 0.1f;
  else
    phici+= deltaY;

  if ((thetaci + deltaX) >= 360.0f)
    thetaci = 360.0f;
  else if ((thetaci + deltaX) <= 0.0f)
    thetaci = 0.0f;
  else
    thetaci += deltaX;
}

void reshape (int w, int h) {
  screenWidth = w;
  screenHeight = h;
  aspectRatio = static_cast<float>(w)/static_cast<float>(h);
  glViewport (0, 0, (GLint)w, (GLint)h); // Dimension of the drawing region in the window
  setupCamera ();
}

/**
 * Draw a sphere based in the passed coordinates.
 *
 * @param x_center Cartesian coordiante of the sphere center at the x-axe
 * @param y_center Cartesian coordiante of the sphere center at the y-axe
 * @param z_center Cartesian coordiante of the sphere center at the z-axe
 * @param radius Radius of the sphere
 * @return void
 */
void glSphere(float x_center, float y_center, float z_center, float radius){

  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glTranslatef (x_center, y_center, z_center);

  glBegin (GL_TRIANGLES);
  float x,y,z;
  float x1,y1,z1;
  float x2,y2,z2;
  float x3,y3,z3;
  float step = 10;

  for (int phi =0; phi<180; phi+=step){
    for(int theta=0; theta<360; theta+=step){
      polar2Cartesian(phi, theta, radius, x, y, z);
      polar2Cartesian(phi, theta+ step, radius, x1, y1, z1);
      polar2Cartesian(phi +step, theta +step, radius, x2, y2, z2);
      polar2Cartesian(phi + step, theta, radius, x3, y3, z3);

      // glColor3f(x, y, z);
      glTexCoord2f(0.0, 1.0f);
      glNormal3f(x, y, z);
      glVertex3f(x,y ,z);

      // glColor3f(x1, y1, z1);
      glTexCoord2f(1.0f, 1.0f);
      glNormal3f(x1, y1, z1);
      glVertex3f(x1, y1, z1);

      // glColor3f(x2, y2, z2);
      glTexCoord2f(1.0f, 0.0f);
      glNormal3f(x2, y2, z2);
      glVertex3f(x2, y2, z2);

      // glColor3f(x, y, z);
      glTexCoord2f(0.0f, 1.0f);
      glNormal3f(x, y, z);
      glVertex3f(x, y, z);

      // glColor3f(x2, y2, z2);
      glTexCoord2f(1.0f, 0.0f);
      glNormal3f(x2, y2, z2);
      glVertex3f(x2, y2, z2);

      // glColor3f(x3, y3, z3);
      glTexCoord2f(0.0f, 0.0f);
      glNormal3f(x3, y3, z3);
      glVertex3f(x3, y3, z3);
    }
  }
  glEnd();

  glPopMatrix();
}

/**
 * Draw a colored sphere based in the passed coordinates/parameters.
 *
 * @param x Cartesian coordiante of the sphere center at the x-axe
 * @param y Cartesian coordiante of the sphere center at the y-axe
 * @param z Cartesian coordiante of the sphere center at the z-axe
 * @param r Radius of the sphere
 * @param difR Diffuse RED reflectance of the sphere (0.0f - 1.0f)
 * @param difG Diffuse GREEN reflectance of the sphere (0.0f - 1.0f)
 * @param difB Diffuse BLUE reflectance of the sphere (0.0f - 1.0f)
 * @param specR Specular RED reflectance of the sphere (0.0f - 1.0f)
 * @param specG Specular GREEN reflectance of the sphere (0.0f - 1.0f)
 * @param specB Specular BLUE reflectance of the sphere (0.0f - 1.0f)
 * @param shininess Shininess of the sphere (0.0f - 1.0f)
 * @return void
 */
void glSphereWithMat(float x,
		     float y,
		     float z,
		     float r,
		     float difR,
		     float difG,
		     float difB,
		     float specR,
		     float specG,
		     float specB,
		     float shininess
		     ){
  GLfloat material_color[4] = {difR, difG, difB, 1.0f};
  GLfloat material_specular[4] = {specR, specG, specB,1.0};
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, material_color);
  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);

  // Set the texture defined at init() to this drawing
  glBindTexture (GL_TEXTURE_2D, texture);

  glSphere(x, y, z, r);

}

/**
 * Draw a pyramid of 14 normal spheres with the given radius
 *
 * @param radius Radius of the spheres
 * @return void
 */
void glPyramid(float radius){

  // Draw the base of the pyramid
  for(float x= -2*radius; x<=2*radius; x+=2*radius)
    for(float z=-2*radius; z<=2*radius; z+=2*radius)
      glSphere(x, 0.0, z, radius);

  glSphere(radius,radius, radius, radius);
  glSphere(-radius,radius, -radius, radius);
  glSphere(-radius,radius, radius, radius);
  glSphere(radius,radius, -radius, radius);

  glSphere(0.0, 2*radius, 0.0, radius);
}

/**
 * Draw a pyramid of 14 colored spheres with the given radius
 *
 * @param radius Radius of the spheres
 * @return void
 */
void glPyramidWithMat(float radius){
  for(float x= -2*radius; x<=2*radius; x+=2*radius)
    for(float z=-2*radius; z<=2*radius; z+=2*radius)
      glSphereWithMat(x, 0.0f, z, radius, 0.0f, 0.0, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

  glSphereWithMat(radius,radius, radius, radius, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  glSphereWithMat(-radius,radius, -radius, radius, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  glSphereWithMat(-radius,radius, radius, radius, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  glSphereWithMat(radius,radius, -radius, radius, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);

  glSphereWithMat(0.0f, 2*radius, 0.0f, radius, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
}

/**
 * Callback funtion to refresh the display.
 *
 * @return void
 */
void display () {
  setupCamera ();
  // Erase the color and z buffers.
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Static 3D examples
  // glSphere(0.0f, 0.0f, 0.0f, 1.0f);
  // glPyramid(1.0f);
  // glSphereWithMat(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
  // glPyramidWithMat(0.5f);
  // glSphere(sin(angle), cos(angle), 0.0f, 1.0f);

  // Animation example: Orbit of ugly planets
  float deltaT = (currentTime- beforeTime);
  speed += deltaT*acceleration/100.0;
  if (speed >= 10.0f)
    speed = 10.0f;
  else if (speed <= -10.0f)
    speed = -10.0f;
  angle += (deltaT*speed)*(3.14/180.0)/100.0;

  glSphere(3*cos(0.7*angle), 0.0f, 3*sin(0.7*angle), 0.2f);
  glSphere(2*sin(0.5*angle), 0.0f, 2*cos(0.5*angle), 0.1f);
  glSphere(sin(angle), 0.0f, cos(angle), 0.5f);

  beforeTime = currentTime;

  glFlush (); // Ensures any previous OpenGL call has been executed
  glutSwapBuffers ();  // swap the render buffer and the displayed (screen) one
}

/**
 * Make something at a button press event
 *
 * @param keypressed The keyboard entry that generated the event
 * @param x
 * @param y
 * @return void
 */
void keyboard (unsigned char keyPressed, int x, int y) {
  switch (keyPressed) {
  case 'w':
    GLint mode[2];
    glGetIntegerv (GL_POLYGON_MODE, mode);
    glPolygonMode (GL_FRONT_AND_BACK, mode[1] ==  GL_FILL ? GL_LINE : GL_FILL);
    break;
  case '0':
    if(light0En){
      glDisable(GL_LIGHT0);
      light0En = 0;
    }
    else{
      glEnable(GL_LIGHT0);
      light0En = 1;
    }
    break;
  case '1':
    if(light1En){
      glDisable(GL_LIGHT1);
      light1En = 0;
    }
    else{
      glEnable(GL_LIGHT1);
      light1En = 1;
    }
    break;
  case '+':
    acceleration = 1.0f;
    break;
  case '-':
    acceleration = -1.0f;
    break;
  case 'q':
  case 27:
    exit (0);
    break;
  default:
    printUsage ();
    break;
  }
  glutPostRedisplay ();
}

/**
 * Make something at a mouse button press event
 *
 * @param button Indicates wich button has been pressed
 * @param x Position of the event at the x-axe
 * @param y Position of the event at the y-axe
 * @return void
 */
void mouse (int button, int state, int x, int y) {

  if (button == GLUT_LEFT_BUTTON){
    if(state == GLUT_DOWN){
      xmi= x;
      ymi= y;
      mouseClicked = 1;
    }
    else{
      mouseClicked = 0;
    }
  }
}

/**
 * Make something at a mouse mouvement event
 *
 * @param x Position of the event at the x-axe
 * @param y Position of the event at the y-axe
 * @return void
 */
void motion (int x, int y) {

  if(mouseClicked){
    // That was tricky... The y value received by this function decreases as
    // we go up and increases as we go down.
    updateCamera(x-xmi, ymi-y);
    xmi = x;
    ymi = y;
  }
}

/**
 * This function is executed in an infinite loop. It updated the window title
 * (frame-per-second, model size) and ask for rendering
 *
 * @return void
 */
void idle () {
  glutPostRedisplay();

  currentTime = glutGet((GLenum)GLUT_ELAPSED_TIME);

}

int main (int argc, char ** argv) {

  // Initialize a glut app
  glutInit (&argc, argv);
  // Setup a RGBA framebuffer to display, with a depth buffer (z-buffer),
  // in double buffer mode (fill a buffer then update the screen)
  glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  // Set the window app size on screen
  glutInitWindowSize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
  // create the window
  window = glutCreateWindow (appTitle.c_str ());
  // Your initialization code (OpenGL states, geometry, material, lights, etc)
  init ();
  // Callback function executed whenever glut need to setup the projection matrix
  glutReshapeFunc (reshape);
  // Callback function executed when the window app need to be redrawn
  glutDisplayFunc (display);
  // Callback function executed when the keyboard is used
  glutKeyboardFunc (keyboard);
  // Callback function executed when a mouse button is clicked
  glutMouseFunc (mouse);
  // Callback function executed when the mouse move
  glutMotionFunc (motion);
  // Callback function executed continuously when no other event happens
  // (good for background procesing or animation for instance).
  glutIdleFunc (idle);
  // By default, display the usage help of the program
  printUsage ();
  glutMainLoop ();
  return 0;
}
