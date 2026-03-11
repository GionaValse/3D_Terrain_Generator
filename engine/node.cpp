/**
 * @file    node.cpp
 * @brief   Node class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include <iostream>

#include "engine.h"

namespace Eng {

////////////////
// NODE CLASS //
////////////////

Node::Node(const std::string& name, const glm::mat4& matrix)
    : Object(name),
      m_parent{nullptr},
      m_matrix{matrix},
      m_isMoving(false),
      anchor{nullptr}

{
    m_destinationMatrix = matrix;
}

Node::~Node() {
    for (int i = 0; i < m_children.size(); i++) {
        delete m_children[i];
    }

    m_parent = nullptr;  // useless but useful for mind
}

void Node::render(glm::mat4 modelview) {

}

glm::mat4 Node::getWorldCoordinateMatrix() const {
    if (m_parent == nullptr) {
        return getMatrix();
    }
    glm::vec3 parentTranslation = glm::vec3(m_parent->getWorldCoordinateMatrix()[3]);

    glm::mat4 parentTranslationMatrix = glm::translate(glm::mat4(1.0f), parentTranslation);

    return parentTranslationMatrix * m_matrix;
}

void Node::setParent(Node* newParent) {
    m_parent = newParent;
}

bool Node::addChild(Node* child) {
    if (child == nullptr) {
        return false;
    }

    for (int i = 0; i < m_children.size(); i++) {
        if (m_children[i] == child)
            return false;
    }

    if (child->getParent() != nullptr) {
        child->getParent()->removeChild(child);
    }

    m_children.push_back(child);
    child->setParent(this);

    return true;
}

bool Node::removeChild(unsigned int n) {
    if (n >= m_children.size()) {
        return false;
    }

    if (m_children[n]->getParent() == this) {
        m_children[n]->setParent(nullptr);
    }

    m_children.erase(m_children.begin() + n);
    return true;
}

bool Node::removeChild(Node* child) {
    for (int i = 0; i < m_children.size(); i++) {
        if (m_children[i] == child) {
            m_children.erase(m_children.begin() + i);
            return true;
        }
    }
    return false;
}

float Node::getSpeed() const { return speed; }
void Node::setSpeed(float speed_) { speed = speed_; }

const glm::mat4 Node::getMatrix() const {
    return m_matrix;
}

void Node::setMatrix(const glm::mat4& matrix) {
    m_matrix = matrix;
}

void Node::resetMove() {
    m_stepMatrix.clear();
    m_steps.clear();
    m_isMoving = false;
}

void Node::stopMove() {
    m_isMoving = false;
}

void Node::resumeMove() {
    m_isMoving = true;
}

void Node::move(glm::mat4 stepMatrix, int steps) {
    if (steps >= 0) {
        for (size_t i = 0; i < steps; i++) {
            m_destinationMatrix = stepMatrix * m_destinationMatrix;
        }
    }

    for (size_t i = 0; i < m_stepMatrix.size(); i++) {
        if (m_stepMatrix[i] == stepMatrix && m_steps[i] == -1) {
            m_isMoving = true;
            return;
        }
    }

    m_stepMatrix.push_back(stepMatrix);
    m_isMoving = true;
    m_steps.push_back(steps);
}

bool Node::isMoving() const {
    return m_isMoving;
}

void Node::calculateMove() {
    speed_counter++;
    if (speed_counter >= Eng::Base::getInstance().getCurrentFPS() * 1 / speed) {
        if (m_stepMatrix.size() > 0) {
            int count = 0;
            for (size_t i = 0; i < m_stepMatrix.size(); i++) {
                if (m_steps[i] > 0) {
                    setMatrix(m_stepMatrix[i] * getMatrix());
                    m_steps[i]--;
                }
                if (m_steps[i] == 0) {
                    m_steps.erase(m_steps.begin() + i);
                    m_stepMatrix.erase(m_stepMatrix.begin() + i);
                } else if (m_steps[i] == -1) {
                    count++;
                    setMatrix(m_stepMatrix[i] * getMatrix());
                }
            }
            if (m_stepMatrix.size() == count) {
                m_isMoving = false;
            }
        } else {
            m_isMoving = false;
        }
        speed_counter = 0;
    }
}

Node* Node::getChild(unsigned int n) const {
    if (m_children.size() <= n) {
        return nullptr;
    }

    return m_children[n];
}

Node* Node::getChild(std::string name) const {
    for (int i = 0; i < m_children.size(); i++) {
        if (m_children[i]->getName() == name) {
            return m_children[i];
        }
    }

    for (int i = 0; i < m_children.size(); i++) {
        Node* rec = m_children[i]->getChild(name);
        if (rec != nullptr) {
            return rec;
        }
    }

    return nullptr;
}

std::vector<Node*> Node::getChildren() const {
    return m_children;
}

unsigned int Node::getNumberOfChildren() const {
    return m_children.size();
}

};  // namespace Eng
