#pragma once
//  TODO: implement deletion, add more logging/error checking
#include "../rendering/shader.h"
#include "../utility.h"
#include "globals.h"

#include <unordered_map>
#include <unordered_set>
#include <string>


class ShaderHandler {

public:

    enum class VertexShader {
        NONE,
        QUAD
    };
    enum class FragmentShader {
        NONE,
        QUAD
    };
    enum class ShaderType {
        VERTEX,
        FRAGMENT
    };

    void Add(EntityID const, VertexShader const, FragmentShader const);
    void Remove(EntityID const, VertexShader const, FragmentShader const);
    // sets the active shaders for entity to given and compiles program if not already
    void SetActiveShader(EntityID const, VertexShader const, FragmentShader const);
    GLuint GetActiveProgram(EntityID const handle);
private:
    std::string GetFragShaderPath(FragmentShader frag);
    std::string GetVertShaderPath(VertexShader vs);

    // select the type of shader and the enum of the file
    GLuint MakeNewShader(ShaderType const type, u32 const enum_value);
    void DeleteShader(ShaderType const type, u32 const enum_value) {};
    void CheckCompileErrors(GLuint shader, std::string type);

    // stores GLuint of shader/program and reference count
    struct IDCount{
        u32 ID = 0;
        u32 count = 0;
    };
    std::unordered_map<VertexShader, IDCount> vertex_shaders_;  
    std::unordered_map<FragmentShader, IDCount> fragment_shaders_;
    // [vertex][fragment] = IDCount of linked program
    Matrix<IDCount> program_matrix_; 

    struct EntityData {
        ShaderHandler::VertexShader active_vertex_shader; 
        std::unordered_set<ShaderHandler::VertexShader> vertex_shaders;
        ShaderHandler::FragmentShader active_fragment_shader;
        std::unordered_set<ShaderHandler::FragmentShader> fragment_shaders;
    };
    std::unordered_map<EntityID, EntityData> index_;

};