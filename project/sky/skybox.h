#pragma once

#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

class Skybox {

private:
    GLuint program_id_;
    GLuint vertex_array_id_;
    GLuint vertex_buffer_object_;
    GLuint vertex_buffer_object_index_;
    GLuint texture_id_;
    glm::mat4 model_matrix_;

public:
    void Init(const GLfloat scale_box) {
	model_matrix_ = scale(IDENTITY_MATRIX, glm::vec3(scale_box));

	program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl", "skybox_fshader.glsl");
	glUseProgram(program_id_);

	glGenVertexArrays(1, &vertex_array_id_);
	glBindVertexArray(vertex_array_id_);

	//// Skybox vertices ////
	// 6 faces * 2 triangles * 3 vertices = 36 vertices to draw the box
	const GLfloat vertices[] = {-1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,
				    1.0f, -1.0f, -1.0f,    -1.0f, -1.0f, -1.0f,
				    -1.0f, 1.0f, -1.0f,     1.0f,  1.0f, -1.0f,
				    -1.0f, -1.0f, 1.0f,     1.0f, -1.0f,  1.0f,
				    1.0f, 1.0f, 1.0f,      -1.0f, -1.0f,  1.0f,
				    1.0f, 1.0f, 1.0f,      -1.0f,  1.0f,  1.0f,
				    -1.0f, 1.0f, 1.0f,      1.0f,  1.0f,  1.0f,
				    1.0f, 1.0f, -1.0f,     -1.0f,  1.0f,  1.0f,
				    1.0f, 1.0f, -1.0f,     -1.0f,  1.0f, -1.0f,
				    -1.0f, -1.0f, 1.0f,    -1.0f,  1.0f, -1.0f,
				    -1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,
				    -1.0f, 1.0f, 1.0f,     -1.0f,  1.0f, -1.0f,
				    1.0f, -1.0f, 1.0f,      1.0f,  1.0f,  1.0f,
				    1.0f, 1.0f, -1.0f,      1.0f, -1.0f, -1.0f,
				    1.0f, -1.0f, 1.0f,      1.0f,  1.0f, -1.0f,
				    -1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,
				    -1.0f, -1.0f, 1.0f,    -1.0f, -1.0f, -1.0f,
				    1.0f, -1.0f, -1.0f,     1.0f, -1.0f,  1.0f};

	glGenBuffers(1, &vertex_buffer_object_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Send position of vertices to the vertex shader
	GLuint position_loc = glGetAttribLocation(program_id_, "position");
	glEnableVertexAttribArray(position_loc);
	glVertexAttribPointer(position_loc, 3, GL_FLOAT, DONT_NORMALIZE,
			      ZERO_STRIDE, ZERO_BUFFER_OFFSET);

	//// Load texture ////
	const string files[] = {"textures/sky/sunny_day_rt.jpeg",
				"textures/sky/sunny_day_lf.jpeg",
				"textures/sky/sunny_day_up.jpeg",
				"textures/sky/sunny_day_dn.jpeg",
				"textures/sky/sunny_day_bk.jpeg",
				"textures/sky/sunny_day_ft.jpeg"};

	glGenTextures(1, &texture_id_);
	int width, height, nb_component;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
	for(int i = 0; i < 6; i++)
	    {
		image = stbi_load(files[i].c_str(), &width, &height, &nb_component, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
			     width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	    }
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glUniform1i(glGetUniformLocation(program_id_, "tex"), GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glBindVertexArray(0);
	glUseProgram(0);
    }

    void Cleanup() {
	glBindVertexArray(0);
	glUseProgram(0);
	glDeleteBuffers(1, &vertex_buffer_object_);
	glDeleteBuffers(1, &vertex_buffer_object_index_);
	glDeleteTextures(1, &texture_id_);
	glDeleteProgram(program_id_);
	glDeleteVertexArrays(1, &vertex_array_id_);
    }

    void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
	      const glm::mat4 &view = IDENTITY_MATRIX,
	      const glm::mat4 &projection = IDENTITY_MATRIX) {

	glUseProgram(program_id_);
	glBindVertexArray(vertex_array_id_);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

	glm::mat4 MVP = projection * view * model * model_matrix_;
	GLuint model_matrix_id = glGetUniformLocation(program_id_, "model_matrix");
	glUniformMatrix4fv(model_matrix_id, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glBindVertexArray(0);
	glUseProgram(0);
    }
};
