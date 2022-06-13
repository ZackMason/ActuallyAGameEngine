// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#include "Graphics/shader.hpp"

#include "Util/exceptions.hpp"

#include "core.hpp"

#include <regex>

#include <fmt/core.h>
#include <fmt/color.h>

namespace internal {
  decltype(shader_t::id) bound_shader = -1;  

    void include_preprocess(std::string& code){
        static const std::regex r("(#include <)([a-zA-Z]+)(\.glsl>)");
        std::smatch m;
        while (std::regex_search(code, m, r))
        {
            std::ifstream file;
            std::stringstream code_stream;
            file.open(fmt::format("{}shaders/{}.glsl", ASSETS_PATH, (std::string)m[2]));
            if (file.is_open())
            {
                code_stream << file.rdbuf();
                file.close();
                std::regex file_reg("(#include <)" + (std::string)m[2] + "(\.glsl>)");
                code = std::regex_replace(code, file_reg, code_stream.str());
            } else {
                throw runtime_error_x(fmt::format("Shader Preprocess: file {}.glsl not found", (std::string)m[2]));
            }
        }
    }

};

void shader_t::reset() {
    internal::bound_shader = -1;
}

void shader_t::bind() {
    if (internal::bound_shader != id) {
        internal::bound_shader = id;
        glUseProgram(id);
    }
}
void shader_t::unbind() {
    if (internal::bound_shader != 0) {
        internal::bound_shader = 0;
        glUseProgram(id);
    }
}

shader_stage_t::shader_stage_t(const std::string& path) {
    if (path.substr(path.find_last_of(".") + 1) == "vs")
    {
        type = GL_VERTEX_SHADER;
    }
    else if (path.substr(path.find_last_of(".") + 1) == "fs")
    {
        type = GL_FRAGMENT_SHADER;
    }
    else if (path.substr(path.find_last_of(".") + 1) == "gs")
    {
        type = GL_GEOMETRY_SHADER;
    }
    else if (path.substr(path.find_last_of(".") + 1) == "cs")
    {
        type = GL_COMPUTE_SHADER;
    }
    else
    {
        throw not_implemented_x();
    }

    std::string shader_code;
    std::ifstream shader_file;
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shader_file.open(fmt::format("{}{}", ASSETS_PATH, path));
        std::stringstream shader_stream;
        shader_stream << shader_file.rdbuf();
        shader_file.close();
        shader_code = shader_stream.str();
        internal::include_preprocess(shader_code);
        //IncludePreprocess(shader_code);
    } catch (std::ifstream::failure & e) {
        throw runtime_error_x(fmt::format("Shader stage error: {} - {}\n", path, e.what()));
    }

    const char* vShaderCode = shader_code.c_str();
    code = shader_code;
    
    int shader_slot = glCreateShader(type);
    glShaderSource(shader_slot, 1, &vShaderCode, NULL);
    glCompileShader(shader_slot);
    // check for shader compile errors
    int success;
    glGetShaderiv(shader_slot, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader_slot, 512, NULL, infoLog);
        throw runtime_error_x(fmt::format("Shader stage error: {} - {} - {}\n", path, type, infoLog));
    }
    else
    {
        id = shader_slot;
    }
}

utl::vector<std::array<std::string, 7>> shader_stage_t::get_uniforms(){
    std::smatch m;
    static std::regex e(R"rgx(^uniform (\w+) (\w+);\s*\/\/\s*(?:auto|export)\s*(\[\s*([-+]?[0-9]*\.?[0-9]+)\s*,\s*([-+]?[0-9]*\.?[0-9]+)\s*,\s*([-+]?[0-9]*\.?[0-9]+)\s*\])?\s*([-+]?[0-9]*\.?[0-9]+)?)rgx");
    utl::vector<std::array<std::string, 7>> res;
    
    auto code_copy = code;
    
    while (std::regex_search(code_copy, m, e))
    {
        res.push_back({ m[0], m[1], m[2], m[4], m[5], m[6], m[7] });
        
        code_copy = m.suffix().str();
    }
    return res;
}


shader_t::shader_t(const std::string& p_name, const std::vector<std::string>& p_files)
    : name(p_name), files(p_files)
{
    id = glCreateProgram();
    for (const auto& file : files)
    {
        // todo load from cache
        stages.emplace_back(file);
        glAttachShader(id, stages.back().id);
    }
    int success;
    
    glLinkProgram(id);
    
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success)
    {
        char errorLog[512];
        glGetProgramInfoLog(id, 512, nullptr, errorLog);
        throw runtime_error_x(fmt::format("Shader error: {} - {}", name, errorLog));
    }
    else
    {
        fmt::print(fg(fmt::color::green) | fmt::emphasis::bold,
            "Shader loaded: {}\n", name);
    }
}

GLint shader_t::get_uniform_location(const std::string& name) const {
    if (uniform_location_cache.count(name))
        return uniform_location_cache[name];
    GLint location = glGetUniformLocation(id, name.c_str());
    uniform_location_cache[name] = location;
    return location;
}


utl::vector<std::array<std::string, 7>> shader_t::get_uniforms()
{
    utl::vector<std::array<std::string, 7>> res;
    for (auto stage : stages)
    {
        auto r = stage.get_uniforms();
        res.insert(res.end(), r.begin(), r.end());
    }
    return res;
}

utl::vector<uniform_variable_t> shader_t::get_uniform_variables()
{
    utl::vector<uniform_variable_t> result;
    
    for(const auto& uniform_array : get_uniforms())
    {
        const auto& [full, type, name, min, step, max, start] = uniform_array;
        uniform_variable_t var;
        var.name = name;
        
        if (min != "")
            var.min = std::stof(min);
        if (max != "")
            var.max = std::stof(max);
        if (step != "")
            var.step = std::stof(step);
        
        if (type == "bool") {
            var.data = std::make_shared<bool>(start != "" ? std::stoi(start) : false);
            var.type = uniform_variable_e::BOOL;
            
        }
        else if (type == "int") {
            var.data = std::make_shared<int>(start != "" ? std::stoi(start) : 1);
            var.type = uniform_variable_e::INT;
        }
        else if (type == "float") {
            var.data = std::make_shared<float>(start != "" ? std::stof(start) : 1.0f);
            var.type = uniform_variable_e::FLOAT;
        }
        else if (type == "vec3") {
            var.data = std::make_shared <v3f>(start != "" ? std::stof(start) : 0.0f);
            var.type = uniform_variable_e::VEC3;
        }
        else if (type == "vec4") {
            var.data = std::make_shared<v4f>(start != "" ? std::stof(start) : 0.0f);
            var.type = uniform_variable_e::VEC4;
        }
        else if (type == "vec2") {
            var.data = std::make_shared<v2f>(start != "" ? std::stof(start) : 0.0f);
            var.type = uniform_variable_e::VEC2;
        }
        else {
            var.type = uniform_variable_e::INVALID;
            
        }
        
        result.push_back(var);
    }
    return result;
}

#if 0
void shader_t::uniform_edit(std::vector<UniformVariable>& uniforms)
{
    for (auto& uniform : uniforms)
    {
        switch (uniform.type)
        {
            case uniform_variable_e::BOOL:
            ImGui::Checkbox(uniform.name.c_str(), (bool*)uniform.data.get());
            break;
            case uniform_variable_e::INT:
            ImGui::DragInt(uniform.name.c_str(), (int*)uniform.data.get(), uniform.step, uniform.min, uniform.max );
            break;
            case uniform_variable_e::FLOAT:
            ImGui::DragFloat(uniform.name.c_str(), (f32*)uniform.data.get(), uniform.step, uniform.min, uniform.max);
            break;
            case uniform_variable_e::VEC3:
            ImGui::DragFloat3(uniform.name.c_str(), glm::value_ptr(*(glm::vec3*)uniform.data.get()));
            break;
            case uniform_variable_e::VEC4:
            ImGui::ColorEdit4(uniform.name.c_str(), glm::value_ptr(*(glm::vec4*)uniform.data.get()));
            break;
            default:
            ImGui::Text("Unimplemented uniform: %s", uniform.name.c_str());
            break;
        }
    }
}
#endif

void shader_t::set_uniforms(utl::vector<uniform_variable_t>& uniforms)
{
    for (auto& uniform : uniforms)
    {
        switch (uniform.type)
        {
            case uniform_variable_e::BOOL:
            set_bool(uniform.name, *(bool*)uniform.data.get());
            break;
            case uniform_variable_e::INT:
            set_int(uniform.name, *(int*)uniform.data.get());
            break;
            case uniform_variable_e::FLOAT:
            set_float(uniform.name, *(f32*)uniform.data.get());
            break;
            case uniform_variable_e::VEC3:
            set_vec3(uniform.name, *(glm::vec3*)uniform.data.get());
            break;
            case uniform_variable_e::VEC4:
            set_vec4(uniform.name, *(glm::vec4*)uniform.data.get());
            break;
            default:
            break;
        }
    }
}

void shader_t::set_bool(const std::string &name, bool value) const
{
	glUniform1i(get_uniform_location(name), (int)value);
}
// ------------------------------------------------------------------------
void shader_t::set_int(const std::string &name, int value) const
{
	glUniform1i(get_uniform_location(name), value);
}
// ------------------------------------------------------------------------
void shader_t::set_uint(const std::string& name, unsigned int value) const
{
	glUniform1ui(get_uniform_location(name), value);
}
// ------------------------------------------------------------------------
void shader_t::set_float(const std::string &name, float value) const
{
	glUniform1f(get_uniform_location(name), value);
}
// ------------------------------------------------------------------------
void shader_t::set_vec2(const std::string &name, const v2f &value) const
{
	glUniform2fv(get_uniform_location(name), 1, &value[0]);
}
void shader_t::set_vec2(const std::string &name, float x, float y) const
{
	glUniform2f(get_uniform_location(name), x, y);
}
// ------------------------------------------------------------------------
void shader_t::set_vec3(const std::string &name, const v3f &value) const
{
	glUniform3fv(get_uniform_location(name), 1, &value[0]);
}
void shader_t::set_vec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(get_uniform_location(name), x, y, z);
}
// ------------------------------------------------------------------------
void shader_t::set_vec4(const std::string &name, const v4f &value) const
{
	glUniform4fv(get_uniform_location(name), 1, &value[0]);
}
void shader_t::set_vec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(get_uniform_location(name), x, y, z, w);
}
// ------------------------------------------------------------------------
void shader_t::set_mat2(const std::string &name, const m22 &mat) const
{
	glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void shader_t::set_mat3(const std::string &name, const m33 &mat) const
{
	glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void shader_t::set_mat4(const std::string &name, const m44 &mat) const
{
	glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
}
