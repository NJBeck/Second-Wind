#include "ShaderHandler.h"
#include "utility.h"
#include <fstream>

using std::string, std::unordered_map;

void ShaderHandler::Add(EntityID const handle,
                        VertexShader const vs, 
                        FragmentShader const fs)
{
    // make the shaders if they dont already exist
    // if NONE then skip
    auto& entity_data = index_[handle];
    if (vs != VertexShader::NONE) {
        auto& entity_vs = entity_data.vertex_shaders;
        entity_vs.emplace(vs);
        // if shader not yet compiled then do that
        auto found_vs = vertex_shaders_.find(vs);
        if (found_vs == vertex_shaders_.end()) {
            u32 shader_enum = static_cast<u32>(vs);
            MakeNewShader(  ShaderType::VERTEX, shader_enum);
        }
    }
    if (fs != FragmentShader::NONE) {
        auto& entity_fs = entity_data.fragment_shaders;
        entity_fs.emplace(fs);
        auto found_fs = fragment_shaders_.find(fs);
        if (found_fs == fragment_shaders_.end()) {
            u32 shader_enum = static_cast<u32>(fs);
            MakeNewShader(ShaderType::FRAGMENT, shader_enum);
        }
    }
}

void ShaderHandler::Remove( EntityID const handle,
                            VertexShader const vs,
                            FragmentShader const fs) 
{
    auto& entity_data = index_[handle];
    if (vs != VertexShader::NONE) {
        auto found_vs = entity_data.vertex_shaders.find(vs);
        if (found_vs == entity_data.vertex_shaders.end()) {
            utility::Log("ShaderHandler", "looking up", "vertex shader for entity");
        }
        else {
            entity_data.vertex_shaders.erase(vs);
            vertex_shaders_[vs].count--;
            if (vertex_shaders_[vs].count == 0) {
                DeleteShader(ShaderType::VERTEX, static_cast<u32>(vs));
            }
        }
    }
    if (fs != FragmentShader::NONE) {
        auto found_vs = entity_data.fragment_shaders.find(fs);
        if (found_vs == entity_data.fragment_shaders.end()) {
            utility::Log("ShaderHandler", "looking up", "fragment shader for entity");
        }
        else {
            entity_data.fragment_shaders.erase(fs);
            fragment_shaders_[fs].count--;
            if (fragment_shaders_[fs].count == 0) {
                DeleteShader(ShaderType::FRAGMENT, static_cast<u32>(fs));
            }
        }
    }
}

u32 ShaderHandler::GetActiveProgram(EntityID const handle)
{
    auto vs = index_[handle].active_vertex_shader;
    auto fs = index_[handle].active_fragment_shader;
    if (vs == VertexShader::NONE || fs == FragmentShader::NONE) {
        utility::Log("ShaderHandler", "getting program for ", "entity with no shader");
    }
    auto found_vs = shader_programs_.find(vs);
    auto found_fs = found_vs->second.find(fs);
    return found_fs->second.ID;
}

void ShaderHandler::SetActiveShader(EntityID const handle,
    VertexShader const vs,
    FragmentShader const fs)
{
    Add(handle, vs, fs);
    auto& entity_data = index_[handle];
    entity_data.active_vertex_shader = vs;
    entity_data.active_fragment_shader = fs;
    GLuint vs_id = vertex_shaders_[vs].ID;
    GLuint fs_id = fragment_shaders_[fs].ID;
    if (vs != VertexShader::NONE && fs != FragmentShader::NONE) {
        auto found_vs = shader_programs_.find(vs);
        // if no program corresponds to vs, fs pair then make one
        if (found_vs == shader_programs_.end()) {
            shader_programs_[vs][fs] = { MakeNewProgram(vs_id, fs_id), 1 };
        }
        else {
            auto fs_programs = found_vs->second;
            auto found_fs = fs_programs.find(fs);
            if (found_fs == fs_programs.end()) {
                shader_programs_[vs][fs] = { MakeNewProgram(vs_id, fs_id), 1 };
            }
        }
    }
}

std::string ShaderHandler::GetShaderPath(ShaderType const type_, u32 shader_enum)
{
    unordered_map<u32, string> vs_path{ {1, "quadshader.vs"}
    };
    unordered_map<u32, string> fs_path{ {1, "quadshader.fs"}
    };

    switch (type_) {
    case ShaderType::VERTEX:
        return utility::getDataPath(vs_path[shader_enum]);
        break;
    case ShaderType::FRAGMENT:
        return utility::getDataPath(fs_path[shader_enum]);
        break;
    }
}

GLuint ShaderHandler::MakeNewShader(ShaderType const sh_type, u32 const enum_value)
{
    // 1. retrieve the vertex/fragment source code from file
    std::string code;
    std::ifstream sh_fstream;
    // ensure ifstream objects can throw exceptions:
    sh_fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        string source_path = GetShaderPath(sh_type, enum_value);

        // open files
        sh_fstream.open(source_path);
        std::stringstream sh_str_stream;
        // read file's buffer contents into streams
        sh_str_stream << sh_fstream.rdbuf();
        // close file handler
        sh_fstream.close();
        // convert stream into string
        code = sh_str_stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* sh_code = code.c_str();

    switch (sh_type) {
    case ShaderType::VERTEX: {
        GLuint sh_ID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(sh_ID, 1, &sh_code, NULL);
        glCompileShader(sh_ID);
        VertexShader vs = static_cast<VertexShader>(enum_value);
        CheckCompileErrors(sh_ID, "VERTEX");
        vertex_shaders_[vs] = { sh_ID, 1 };
        return sh_ID;
    }
    break;

    case ShaderType::FRAGMENT: {
        GLuint sh_ID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(sh_ID, 1, &sh_code, NULL);
        glCompileShader(sh_ID);
        FragmentShader fs = static_cast<FragmentShader>(enum_value);
        CheckCompileErrors(sh_ID, "FRAGMENT");
        fragment_shaders_[fs] = { sh_ID, 1 };
        return sh_ID;
    }
            break;
    }
}

GLuint ShaderHandler::MakeNewProgram(GLuint const vs, GLuint const fs)
{
    auto ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");
    return ID;
}

void ShaderHandler::CheckCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}