#include "ShaderHandler.hpp"
#include "utility.h"

using std::string;

void ShaderHandler::Add(EntityID const handle, 
                        VertexShader const vs, 
                        FragmentShader const fs)
{
    // make the shaders if they dont already exist
    // if NONE then skip
    if (vs != VertexShader::NONE) {
        auto& entity_data = index_[handle];
        auto& entity_vs = entity_data.vertex_shaders;
        // if not yet registered to entity then add it
        auto found_entity_vs = entity_vs.find(vs);
        if (found_entity_vs == entity_vs.end()) {
            // if shader not yet compiled then do that first
            auto& found_vs = vertex_shaders_.find(vs);
            if (found_vs == vertex_shaders_.end()) {
                u32 shader_enum = static_cast<u32>(vs);
                vertex_shaders_[vs] = { MakeNewShader(  ShaderType::VERTEX,
                                                        shader_enum), 0 };
            }
            found_vs->second.count++;
        }
        entity_vs.insert(vs);
    }
    if (fs != FragmentShader::NONE) {
        auto& entity_fs = index_[handle].fragment_shaders;
        // if not yet registered to entity then add it
        auto found_entity_fs = entity_fs.find(fs);
        if (found_entity_fs == entity_fs.end()) {
            // if shader not yet compiled then do that first
            auto& found_fs = fragment_shaders_.find(fs);
            if (found_fs == fragment_shaders_.end()) {
                u32 shader_enum = static_cast<u32>(fs);
                fragment_shaders_[fs] = { MakeNewShader(ShaderType::FRAGMENT, 
                                                        shader_enum), 0 };
            }
            found_fs->second.count++;
        }
        entity_fs.insert(fs);
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
    return program_matrix_(static_cast<u32>(vs), static_cast<u32>(fs)).ID;
}

void ShaderHandler::SetActiveShader(EntityID const handle,
    VertexShader const vs,
    FragmentShader const fs)
{
    Add(handle, vs, fs);
    auto& entity_data = index_[handle];
    auto& active_vs = entity_data.active_vertex_shader;
    auto& active_fs = entity_data.active_fragment_shader;
    active_vs = vs;
    active_fs = fs;
    if (vs != VertexShader::NONE && fs != FragmentShader::NONE) {
        u32 vs_num = static_cast<u32>(vs);
        u32 fs_num = static_cast<u32>(fs);
        auto& ID = program_matrix_(vs_num, fs_num).ID;
        auto& count = program_matrix_(vs_num, fs_num).count;
        if (count == 0) {
            if (vertex_shaders_[vs].count == 0) {
                MakeNewShader(ShaderType::VERTEX, vs_num);
            }
            if (fragment_shaders_[fs].count == 0) {
                MakeNewShader(ShaderType::FRAGMENT, fs_num);
            }
            u32 vs_ID = vertex_shaders_[vs].ID;
            u32 fs_ID = fragment_shaders_[fs].ID;
            auto new_ID = glCreateProgram();
            glAttachShader(new_ID, vs_ID);
            glAttachShader(new_ID, fs_ID);
            glLinkProgram(new_ID);
            CheckCompileErrors(ID, "PROGRAM");
            ID = new_ID;
        }
        count++;
    }
}

std::string ShaderHandler::GetFragShaderPath(FragmentShader frag)
{
    switch (frag) {
    case FragmentShader::QUAD: {
        return utility::getDataPath("quadshader.fs");
    }
    }
}

std::string ShaderHandler::GetVertShaderPath(VertexShader vert) {
    switch (vert) {
    case VertexShader::QUAD: {
        return utility::getDataPath("quadshader.vs");
    }
    }
}

GLuint ShaderHandler::MakeNewShader(ShaderType const sh_type, u32 const enum_value)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string code;
    std::ifstream sh_fstream;
    // ensure ifstream objects can throw exceptions:
    sh_fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        string source;
        switch (sh_type) {
            case ShaderType::VERTEX:

                source = GetVertShaderPath(static_cast<VertexShader>(enum_value));
                break;

            case ShaderType::FRAGMENT:
                source = GetFragShaderPath(static_cast<FragmentShader>(enum_value));
                break;
        }

        // open files
        sh_fstream.open(source);
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
    // 2. compile shader
    u32 sh_ID;
    // vertex shader
    sh_ID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sh_ID, 1, &sh_code, NULL);
    glCompileShader(sh_ID);
    // check and store

    switch (sh_type) {
        case ShaderType::VERTEX:
            VertexShader vs = static_cast<VertexShader>(enum_value);
            CheckCompileErrors(sh_ID, "VERTEX");
            vertex_shaders_[vs] = { sh_ID, 1 };
            break;

        case ShaderType::FRAGMENT:
            FragmentShader fs = static_cast<FragmentShader>(enum_value);
            CheckCompileErrors(sh_ID, "FRAGMENT");
            fragment_shaders_[fs] = { sh_ID, 1 };
            break;
    }

    return sh_ID;
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