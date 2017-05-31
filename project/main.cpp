// Glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "grid/grid.h"
#include "sky/skybox.h"
#include "water/water.h"
#include "shadow/shadow.hpp"

#include "framebuffer.h"
#include "camera.h"

#include "noise/heightmap.hpp"
#include "noise/movement.hpp"


#include "bezier/bezier.h"
#include "bezier/bezierCamera.h"

using namespace glm;

GLFWwindow* window;

int window_width = 800;
int window_height = 600;

mat4 projection_matrix;
mat4 view_matrix;
mat4 quad_model_matrix;
float y_last;

vec3 light_pos;

Camera camera;
bool dragging;

bool mv_forward, mv_backward, mv_right, mv_left;
bool turn_right, turn_left, turn_up, turn_down;
bool camera_forward, camera_backward;
bool fps_mode;

FrameBuffer fb_noise;
FrameBuffer fb_water;
FrameBuffer fb_shadow;
Heightmap noise;

Grid grid;
Skybox sky;
Water water;
Shadow shadow;

GLuint water_texture_id;
GLuint shadow_texture_id;
GLuint noise_texture_id;

int speed = 0; 
int controlSpeed = 1; 
BezierCamera BezierCam; 

vec3 shift = vec3(0.0, -0.1, 0.0);
int fogSelector = 0;

vec2 TransformScreenCoords(int x, int y);
void MouseButton(GLFWwindow* window, int button, int action, int mod);
void MousePos(GLFWwindow* window, double x, double y);
void SetupProjection(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


void Init() {
    // Sets background color
    glClearColor(0.7, 0.7, 0.7 /*gray*/, 1.0 /*solid*/);

    GLuint noise_texture_id = fb_noise.Init(1024, 1024, 1, true);
    noise.Init(1024, 1024);

    shadow_texture_id = fb_shadow.Init(1024, 1024, 2);
    shadow.Init(1024, 1024, shadow_texture_id);

    water_texture_id = fb_water.Init(window_width, window_height, 3, true);
    water.Init(512, 1024, 1024, water_texture_id, noise_texture_id);

    // Light source position
    vec3 light_pos = vec3(1.0f, 1.0f, 2.0f);

    grid.Init(512, noise_texture_id, light_pos);
    sky.Init(2.0f);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    view_matrix = camera.getViewMatrix();

    BezierCam.init(); 

    mv_forward = false;
    mv_right = false;
    mv_left = false;
    mv_right = false;

    turn_up = false;
    turn_down = false;
    turn_left = false;
    turn_right = false;

    dragging = false;
    camera_forward = false;
    camera_backward = false;

}

// Gets called for every frame.
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fb_noise.Bind();
    vec2 movement_offset = movement(camera.getViewDirection(), mv_right, mv_left,
				    mv_forward, mv_backward);

    noise.Draw(movement_offset);
    float altitude;
    vec2 terrain_offset = camera.getTerrainOffset();
    glReadPixels(512 + 50 * terrain_offset.x, 512 + 50 * terrain_offset.y,
		 1, 1, GL_RED, GL_FLOAT, &altitude);
    fb_noise.Unbind();

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    vec2 p = TransformScreenCoords(xpos, ypos);
    camera.Update(dragging, p, mv_forward, mv_backward, camera_forward, camera_backward,
		  turn_right, turn_left, turn_up, turn_down, fps_mode, altitude);

    if(BezierCam.get_bezier() && speed < BezierCam.get_indexMax() && speed >= BezierCam.get_indexMin()){
        BezierCam.BezCam(camera, speed); 
        speed += controlSpeed; 
        if (speed > BezierCam.get_indexMax()){
            BezierCam.changeBezier(); 
        }
    } 


    view_matrix = camera.getViewMatrix();

    // Draw a quad on the ground.
    glViewport(0, 0, window_width, window_height);
    grid.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix, movement_offset);

    sky.Draw(translate(IDENTITY_MATRIX, shift), view_matrix, projection_matrix);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fb_water.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    view_matrix = camera.invY();
    glViewport(0, 0, window_width, window_height);
    grid.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix, movement_offset, 0);
    sky.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);
    fb_water.Unbind();

    view_matrix = camera.getViewMatrix();

    glViewport(0, 0, window_width, window_height);
    water.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix, glfwGetTime());

    glDisable(GL_BLEND);
}


int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
	fprintf(stderr, "Failed to initialize GLFW\n");
	return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // Hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Attempt to open the window: fails if required version unavailable
    // Note some Intel GPUs do not support OpenGL 3.2
    // Note update the driver of your graphic card
    window = glfwCreateWindow(window_width, window_height,
			      "Terrain", NULL, NULL);
    if(!window) {
	glfwTerminate();
	return EXIT_FAILURE;
    }

    // Makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // Set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // Set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, SetupProjection);

    // Set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MousePos);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
	fprintf( stderr, "Failed to initialize GLEW\n");
	return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;


    // Update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    // Initialize our OpenGL program
    Init();


    SetupProjection(window, window_width, window_height);

    // Render loop
    while(!glfwWindowShouldClose(window)){
	Display();
	glfwSwapBuffers(window);
	glfwPollEvents();
    }

    grid.Cleanup();

    // Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

mat4 PerspectiveProjection(float fovy, float aspect, float near, float far) {
    // Create a perspective projection matrix given the field of view,
    // aspect ratio, and near and far plane distances.
    fovy = fovy*(M_PI/180);
    fovy = fovy/2.0f;
    mat4 projection = mat4(1.0f);

    float left = -sin(fovy)*near;
    float right = -left;
    float top = right/aspect;
    float bottom = -top;

    projection[0][0] = (2.0f*near) / (right - left);
    projection[1][1] = (2.0f*near) / (top - bottom);
    projection[2][2] = (-(far + near)) / (far - near);
    projection[3][3] = 0;
    projection[2][0] = (right + left) / (right - left);
    projection[2][1] = (top + bottom) / (top - bottom);
    projection[3][2] = (-2.0f*far*near) / (far - near);
    projection[2][3] = -1.0f;

    return projection;
}

// Transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(int x, int y) {
    // The framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f,
		1.0f - 2.0f * (float)y / height);
}

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
	double x_i, y_i;
	glfwGetCursorPos(window, &x_i, &y_i);
	vec2 p = TransformScreenCoords(x_i, y_i);
	camera.BeginDrag(p.x, p.y);
    }

    dragging = (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS);
}

void MousePos(GLFWwindow* window, double x, double y) {
}

// Gets called when the windows/framebuffer is resized.
void SetupProjection(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
	 << window_width << "x" << window_height << "." << endl;

    glViewport(0, 0, window_width, window_height);

    // Use a perspective projection instead;
    projection_matrix = PerspectiveProjection(45.0f, (GLfloat)window_width / window_height, 0.1f, 100.0f);

    water_texture_id = fb_water.Init(window_width,
				     window_height, 3, true);
    water.Init(512, 1024, 1024, water_texture_id, noise_texture_id);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch(key) {
    case GLFW_KEY_ESCAPE:
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
	    glfwSetWindowShouldClose(window, GL_TRUE);
	}
	break;

    case 'P':
	if (action == GLFW_REPEAT || action == GLFW_PRESS) {
	    cout << "Augmenting parameter" << endl;
	    noise.mod(0.1f);
	}
	break;

    case 'L':
	if (action == GLFW_REPEAT || action == GLFW_PRESS) {
	    cout << "Decreasing parameter" << endl;
	    noise.mod(-0.1f);
	}
	break;

    case 'W':
        if (action == GLFW_PRESS) {
            if(BezierCam.get_bezier()){
                controlSpeed += 1;
                cout << "ON AUGMENTE LA VITESSE " << endl; 
                if (controlSpeed > 4){
                    controlSpeed = 4; 
                }   
            }
            else {
                cout << "Forward" << endl;
                mv_forward = true;
            }
        } else if (action == GLFW_RELEASE) {
            mv_forward = false;
        }
    break;

    case 'S':
        if (action == GLFW_PRESS) {
            if(BezierCam.get_bezier()){
                controlSpeed -= 1; 
                if(controlSpeed < -4){
                    controlSpeed = -4; 
                }
            }
            else {
                cout << "Backward" << endl;
                mv_backward = true;
            }
        }
        else if (action == GLFW_RELEASE) {
            mv_backward = false;
        }
    break;

    case '3':
        if (action == GLFW_PRESS){
            BezierCam.changeMode(1); 
            speed = BezierCam.get_indexMin(); 

        }
        break; 
    case '4':
        if (action == GLFW_PRESS){
            cout << "change mode 2" << endl; 
            BezierCam.changeMode(2); 
            speed = BezierCam.get_indexMin(); 
        }
    break; 

    case 'A':
	if (action == GLFW_PRESS) {
	    cout << "Turn left" << endl;
	    turn_left = true;
	    double x_i, y_i;
	    glfwGetCursorPos(window, &x_i, &y_i);
	    vec2 p = TransformScreenCoords(x_i, y_i);
	    camera.BeginDrag(p.x, p.y);
	}
	else if (action == GLFW_RELEASE) {
	    turn_left = false;
	}
	break;

     case 'B':
    
    if (action == GLFW_PRESS) {
        cout << "Bezier Mode" << endl;
        speed = BezierCam.get_indexMin(); 
        
        controlSpeed = 1;
        BezierCam.changeBezier(); 
        cout << "Bezier : " << BezierCam.get_bezier() << endl; 
    }
    else if (action == GLFW_RELEASE) {
    }
    break;

    case 'D':
	if (action == GLFW_PRESS) {
	    cout << "Turn right" << endl;
	    turn_right = true;
	    double x_i, y_i;
	    glfwGetCursorPos(window, &x_i, &y_i);
	    vec2 p = TransformScreenCoords(x_i, y_i);
	    camera.BeginDrag(p.x, p.y);
	}
	else if (action == GLFW_RELEASE) {
	    turn_right = false;
	}
	break;

    case 'Q':
	if (action == GLFW_PRESS) {
	    cout << "Turn down" << endl;
	    turn_down = true;
	    double x_i, y_i;
	    glfwGetCursorPos(window, &x_i, &y_i);
	    vec2 p = TransformScreenCoords(x_i, y_i);
	    camera.BeginDrag(p.x, p.y);
	}
	else if (action == GLFW_RELEASE) {
	    turn_down = false;
	}
	break;

    case 'E':
	if (action == GLFW_PRESS) {
	    cout << "Turn up" << endl;
	    turn_up = true;
	    double x_i, y_i;
	    glfwGetCursorPos(window, &x_i, &y_i);
	    vec2 p = TransformScreenCoords(x_i, y_i);
	    camera.BeginDrag(p.x, p.y);
	}
	else if (action == GLFW_RELEASE) {
	    turn_up = false;
	}
	break;

    case 'Z':
	if (action == GLFW_PRESS) {
	    cout << "Left" << endl;
	    mv_left = true;
	}
	else if (action == GLFW_RELEASE) {
	    mv_left = false;
	}
	break;

    case 'C':
	if (action == GLFW_PRESS) {
	    cout << "Right" << endl;
	    mv_right = true;
	}
	else if (action == GLFW_RELEASE) {
	    mv_right = false;
	}
	break;

    case 'T':
	if (action == GLFW_PRESS) {
	    cout << "Camera forward" << endl;
	    camera_forward = true;
	    fps_mode = false;
	}
	else if (action == GLFW_RELEASE) {
	    camera_forward = false;
	}
	break;

    case 'G':
	if (action == GLFW_PRESS) {
	    cout << "Camera backward" << endl;
	    camera_backward = true;
	    fps_mode = false;
	}
	else if (action == GLFW_RELEASE) {
	    camera_backward = false;
	}
	break;

    case '1':
	if (action == GLFW_PRESS) {
	    cout << "FPS mode" << endl;
	    fps_mode = true;
	}
	break;

    case '2':
	if (action == GLFW_PRESS) {
	    cout << "Normal mode" << endl;
	    fps_mode = false;
	}
    
    case 'F':
    if (action == GLFW_PRESS) {
        water.toggleFog();
        grid.toggleFog();
        fogSelector = (fogSelector+1)%3;
        if (fogSelector == 0) {
            shift = vec3(0.0, -0.1, 0.0);
        } else {
            shift = vec3(0.0, 0.3, 0.0);
        }
        
    }
    default:
	break;
    }
}
