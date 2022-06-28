// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include <string>
#include <unordered_map>
#include <memory>

#include "Graphics/bindable.hpp"
#include "Util/vector.hpp"

enum class uniform_variable_e : std::uint32_t {
    BOOL,
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    MAT2,
    MAT3,
    MAT4,
    SAMPLER2D,
    INVALID
};

struct uniform_variable_t {
    f32 min{0.001f}, max{1.0f}, step{0.1f};
    uniform_variable_e type{uniform_variable_e::INVALID};
    std::string name;
    std::shared_ptr<void> data; // cant be unique unfortunately
};

struct shader_stage_t {
    operator int() { return id; }

    explicit shader_stage_t(const std::string& path, const std::string& asset_dir);
    
    utl::vector<std::array<std::string, 7>> get_uniforms();
    GLint id;
    GLenum type;
    std::string code;
};

struct shader_t : bindable_i {
    std::string name;
    utl::vector<shader_stage_t> stages;
	mutable std::unordered_map<std::string, GLint> uniform_location_cache;
    utl::vector<std::string> files;

    //shader_t() = delete;
    virtual ~shader_t() { 
        stages.clear();
        glDeleteProgram(id); 
    };
    shader_t(){};
    shader_t(const std::string& p_name, const utl::vector<std::string>& files, const std::string& asset_dir);


	operator int() { return id; }

    void bind() override;
    void unbind() override;

    utl::vector<std::array<std::string, 7>> get_uniforms();
    utl::vector<uniform_variable_t> get_uniform_variables();
    void uniform_edit(utl::vector<uniform_variable_t>& uniforms);
    void set_uniforms(utl::vector<uniform_variable_t>& uniforms);
    

    GLint get_uniform_location(const std::string& name) const;
    // utility uniform functions
    // --------------------------------------------------------------------------
    void set_uniform_block(const std::string &name, int loc) const;
	void set_bool  (const std::string &name, bool  value) const;
	void set_int   (const std::string& name, int   value) const;
	void set_uint  (const std::string &name, unsigned int   value) const;
	void set_float (const std::string &name, float value) const;
	void set_vec2  (const std::string &name, float x, float y) const;
	void set_vec3  (const std::string &name, float x, float y, float z) const;
	void set_vec4  (const std::string &name, float x, float y, float z, float w);
	void set_vec2  (const std::string &name, const v2f &value) const;
	void set_vec3  (const std::string &name, const v3f &value) const;
	void set_vec4  (const std::string &name, const v4f &value) const;
	void set_mat2  (const std::string &name, const m22 &mat) const;
	void set_mat3  (const std::string &name, const m33 &mat) const;
	void set_mat4  (const std::string &name, const m44 &mat) const;
    void set_mat4v (const std::string &name, const m44& mat, int count) const;
	
    static void reset();
};

