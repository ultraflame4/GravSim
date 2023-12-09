//
// Created by powew on 5/12/2023.
//

#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "GravSim/QuadTree.hh"


namespace QuadTree {

    struct Square {
        glm::vec2 center;
        float size;

        Square(const glm::vec2 &center, float size) : center(center), size(size) {}

        float getHalfSize() const {
            return size / 2;
        }

        inline glm::vec2 getTop() const {
            glm::vec2 a = center;
            a.y += getHalfSize();
            return a;
        }

        inline glm::vec2 getBottom() {
            glm::vec2 a = center;
            a.y -= getHalfSize();
            return a;
        }

        inline glm::vec2 getLeft() {
            glm::vec2 a = center;
            a.x -= getHalfSize();
            return a;
        }

        inline glm::vec2 getRight() {
            glm::vec2 a = center;
            a.x += getHalfSize();
            return a;
        }

        bool collidePoint(glm::vec2 pos) {
            float halfSize = size / 2;
            float rightX = center.x + halfSize;
            float leftX = center.x - halfSize;
            float topY = center.y + halfSize;
            float botY = center.y - halfSize;
            if (pos.x > rightX || pos.x < leftX) return false;
            if (pos.y > topY || pos.y < botY) return false;
            return true;
        }


        int getCollideQuadIndex(glm::vec2 pos) {
            float halfSize = size / 2;
            // top
            if (pos.y > center.y) {
                // Top left
                if (pos.x < center.x) {
                    return 0;
                }
                // top right
                return 1;
            }
            // bottom left
            if (pos.x < center.x) {
                return 2;
            }
            // bottom right
            return 3;
        }

        Square subdivide(int childIndex) const{
            Square copy(center, size);
            if (childIndex % 2 == 0) {
                copy.center.x -= getHalfSize();
            }else{
                copy.center.x += getHalfSize();
            }

            if (childIndex <= 1) {
                copy.center.y += getHalfSize();
            }
            else{
                copy.center.y -= getHalfSize();
            }
            copy.size/=2;
            return copy;
        }


    };

    template<typename T>
    class Node {
    private:
        bool isEmpty = true;
    public:
        std::unique_ptr<Node<T>> children[4];
        int depth = 0;
        int index;
        Node<T> *parent;
        std::vector<T> items;

        bool empty() {
            return isEmpty;
        }

        Node<T> *addchild(int index, int _depth, bool force = false) {
            std::unique_ptr<Node<T>> &child = children[index];
            if (force) {
                child.reset();
            }
            if (child == nullptr) {
                child.reset(new Node<T>());
                child->parent = this;
                child->index = index;
                child->depth = _depth;
            }
            isEmpty = false;

            return child.get();
        }

        void clearItems() {
            items.clear();
            for (const auto &child: children) {
                if (child != nullptr && !child->isEmpty) {
                    child->clearItems();
                }
            }
            bool deleteSelf = true;
            for (int i = 0; i < 4; ++i) {
                auto child = children[i].get();
                if (child != nullptr) {
                    if (!child->isEmpty) deleteSelf = false;
                }
            }

            isEmpty = deleteSelf;
        }


    };

    inline glm::vec2 IndexToQuadCorner(int index) {
        switch (index) {
            case 0:
                return glm::vec2(-.5, .5);
            case 1:
                return glm::vec2(.5, .5);
            case 2:
                return glm::vec2(-.5, -.5);
            case 3:
                return glm::vec2(.5, -.5);
            default:
                break;
        }
        throw std::invalid_argument("Index is more than 3 ! (0-4 only)");
    }


    template<typename T>
    class QuadTreeManager {
    private:
        std::shared_ptr<spdlog::logger> logger = logging::get<QuadTreeManager>();
    public:
        std::unique_ptr<Node<T>> rootNode;
        float physicalSize;
        glm::vec2 center;

        explicit QuadTreeManager(float initialSize = 5000.f, glm::vec2 center = glm::vec2(0, 0)) {
            rootNode.reset(new Node<T>());
            rootNode->depth = 0;
            this->center = center;
            this->physicalSize = initialSize;
        }

        void clearItems() {
            rootNode->clearItems();
        }

        // Raycast for quad trees. Return all none empty nodes / quads
        Node<T> *Raycast(glm::vec2 origin, glm::vec2 direction, int maxDepth){
            QuadTree::Node<T> *current = rootNode.get();
            QuadTree::Node<T> *child = rootNode.get();
            Square quad(center,physicalSize);
            glm::vec2 head = origin;
            glm::vec2 dir = glm::normalize(direction);

            int depth = 0;
            while (current != nullptr) {

                int childIndex = quad.getCollideQuadIndex(head);
                child = current->children[childIndex];
                Square childQuad = quad.subdivide(childIndex);

                if (child == nullptr || child->empty()){
                    // If empty, advance ray
                    head += dir * childQuad.size;
                    continue;
                }

                // Descending to child! todo test

                // If raycast head isn,t in the child (new current) quad, stop descending and ascend back up!
                if (!childQuad.collidePoint(head)) {
                    current = current->parent;
                    continue;
                }


                depth++;
                current = child;
                quad=childQuad;
                head = origin;


            }
        }

        Node<T> *CreateNodeFromPosition(glm::vec2 position, int maxDepth) {


            // pointer to the current quad
            QuadTree::Node<T> *parent = rootNode.get();
            // pointer used when indexing children of search quad
            QuadTree::Node<T> *current = rootNode.get();
            Square quad(center, physicalSize);

            int depth = rootNode->depth;
            while (current != nullptr && depth <= maxDepth) {
                parent = current;
                int child_quad_index = quad.getCollideQuadIndex(position);
//                logger->debug("Quad index: {}", child_quad_index)
                current = parent->addchild(child_quad_index, depth);

                if (child_quad_index % 2 == 0) {
                    quad.center.x -= quad.getHalfSize();
                }else{
                    quad.center.x += quad.getHalfSize();
                }

                if (child_quad_index <= 1) {
                    quad.center.y += quad.getHalfSize();
                }
                else{
                    quad.center.y -= quad.getHalfSize();
                }

                quad.size /= 2;
                depth++;
            }
            return parent;
        }
    };
}