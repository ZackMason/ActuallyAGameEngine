// Author: Zackery Mason-Blaug
// Date: 2022-06-10
//////////////////////////////////////////////////////////


#include "Graphics/compute.hpp"

#include <glad/glad.h>

#include "core.hpp"

#include "Util/exceptions.hpp"


void texture_compute_t::bind_buffer(unsigned int buffer) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, buffer_count++, buffer);
}

void texture_compute_t::execute(unsigned int count) {
    shader.bind();
    shader.set_int("uInput", 0);
    shader.set_int("uOutput", 1);
    //input.slot = 0;
    output.slot = 1;
    //input.bind_image();
    output.bind_image();

    glDispatchCompute(output.width, output.height, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    buffer_count = 0;
    
    shader_t::reset();
}
texture_compute_t::texture_compute_t(const std::string& file, int w, int h) 
: shader(file, {file}), output(w,h) {
}
#if 0
auto foo(auto& file) {
    std::string code;
	std::ifstream shaderFile;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	std::string path = fmt::format("{}shaders/{}.cs", ASSETS_PATH, file);

	try	{
		// open 
		shaderFile.open(path);
		std::stringstream shaderStream;
		// read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		// close file handlers
		shaderFile.close();
		// convert stream into string
		code = shaderStream.str();
	} catch (std::ifstream::failure &e){
	    throw runtime_error_x(fmt::format("Compute Shader: {}.cs - {}", file, e.what()));
	}
	const char* shaderCode = code.c_str();

	int shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
	    throw runtime_error_x(fmt::format("Compute Shader Compiling: {}.cs - {}", file, infoLog));
	}
	id = glCreateProgram();
	glAttachShader(id, shader);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
	    throw runtime_error_x(fmt::format("Compute Shader Linking: {}.cs - {}", file, infoLog));
	}
	else
	{
		fmt::print(fg(fmt::color::purple) | fmt::emphasis::bold, "Compute Shader Loaded: {}\n", file);
	}
	glDeleteShader(shader);
}

#endif
