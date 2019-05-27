#include "beachline.h"
#include "../mathVoronoi/parabola.h"

namespace Voronoi {

    /**
     * @brief Beachline::findArc
     * @param x of the point
     * @return the arc that is vertically above the point
     */
    Leaf* Beachline::findArc(const double x) const {
        if(!root)
            return nullptr;
        else {
            Node* node = root;
            while(!isLeaf(node)) {
                if(x < getValue(node))
                    node = node->left;
                else
                    node = node->right;
            }
            return static_cast<Leaf*>(node);
        }
    }

    /**
     * @brief Beachline::getValue
     * @param _node which value is returned
     * @return value of the node:
     *  - if Leaf returns site.x
     *  - if InternalNode returns xBreakpoint, but if it is NaN the breakpoint is computed and then xBreakpoint is set
     */
    double Beachline::getValue(Node* _node) const {
        if(isLeaf(_node)) {
            return static_cast<Leaf*>(_node)->site->x();
        } else {
            InternalNode* node = static_cast<InternalNode*>(_node);
            if (!isnan(node->xBreakpoint)) {
                return node->xBreakpoint;
            } else {
                std::vector<cg3::Point2Dd> intPoints = Voronoi::findIntersectionPoint(*node->breakpoint.first, *node->breakpoint.second, *sweepline);

                if(intPoints.size() == 2) {
                    if (node->breakpoint.first->y() < node->breakpoint.second->y()) {
                        node->xBreakpoint = intPoints[0].x();
                        return intPoints[0].x();
                    } else {
                        node->xBreakpoint = intPoints[1].x();
                        return intPoints[1].x();
                    }
                } else {
                    node->xBreakpoint = intPoints[0].x();
                    return intPoints[0].x();
                }
            }
        }
    }

    /**
     * @brief Beachline::makeSubtree creates a subtree for a new siteEvent
     * @param node
     */
    void Beachline::makeSubtree(Node *node, const cg3::Point2Dd& p) {
        Leaf* leaf = static_cast<Leaf*>(node);
        InternalNode* newNode = new InternalNode(node->parent,
                                                 new Leaf(nullptr, nullptr, nullptr, leaf->site),
                                                 new InternalNode(nullptr,
                                                                  new Leaf(nullptr, nullptr, nullptr, &p),
                                                                  new Leaf(nullptr, nullptr, nullptr, leaf->site),
                                                                  std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*>(&p, leaf->site)),
                                                 std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*>(leaf->site, &p));
        newNode->left->parent = newNode;
        newNode->right->parent = newNode;
        newNode->right->left = newNode->right;
        newNode->right->right = newNode->right;
        delete node;
        node = newNode;
    }

    /**
     * @brief Beachline::addPoint
     * @param point to be added to the beachline
     */
    void Beachline::addPoint(const cg3::Point2Dd &p) {
        if(!root) {
            root = new Leaf(&p);
        } else {
            Node* arc = findArc(p.x());
            makeSubtree(arc, p);
        }
    }

    /**
     * @brief Beachline::deleteNode deletes the subtree with node as root
     * @param node
     */
    void Beachline::deleteNode(Node *node) {
        if(node) {
            deleteNode(node->left);
            deleteNode(node->right);
            delete node;
        }
    }

}
