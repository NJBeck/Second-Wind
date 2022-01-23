#include "ShaderHandler.hpp"

using std::get, std::string;

void ShaderHandler::Add(VertexShaders vs, FragmentShaders fs, EntityID handle)
{
    auto found = shaders_.find({ vs, fs });
    if (found != shaders_.end()) {
        entity_shaders_[handle] = { vs, fs, get<1>(found->second) };
        ++std::get<0>(found->second);   //increment the count of shader refs
    }
    else {
        auto new_shader = MakeNewShader(vs, fs);
        shaders_[{vs, fs}] = { 1, new_shader };
        entity_shaders_[handle] = { vs, fs, new_shader };
    }
}

void ShaderHandler::Remove(EntityID handle) {
    auto found_entity = entity_shaders_.find(handle);
    if (found_entity != entity_shaders_.end()) {
        // find shader and decrement count
        // then delete it from shaders_ if count is 0
        ShaderTuple shad_tup{ get<0>(found_entity->second), 
                              get<1>(found_entity->second) };
        auto found_shader = shaders_.find(shad_tup);
        uint32_t& count = get<0>(found_shader->second);
        --count;
        if (count == 0) shaders_.erase(found_shader);
        entity_shaders_.erase(handle);
    }
    else {
        string err{ "entity " + std::to_string(handle) + 
                    " does not have an associated shader when trying to erase" };
        throw std::runtime_error(err);
    }
}

Shader ShaderHandler::GetShader(EntityID handle)
{
    auto found = entity_shaders_.find(handle);
    if (found != entity_shaders_.end()) {
        return get<2>(found->second);
    }
    else {
        string err{ "entity " + std::to_string(handle) +
            " does not have an associated shader when trying to retrieve it" };
        throw std::runtime_error(err);
    }
}

std::string ShaderHandler::GetFragShaderPath(FragmentShaders frag)
{
    switch (frag) {
    case FragmentShaders::Quad: {
        return utility::getDataPath("quadshader.fs");
    }
    }
}

std::string ShaderHandler::GetVertShaderPath(VertexShaders vert) {
    switch (vert) {
    case VertexShaders::Quad: {
        return utility::getDataPath("quadshader.vs");
    }
    }
}

Shader ShaderHandler::MakeNewShader(VertexShaders vs, FragmentShaders fs)
{
    std::string vert_source = GetVertShaderPath(vs);
    std::string frag_source = GetFragShaderPath(fs);

    return Shader(vert_source, frag_source);

}
