//
// Created by powew on 5/12/2023.
//

#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "GravSim/QuadTree.hh"


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

    template<typename T>
    QuadTree::QuadTreeManager<T>::QuadTreeManager(float physicalSize, glm::vec2 center):physicalSize(physicalSize),
                                                                                        center(center) {
    }

    template<typename T>
    const QuadTree::Node<T> &QuadTree::QuadTreeManager<T>::GetNodeFromPosition(glm::vec2 position) {
        glm::vec2 quad_center = QuadTree::QuadTreeManager<T>::center;
        float halfSize = QuadTree::QuadTreeManager<T>::physicalSize / 2;
        // pointer to the current quad
        QuadTree::Node<T> *parent = &QuadTree::QuadTreeManager<T>::rootNode;
        // pointer used when indexing children of search quad
        QuadTree::Node<T> *current = &QuadTree::QuadTreeManager<T>::rootNode;

        while (current != nullptr) {
            parent = current;
            if (position.y > quad_center.y) {
                // Top left
                if (position.x < quad_center.x) {
                    current = parent->children[0].get();
                    quad_center.x -= halfSize;
                }
                    // top right
                else {
                    current = parent->children[1].get();
                    quad_center.x += halfSize;
                }
                quad_center.y += halfSize;
            } else {
                // bottom left
                if (position.x < quad_center.x) {
                    current = parent->children[2].get();
                    quad_center.x -= halfSize;
                }
                    // bottom right
                else {
                    current = parent->children[3].get();
                    quad_center.x += halfSize;
                }
                quad_center.y -= halfSize;
            }
            halfSize = halfSize / 2;
        }
        return parent;
    }
}