﻿#pragma once

#include <vector>
#include <array>
#include <functional>
#include <set>
#include <format>

#include "globals.h"
#include "vulkan/vulkan.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"

namespace LG
{
    enum class EPrimitiveType : uint8_t
    {
        Plane = 0,
        Cube = 1,
        GenericPortal = 2,
        BluePortal = 3,
        OrangePortal = 4,
    };

    class LGraphicsComponent
    {
        LGraphicsComponent(const LGraphicsComponent&) = delete;
        LGraphicsComponent& operator=(const LGraphicsComponent&) = delete;

    public:

        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 color;
            glm::vec2 texCoord;
        };

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }

        uint32 getIndicesCount() const
        {
            assert(indices);
            return static_cast<uint32>(indices->size());
        }

        uint32 getVerticesCount() const
        {
            assert(vertices);
            return static_cast<uint32>(vertices->size());
        }

        const std::vector<Vertex>& getVertexBuffer() const
        {
            assert(vertices);
            return *vertices;
        };

        const std::vector<uint16>& getIndexBuffer() const
        {
            assert(indices);
            return *indices;
        };

        EPrimitiveType getType()
        {
            return type;
        }

        static const std::set<std::string>& getInitTexturesData() { return textures; }

        virtual std::string getColorTexturePath() const { return texturePath;}
        void setColorTexture(std::string&& path);
        
        LGraphicsComponent();
        virtual ~LGraphicsComponent();

        const std::function<glm::mat4x4()> getModelMatrix = [](){ return glm::mat4x4(1); };
        const std::function<bool()> isAlwaysStatic = []() { return true; };
        
    protected:

        const std::vector<LG::LGraphicsComponent::Vertex>* vertices = nullptr;
        const std::vector<uint16>* indices = nullptr;

        // TODO: should be changed to hash/index storage to reduce memory usage

        //std::string typeName;
        EPrimitiveType type;
        std::string texturePath;

        // TODO: temporal desicion
        static std::set<std::string> textures;
    };

    extern const std::vector<LG::LGraphicsComponent::Vertex> verticesCube;
    extern const std::vector<uint16> indicesCube;
    extern const std::vector<LG::LGraphicsComponent::Vertex> verticesPortalCube;

    class LCube : public LGraphicsComponent
    {

    public:

        LCube()
        {
            type = EPrimitiveType::Cube;
            vertices = &verticesCube;
            indices = &indicesCube;
        }
    };

    extern const std::vector<LG::LGraphicsComponent::Vertex> verticesPlane;
    extern const std::vector<uint16> indicesPlane;

    class LPlane : public LGraphicsComponent
    {

    public:

        LPlane()
        {
            type = EPrimitiveType::Plane;
            vertices = &verticesPlane;
            indices = &indicesPlane;
        }
    };

    class LPortal : public LGraphicsComponent
    {
        friend class LRenderer;

    public:

        LPortal()
        {
            type = EPrimitiveType::GenericPortal;
            vertices = &verticesPortalCube;
            indices = &indicesCube;

            portalIndex = ++portalCounter;
            std::string textureName = std::format("portal{}", portalIndex);
            setColorTexture(std::move(textureName));
        }

        const glm::mat4& getPortalView() const { return view; }
        void setPortalView(const glm::mat4& view);
        bool needsRecalculation() const { return bNeedRecalculation; }
        virtual std::string getColorTexturePath() const override
        {
            return std::format("portal{}", portalIndex);
        }

    protected:

        glm::mat4 view = glm::mat4(1.0f);
        bool bNeedRecalculation = true;
        uint32 portalIndex;

        static uint32 portalCounter;
    };

    template<typename Component>
    bool isInstancePrimitive(Component* ptr)
    {
        return dynamic_cast<LG::LCube*>(ptr) || dynamic_cast<LG::LPlane*>(ptr);
    }

    template<typename Component>
    bool isPortal(Component* ptr)
    {
        return dynamic_cast<LG::LPortal*>(ptr);
    }

    class BluePortal: public LPortal
    {
        public:

        BluePortal()
        {
            type = EPrimitiveType::BluePortal;
        }
    };

    class OrangePortal: public LPortal
    {
        public:
        
        OrangePortal()
        {
            type = EPrimitiveType::OrangePortal;
        }
    };

}