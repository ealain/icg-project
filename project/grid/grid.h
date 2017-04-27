#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>


class Grid {

private:
    int grid_dim_;                          // Width/height of the grid

    GLuint vertex_array_id_;                // Vertex array object
    GLuint vertex_buffer_object_position_;  // Memory buffer for positions
    GLuint vertex_buffer_object_index_;     // Memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID
    GLuint texture_id_;                     // Texture ID
    GLuint num_indices_;                    // Number of vertices to render
    GLuint MVP_id_;                         // Model, view, proj matrix ID
    GLuint MV_id_;
    GLuint _grassTexId;
    GLuint _snowTexId;
    GLuint _sandTexId;
    GLuint _rockTexId;

    GLuint gen2DTexture(const char* imagePath, GLint format) {
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(textureId);

    int req_comp = (format == GL_RGB) ? STBI_rgb : STBI_rgb_alpha;

    int width, height, nb_comp;
    unsigned char* image = stbi_load(imagePath, &width, &height, &nb_comp, req_comp);
    if (image != NULL) {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

public:
    void Init(int grid_size, GLuint texture_id, glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 1.0f)) {
	grid_dim_ = grid_size;
	texture_id_ = texture_id;

	// Compile the shaders
	program_id_ = icg_helper::LoadShaders("grid_vshader.glsl", "grid_fshader.glsl");
	if(!program_id_) {
	    exit(EXIT_FAILURE);
	}

	glUseProgram(program_id_);

	// Vertex one vertex array
	glGenVertexArrays(1, &vertex_array_id_);
	glBindVertexArray(vertex_array_id_);

	// Vertex coordinates and indices
	{
	    std::vector<GLfloat> vertices;
	    std::vector<GLuint> indices;
	    // Make a triangle grid with dimension grid_dim_xgrid_dim_.
	    // always two subsequent entries in 'vertices' form a 2D vertex position.
	    for (float i = 0.0f; i < grid_dim_; i++){
		for(float j = 0.0f; j < grid_dim_; j++){
		    vertices.push_back((i/(grid_dim_ -1))*2.0f-1.0f);
		    vertices.push_back((j/(grid_dim_ -1))*2.0f-1.0f);
		}
	    }

	    for(int i = 0; i < grid_dim_ - 1 ; i++){
		for(int j = 0; j < grid_dim_ - 1; j++){
		    // First triangle
		    indices.push_back(j + i * grid_dim_);
		    indices.push_back(j + 1 + i * grid_dim_);
		    indices.push_back(j + grid_dim_ + i * grid_dim_);
		    // Second triangle
		    indices.push_back(j + 1 + i * grid_dim_);
		    indices.push_back(j + grid_dim_ + i * grid_dim_);
		    indices.push_back(j + grid_dim_ + 1 + i * grid_dim_);
		}
	    }

	    num_indices_ = indices.size();

	    // Position buffer
	    glGenBuffers(1, &vertex_buffer_object_position_);
	    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
	    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
			 &vertices[0], GL_STATIC_DRAW);

	    // Vertex indices
	    glGenBuffers(1, &vertex_buffer_object_index_);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
			 &indices[0], GL_STATIC_DRAW);

	    // Position shader attribute
	    GLuint loc_position = glGetAttribLocation(program_id_, "position");
	    glEnableVertexAttribArray(loc_position);
	    glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
				  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
	}

	// Other uniforms
	MVP_id_ = glGetUniformLocation(program_id_, "MVP");
	MV_id_ = glGetUniformLocation(program_id_, "MV");	

	GLuint light_pos_id = glGetUniformLocation(program_id_, "MV");
	glUniform3f(light_pos_id, (GLfloat)light_pos.x, (GLfloat)light_pos.y, (GLfloat)light_pos.z);

	// Bind Grass Texture
    _grassTexId = gen2DTexture("Texture/grass2.jpg", GL_RGB);
     // Bind Snow Texture
    _snowTexId = gen2DTexture("Texture/snow.jpg", GL_RGB);
     // Bind Sand Texture
    _sandTexId = gen2DTexture("Texture/sand.jpg", GL_RGB);
     // Bind Rock Texture
    _rockTexId = gen2DTexture("Texture/rock.jpg", GL_RGB);

		

	// To avoid the current object being polluted
	glBindVertexArray(0);
	glUseProgram(0);
    }

    void Cleanup() {
	glBindVertexArray(0);
	glUseProgram(0);
	glDeleteBuffers(1, &vertex_buffer_object_position_);
	glDeleteBuffers(1, &vertex_buffer_object_index_);
	glDeleteVertexArrays(1, &vertex_array_id_);
	glDeleteTextures(1, &texture_id_);
 	glDeleteTextures(1, &_snowTexId);
    glDeleteTextures(1, &_sandTexId);
    glDeleteTextures(1, &_grassTexId);
    glDeleteTextures(1, &_rockTexId);
    glDeleteProgram(program_id_);

    }

    void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
	      const glm::mat4 &view = IDENTITY_MATRIX,
	      const glm::mat4 &projection = IDENTITY_MATRIX, const glm::vec3 movement = glm::vec3(0,0,0)) {
	glUseProgram(program_id_);
	glBindVertexArray(vertex_array_id_);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glUniform1i(glGetUniformLocation(program_id_, "tex"), 0);
	//Grass
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _grassTexId);
    glUniform1i(glGetUniformLocation(program_id_, "grassTex"), 1);
    //Snow
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _snowTexId);
    glUniform1i(glGetUniformLocation(program_id_, "snowTex"), 2);
    //Sand
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _sandTexId);
    glUniform1i(glGetUniformLocation(program_id_, "sandTex"), 3);
    //Rock
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _rockTexId);
    glUniform1i(glGetUniformLocation(program_id_, "rockTex"), 4);

    //Set Offset position
    glUniform3f(glGetUniformLocation(program_id_, "movement"), movement.x, movement.y, movement.z);



	// Setup MVP
	glm::mat4 MVP = projection*view*model;
	glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));
	glm::mat4 MV = view*model;
	glUniformMatrix4fv(MV_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MV));


	// For debugging it can be helpful to draw only the wireframe
	// Do that by uncommenting the next line
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
    }
};
