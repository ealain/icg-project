// Glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "grid/grid.h"

#include "framebuffer.h"
#include "camera.h"
#include "noise/heightmap.h"
#include "water/water.h"

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
bool zooming;

vec3 movement;
bool mv_forward, mv_backward, mv_right, mv_left;

FrameBuffer fb_noise;
FrameBuffer fb_water;
Heightmap noise;

Grid grid;
Water water;


vec2 TransformScreenCoords(int x, int y);
void MouseButton(GLFWwindow* window, int button, int action, int mod);
void MousePos(GLFWwindow* window, double x, double y);
void SetupProjection(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void Init() {
    // Sets background color
    glClearColor(0.7, 0.7, 0.7 /*gray*/, 1.0 /*solid*/);

    int noise_texture_resolution_x = 1024;
    int noise_texture_resolution_y = 1024;
    GLuint noise_texture_id = fb_noise.Init(noise_texture_resolution_x,
        noise_texture_resolution_y, true);
    noise.Init(noise_texture_resolution_x,
        noise_texture_resolution_y);
           
    GLuint water_texture_id = fb_water.Init(noise_texture_resolution_x, 
        noise_texture_resolution_y, true);
    water.Init(512, noise_texture_resolution_x, 
        noise_texture_resolution_y, water_texture_id);
    

    // Light source position
    vec3 light_pos = vec3(-1.0f, 1.0f, 1.0f);

    grid.Init(512, noise_texture_id, light_pos);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

    view_matrix = camera.getViewMatrix();

    movement = vec3(0.0f, 0.0f, glfwGetTime());
    mv_forward = false;
    mv_backward = false;
    mv_right = false;
    mv_left = false;

    dragging = false;
    zooming = false;
}

// Gets called for every frame.
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fb_noise.Bind();
    float delta = glfwGetTime() - movement.z;
        vec3 view_dir = camera.getViewDirection();
    if(mv_forward)
        movement += vec3(view_dir.x * delta, -view_dir.z * delta, 0.0f);
    if(mv_backward)
        movement += vec3(-view_dir.x * delta, view_dir.z * delta, 0.0f);
    if(mv_right)
        movement += vec3(-cross(vec3(0.0f, 1.0f, 0.0f),
            vec3(view_dir.x, 0.0f, view_dir.z)).x * delta,
            cross(vec3(0.0f, 1.0f, 0.0f),
            vec3(view_dir.x, 0.0f, view_dir.z)).z * delta,
            0.0f);
    if(mv_left)
        movement += vec3(cross(vec3(0.0f, 1.0f, 0.0f),
            vec3(view_dir.x, 0.0f, view_dir.z)).x * delta,
            -cross(vec3(0.0f, 1.0f, 0.0f),
            vec3(view_dir.x, 0.0f, view_dir.z)).z * delta,
            0.0f);
    movement += vec3(0.0f, 0.0f, delta);
    
    noise.Draw(movement);
    
    fb_noise.Unbind();


    // Change view direction and / or zoom
    if(dragging || zooming) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        vec2 p = TransformScreenCoords(xpos, ypos);
        if(dragging)
            camera.Drag(p.x, p.y);
        if(zooming)
            camera.Zoom(p.y);
        view_matrix = camera.getViewMatrix();
    }

    glViewport(0, 0, window_width, window_height);
    grid.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);
    
    
    fb_water.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window_width, window_height);

    camera.invY();
    
    view_matrix = camera.getViewMatrix();
    
     
    grid.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);
    fb_water.Unbind();
    
    
    camera.invY();
    water.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);
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

    // Initialize our OpenGL program
    Init();

    // Update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window, &window_width, &window_height);
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
    if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)
	&& action == GLFW_PRESS) {
	double x_i, y_i;
	glfwGetCursorPos(window, &x_i, &y_i);
	vec2 p = TransformScreenCoords(x_i, y_i);
	camera.BeginDrag(p.x, p.y);
    }

    dragging = (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS);
    zooming = (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
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
	    cout << "Forward" << endl;
	    mv_forward = true;
	}
	else if (action == GLFW_RELEASE) {
	    mv_forward = false;
	}
	break;

    case 'S':
	if (action == GLFW_PRESS) {
	    cout << "Backward" << endl;
	    mv_backward = true;
	}
	else if (action == GLFW_RELEASE) {
	    mv_backward = false;
	}
	break;

    case 'A':
	if (action == GLFW_PRESS) {
	    cout << "Left" << endl;
	    mv_left = true;
	}
	else if (action == GLFW_RELEASE) {
	    mv_left = false;
	}
	break;

    case 'D':
	if (action == GLFW_PRESS) {
	    cout << "Right" << endl;
	    mv_right = true;
	}
	else if (action == GLFW_RELEASE) {
	    mv_right = false;
	}
	break;

    default:
	break;
    }
}
