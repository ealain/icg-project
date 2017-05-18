#include <glm/gtc/type_ptr.hpp>

class Shadow {
private:
    int resolution_x_, resolution_y_;
    int grid_dim_;

    GLuint texture_shadow_;
    GLuint vertex_array_id_;
    GLuint vertex_buffer_object_position_;
    GLuint vertex_buffer_object_index_;
    GLuint program_id_;
    GLuint num_indices_;
    GLuint MVP_id_;

public:
    void Init(int resolution_x, int resolution_y, GLuint texture_shadow) {
	texture_shadow_ = texture_shadow;
	resolution_x_ = resolution_x;
	resolution_y_ = resolution_y;

        // Compile the shaders
	program_id_ = icg_helper::LoadShaders("shadow_vshader.glsl",
					      "shadow_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

	// Setup uniform
        MVP_id_ = glGetUniformLocation(program_id_, "MVP");

        GLuint loc_tex_shadow = glGetUniformLocation(program_id_, "shadow_tex");
        glUniform1i(loc_tex_shadow, 1);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteProgram(program_id_);
        glDeleteTextures(1, &texture_shadow_);
    }

    void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
	      const glm::mat4 &view = IDENTITY_MATRIX,
	      const glm::mat4 &projection = IDENTITY_MATRIX, float time = 0) {

        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

	// Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_shadow_);

        // Setup MVP
        glm::mat4 MVP = projection*view*model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
