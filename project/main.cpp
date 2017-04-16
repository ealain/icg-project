// Glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "grid/grid.h"

#include "framebuffer.h"
#include "trackball.h"
#include "noise/heightmap.h"

using namespace glm;

int window_width = 800;
int window_height = 600;

mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;
mat4 old_trackball_matrix;
mat4 quad_model_matrix;
float y_last;

vec3 light_pos;
vec3 eye;
vec3 target;

vec3 movement;
bool mv_forward, mv_backward, mv_right, mv_left;

FrameBuffer fb_noise;

Grid grid;

Trackball trackball;
Heightmap noise;

mat4 LookAt(vec3 eye, vec3 center, vec3 up);
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y);
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

    // Light source position
    vec3 light_pos = vec3(-1.0f, 1.0f, 1.0f);

    grid.Init(128, noise_texture_id, light_pos);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

    // Using the trackball requires a view matrix that looks straight down the -z axis.
    // Otherwise LookAt may be used.
    eye = vec3(0.0f, 1.0f, 3.0f);
    target = vec3(0.0f);
    view_matrix = LookAt(eye, target, vec3(0.0f, 1.0f, 0.0f));
    //view_matrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -4.0f));
    trackball_matrix = IDENTITY_MATRIX;

    movement = vec3(0.0f, 0.0f, glfwGetTime());
    mv_forward = false;
    mv_backward = false;
    mv_right = false;
    mv_left = false;
}

// Gets called for every frame.
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fb_noise.Bind();
    float delta = glfwGetTime() - movement.z;
    if(mv_forward)
	movement += vec3(-(target-eye).x * delta, -(target-eye).z * delta, 0.0f);
    if(mv_backward)
	movement += vec3((target-eye).x * delta, (target-eye).z * delta, 0.0f);
    if(mv_right)
	movement += vec3(-cross(vec3(0.0f, 1.0f, 0.0f),
			       vec3((target-eye).x, 0.0f, (target-eye).z)).x * delta,
			 -cross(vec3(0.0f, 1.0f, 0.0f),
			       vec3((target-eye).x, 0.0f, (target-eye).z)).z * delta,
			 0.0f);
    if(mv_left)
	movement += vec3(cross(vec3(0.0f, 1.0f, 0.0f),
			       vec3((target-eye).x, 0.0f, (target-eye).z)).x * delta,
			 cross(vec3(0.0f, 1.0f, 0.0f),
			       vec3((target-eye).x, 0.0f, (target-eye).z)).z * delta,
			 0.0f);
    movement += vec3(0.0f, 0.0f, delta);
    noise.Draw(movement);
    fb_noise.Unbind();
    // Draw a quad on the ground.
    glViewport(0, 0, window_width, window_height);
    grid.Draw(trackball_matrix, view_matrix, projection_matrix);
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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
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

mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    // Function that converts from world coordinates into camera coordinates.

    vec3 z_cam = normalize(eye - center);
    vec3 x_cam = normalize(cross(up, z_cam));
    vec3 y_cam = cross(z_cam, x_cam);

    mat3 R(x_cam, y_cam, z_cam);
    R = transpose(R);

    mat4 look_at(vec4(R[0], 0.0f),
		 vec4(R[1], 0.0f),
		 vec4(R[2], 0.0f),
		 vec4(-R * (eye), 1.0f));
    return look_at;
}

// Transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
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
	vec2 p = TransformScreenCoords(window, x_i, y_i);
	trackball.BeingDrag(p.x, p.y);
	old_trackball_matrix = trackball_matrix;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
	double x_i, y_i;
	glfwGetCursorPos(window, &x_i, &y_i);
	vec2 p = TransformScreenCoords(window, x_i, y_i);
	y_last = p.y;
    }
}

void MousePos(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
	vec2 p = TransformScreenCoords(window, x, y);
	// Calculate 'trackball_matrix' given the return value of
	// trackball.Drag(...) and the value stored in 'old_trackball_matrix'.
	// See also the mouse_button(...) function.
	trackball_matrix =  trackball.Drag(p.x, p.y, old_trackball_matrix) *old_trackball_matrix;
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
	// Implement zooming. When the right mouse button is pressed,
	// moving the mouse cursor up and down (along the screen's y axis)
	// should zoom out and it. For that you have to update the current
	// 'view_matrix' with a translation along the z axis.
	vec2 p = TransformScreenCoords(window, x, y);
	view_matrix[3][2] += 5 * (p.y - y_last);
	y_last = p.y;
    }
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
