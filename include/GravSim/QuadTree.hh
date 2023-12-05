//
// Created by powew on 5/12/2023.
//

#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace QuadTree {

    template<typename T>
    class Node {
    public:
        std::unique_ptr<Node> children[4];
        int depth;
        std::vector<T> items;
    };

    template<typename T>
    class QuadTreeManager {
    public:
        Node <T> rootNode;
        float physicalSize;
        glm::vec2 center;

        explicit QuadTreeManager(float physicalSize  = 100.f, glm::vec2 center= glm::vec2(0,0));
        const Node <T> &GetNodeFromPosition(glm::vec2 position);
    };

}