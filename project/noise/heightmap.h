#include <glm/gtc/type_ptr.hpp>

#define GRID_DIM 3           // Size of the grid (unit: nb of stripes)
#define HIGH_FREQUENCY 255   // Size of the grid (unit: nb of stripes)
#define NB_GRADIENTS 53      // Prime number

// Vertex positions of the two triangles for texture quad
const GLfloat triangle_vertex_positions[] = {-1.0f, -1.0f, 0.0f,
                                             1.0f, -1.0f, 0.0f,
                                             -1.0f,  1.0f, 0.0f,
					     1.0f, 1.0f, 0.0f};

class Heightmap {

private:
    int resolution_x_, resolution_y_;  // The resolution is the "window_width" of the texture
    int grid_dim_;                     // The grid_dim determines the lower frequency of the noise
    int f_max_;                        // Higher frequency of the noise

    GLuint program_id_;
    GLuint vertex_array_id_;
    GLuint vertex_buffer_object_position_;
    GLuint vertex_buffer_object_index_;
    GLuint vertex_buffer_object_gradient_values_;

public:
    void Init(int resolution_x, int resolution_y) {
	resolution_x_ = resolution_x;
	resolution_y_ = resolution_y;

	// Compile the shaders
	program_id_ = icg_helper::LoadShaders("heightmap_vshader.glsl",
					      "heightmap_fshader.glsl");
	if(!program_id_) {
	    exit(EXIT_FAILURE);
	}

	glUseProgram(program_id_);

	// Setup vertex array;
	// Vertex arrays wrap buffers & attributes together
	glGenVertexArrays(ONE, &vertex_array_id_);
	glBindVertexArray(vertex_array_id_);

	// Generate memory for vertex buffer
	GLuint vertex_buffer;
	glGenBuffers(ONE, &vertex_buffer);
	// The subsequent commands will affect the specified buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	// Pass the vertex positions to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertex_positions),
		     triangle_vertex_positions, GL_STATIC_DRAW);

	GLuint loc_position = glGetAttribLocation(program_id_, "position");
	glEnableVertexAttribArray(loc_position);
	glVertexAttribPointer(loc_position, 3, GL_FLOAT, DONT_NORMALIZE,
			      ZERO_STRIDE, ZERO_BUFFER_OFFSET);

	gen_gradients();

	glBindVertexArray(0);
	glUseProgram(0);
    }

    void gen_gradients() {
	// Gradient generation (NB_GRADIENTS vec2s)
	{
	    glm::vec2 values[NB_GRADIENTS];
	    srand(20);
	    for(int i = 0 ; i < NB_GRADIENTS ; i++) {
		values[i] = glm::vec2((float)rand() / RAND_MAX * 2 - 1,
				      (float)rand() / RAND_MAX * 2 - 1);
	    }

	    GLint grad_values_id = glGetUniformLocation(program_id_, "grad_values");
	    glUniform2fv(grad_values_id, NB_GRADIENTS,
			 (GLfloat*)glm::value_ptr(values[0]));
	}

	// Data for the shader
	{
	    GLint grid_dim_id_ = glGetUniformLocation(program_id_, "grid_dim");
	    glUniform1f(grid_dim_id_, (float)GRID_DIM);
	    GLint fmax_id_ = glGetUniformLocation(program_id_, "fmax");
	    glUniform1i(fmax_id_, HIGH_FREQUENCY);
	    GLint ratio_id = glGetUniformLocation(program_id_, "ratio");
	    glUniform2f(ratio_id, (float)resolution_x_ / (float)HIGH_FREQUENCY,
			(float)resolution_y_ / (float)HIGH_FREQUENCY);
	}
    }

    void Draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(program_id_);
	glBindVertexArray(vertex_array_id_);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);
    }
};
