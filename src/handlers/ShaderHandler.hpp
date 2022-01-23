#pragma once
#include "../rendering/shader.h"
#include "../utility.h"
#include "globals.h"

#include <unordered_map>
#include <tuple>
#include <string>


class ShaderHandler {

public:
    enum class VertexShaders {
        Quad
    };
    enum class FragmentShaders {
        Quad
    };
    // vertex then fragment shader then entity handle to register them to
    void Add(VertexShaders vs, FragmentShaders fs, EntityID);
    void Remove(EntityID handle);
    Shader GetShader(EntityID handle);
private:
    std::string GetFragShaderPath(FragmentShaders frag);
    std::string GetVertShaderPath(VertexShaders vs);

    Shader MakeNewShader(VertexShaders vs, FragmentShaders fs);

    using ShaderTuple = std::tuple<ShaderHandler::VertexShaders,
                                   ShaderHandler::FragmentShaders>;
    // stores already compiled shaders for a given vertex+fragment pair
    // maps to a shader and the count of entities using this shader
    std::unordered_map<ShaderTuple, std::tuple<uint32_t, Shader>,
                       utility::TupleHash<ShaderTuple>> shaders_;
    // stores which shader to use for each entity
    using ShaderIDTuple = std::tuple<ShaderHandler::VertexShaders,
                                     ShaderHandler::FragmentShaders, Shader>;
    std::unordered_map<EntityID, ShaderIDTuple> entity_shaders_;

};