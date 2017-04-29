#include <glm/gtc/type_ptr.hpp>

class Heightmap {
private:
    
public:
    void Init(int resolution_x, int resolution_y) {
        resolution_x_ = resolution_x;
    	resolution_y_ = resolution_y;
        
        // Compile the shaders
    	program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
            "water_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

    	glUseProgram(program_id_);
        
        // Setup vertex array;
    	// Vertex arrays wrap buffers & attributes together
    	glGenVertexArrays(ONE, &vertex_array_id_);
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
        glDeleteTextures(1, &texture_id_);
    }
    
    void Draw() {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_):
        
        glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        glUseProgram(0);
    }
}