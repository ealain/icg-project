#include <glm/gtc/type_ptr.hpp>

// Vertex positions of the two triangles for texture quad
const GLfloat triangle_vertex_positions[] = {-1.0f, -1.0f, 0.0f,
                                             1.0f, -1.0f, 0.0f,
                                             -1.0f,  1.0f, 0.0f,
					     1.0f, 1.0f, 0.0f};

class Heightmap {

private:
    // The resolution is the "window_width" of the texture
    int resolution_;
    // The grid_dim determines the scale of the noise
    // Note : there is no real grid, it's an abstraction
    int grid_dim_;
    GLuint program_id_;
    GLuint vertex_array_id_;
    GLuint vertex_buffer_object_position_;
    GLuint vertex_buffer_object_index_;
    GLuint vertex_buffer_object_gradient_values_;

public:
    void Init(int resolution, int grid_dimen) {
	resolution_ = resolution;
	grid_dim_ = grid_dimen;
	
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
	// Gradient generation (§d+1^2 vec2)
	{
	    glm::vec2 values[10000];
	    srand(20);
	    for(int i = 0 ; i < (grid_dim_ + 1) * (grid_dim_ + 1) ; i++) {
		values[i] = glm::vec2((float)rand() / RAND_MAX * 2 - 1,
				      (float)rand() / RAND_MAX * 2 - 1);
	    }

	    GLint grad_values_id = glGetUniformLocation(program_id_, "grad_values");
	    glUniform2fv(grad_values_id, (grid_dim_ + 1) * (grid_dim_ + 1),
			 (GLfloat*)glm::value_ptr(values[0]));
	}

	// Data for the shader
	{
	    GLint grid_dim__id = glGetUniformLocation(program_id_, "grid_dim");
	    glUniform1f(grid_dim__id, (float)grid_dim_);
	    GLint ratio_id = glGetUniformLocation(program_id_, "ratio");
	    glUniform2f(ratio_id, (float)resolution_ / grid_dim_, (float)resolution_ / grid_dim_);
	}
    }

    void Draw() {
	// Uncomment to show the grid
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(program_id_);
	glBindVertexArray(vertex_array_id_);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);
    }
};
