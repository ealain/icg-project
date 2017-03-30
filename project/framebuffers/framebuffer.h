#pragma once
#include "icg_helper.h"

class FrameBuffer {
    // For frame buffers with one color attachment (with 1 component)

private:
    int width_;
    int height_;
    GLuint framebuffer_object_id_;
    GLuint color_texture_id_;
    GLuint program_id_;

public:
    // Warning: overrides viewport!!
    void Bind() {
	glViewport(0, 0, width_, height_);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1 /*length of buffers[]*/, buffers);
    }

    void Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    int Init(int image_width, int image_height, bool use_interpolation = false) {
	this->width_ = image_width;
	this->height_ = image_height;

	// Compile the shaders.
	program_id_ = icg_helper::LoadShaders("heightmap_vshader.glsl",
					      "heightmap_fshader.glsl");
	if(!program_id_) {
	    exit(EXIT_FAILURE);
	}

	glUseProgram(program_id_);

	// Create color attachment
	{
	    glGenTextures(1, &color_texture_id_);
	    glBindTexture(GL_TEXTURE_2D, color_texture_id_);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	    if(use_interpolation){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    } else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    }

	    // Create texture for the color attachment
	    // https://www.khronos.org/opengl/wiki/GLAPI/glTexImage2D
	    // One component (format GL_R32F) as suggested in slides
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width_, height_, 0,
			 GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}

	// Tie it all together
	{
	    glGenFramebuffers(1, &framebuffer_object_id_);
	    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
	    glFramebufferTexture2D(GL_FRAMEBUFFER,
				   GL_COLOR_ATTACHMENT0 /*location = 0*/,
				   GL_TEXTURE_2D, color_texture_id_,
				   0 /*level*/);

	    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		cerr << "!!!ERROR: Framebuffer not OK :(" << endl;
	    }
	    glBindFramebuffer(GL_FRAMEBUFFER, 0); // avoid pollution
	}

	// To prevent the current object from being polluted
	glBindVertexArray(0);
	glUseProgram(0);

	return color_texture_id_;
    }

    void Cleanup() {
	glDeleteTextures(1, &color_texture_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
	glDeleteFramebuffers(1, &framebuffer_object_id_);
    }
};
