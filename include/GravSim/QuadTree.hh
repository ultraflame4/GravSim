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

        int getchild_index(glm::vec2 pos) {
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

        Square getchild(int childIndex) const{
            Square copy(center, size);
            copy.size/=2;
            if (childIndex % 2 == 0) {
                copy.center.x -= copy.getHalfSize();
            }else{
                copy.center.x += copy.getHalfSize();
            }

            if (childIndex <= 1) {
                copy.center.y += copy.getHalfSize();
            }
            else{
                copy.center.y -= copy.getHalfSize();
            }
            return copy;
        }

        Square getchild(glm::vec2 pos){
            return getchild(getchild_index(pos));
        }
        /**
         * Gets the parent square to this square.
         * @param quad_index The quad index of this square
         * @return A square representing the parent
         */
        Square getparent(int quad_index){
            Square copy(center, size)
            copy.size*=2;
            if (quad_index % 2 == 0) {
                copy.center.x += copy.getHalfSize();
            }else{
                copy.center.x -= copy.getHalfSize();
            }

            if (quad_index <= 1) {
                copy.center.y -= copy.getHalfSize();
            }
            else{
                copy.center.y += copy.getHalfSize();
            }

            return copy;
        }

    };

    template<typename T>
    class Node {
    private:
        bool _isEmpty = true;
    public:
        std::unique_ptr<Node<T>> children[4];
        int depth = 0;
        int index;
        Node<T> *parent;
        std::vector<T> items;

        bool IsEmpty() {
            return _isEmpty;
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
            _isEmpty = false;

            return child.get();
        }

        void clearItems() {
            items.clear();
            for (const auto &child: children) {
                if (child != nullptr && !child->_isEmpty) {
                    child->clearItems();
                }
            }
            bool deleteSelf = true;
            for (int i = 0; i < 4; ++i) {
                auto child = children[i].get();
                if (child != nullptr) {
                    if (!child->_isEmpty) deleteSelf = false;
                }
            }

            _isEmpty = deleteSelf;
        }
    };

    template<typename T>
    struct Quad{

        Node<T>* node;
        Square square;


        Quad<T> addchild(int index, bool force=false){
            node->addchild(index, node->depth+1, force);
            return getchild(index);
        }

        Quad<T> addchild(glm::vec2 pos, bool force = false){
            return addchild(square.getchild_index(pos),  force);
        }
        Quad<T> getchild(int index){
            return Quad<T>(node->children[index].get(),square.getchild(index));
        }
        Quad<T> getchild(glm::vec2 pos){
            return getchild(square.getchild_index(pos));
        }
        Quad<T> getparent(){
            return Quad<T>(node->parent,square.getparent(node->index));
        }

        bool collidePoint(glm::vec2 pos){
            return square.collidePoint(pos);
        }
        bool IsEmpty(){
            return node == nullptr || node->IsEmpty();
        }

        static inline Quad<T> fromRootNode(Node<T>* node, glm::vec2 center, float rootSize){
            return Quad<T>(node, Square(center, rootSize));
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

        Quad<T> GetRootQuad(){
            return Quad<T>::fromRootNode(rootNode.get(),center, physicalSize);
        }

        Quad<T> GetOrCreateQuad(glm::vec2 position, int maxDepth) {

            Quad<T> current = GetRootQuad();

            while ( current.node->depth <= maxDepth) {
                current = current.addchild(position);
            }
            return current;
        }
    };
}