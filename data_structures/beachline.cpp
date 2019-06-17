#include "beachline.h"
#include <iostream>

namespace Voronoi {

    Beachline::Beachline(const Beachline& bl) {
        this->root = copyBeachline(bl.root);
        updateAttributes(this->root);
    }

    Beachline::Beachline(Beachline&& bl) {
        this->root = bl.root;
        this->sweepline = bl.sweepline;

        bl.root = nullptr;
        bl.sweepline = nullptr;
    }

    void Beachline::swap(Beachline& bl) {
        using std::swap;
        swap(this->root, bl.root);
        swap(this->sweepline, bl.sweepline);
    }

    Beachline& Beachline::operator=(Beachline bl) {
        Beachline::swap(bl);
        return *this;
    }

    Beachline& Beachline::operator=(Beachline&& bl) {
        deleteNode(this->root);

        std::swap(this->root, bl.root);
        std::swap(this->sweepline, bl.sweepline);

        return *this;
    }

    Beachline::~Beachline() {
        deleteNode(root);
    }

    /**
     * @brief Beachline::findArc find the arc (leaf) where the new subtree needs to be created
     * @param x: x-coordinate of the new point that needs to be added
     * @param _balance: stores the heights of each subtree
     * @param path: stores a path to the arc with -1 means moving to left child and 1 means moving to right child
     * @param diff: stores the differences in _balance
     * @return
     */
    Leaf* Beachline::findArc(const double x,
                             std::vector<std::pair<int,int>>& _balance, std::vector<int>& path, int& diff) const {
        if(!root)
            return nullptr;
        else {
            Node* node = root;
            Node* child;
            bool oneFound = false;
            while(!isLeaf(node)) {
                if(x < getValue(node)) {
                    child = node->left;
                    path.push_back(-1);
                    if (!isLeaf(child)) {
                        int temp = node->left->height - node->right->height;
                        if(temp == -1 && oneFound)
                            diff += temp;
                        if(temp == 1) {
                            if(!oneFound) oneFound = true;
                            if(diff!= 2) diff += temp;
                        }
                        _balance.push_back(std::pair<int,int>(node->left->height, node->right->height));
                    }
                    node = child;
                } else {
                    child = node->right;
                    path.push_back(1);
                    if (!isLeaf(child)) {
                        int temp = node->right->height - node->left->height;
                        if(temp == -1 && oneFound)
                            diff += temp;
                        if(temp == 1) {
                            if(!oneFound) oneFound = true;
                            if(diff!= 2) diff += temp;
                        }
                        _balance.push_back(std::pair<int,int>(node->right->height, node->left->height));
                    }
                    node = child;
                }
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

            std::vector<cg3::Point2Dd> intPoints = Voronoi::findIntersectionPoint(*node->breakpoint.first, *node->breakpoint.second, *sweepline);

            if(intPoints.size() == 2) {
                if (node->breakpoint.first->y() < node->breakpoint.second->y())
                    return intPoints[1].x();
                else
                    return intPoints[0].x();
            } else {
                return intPoints[0].x();
            }
        }
    }

    /**
     * @brief Beachline::makeSubtree creates a subtree for a new siteEvent
     * @param node
     * @param p: the new site to be added to the Beachline
     * @return the pointer to the circleEvent to check if it is a false alarm
     */
    Event* Beachline::makeSubtree(Node*& node, const cg3::Point2Dd& p, std::vector<Voronoi::HalfEdge>& edges) {
        Leaf* leaf = static_cast<Leaf*>(node);
        InternalNode* newNode = new InternalNode(node->parent,
                                                 new Leaf(nullptr, leaf->prev, nullptr, leaf->site),
                                                 new InternalNode(nullptr,
                                                                  new Leaf(nullptr, nullptr, nullptr, &p),
                                                                  new Leaf(nullptr, nullptr, leaf->next, leaf->site),
                                                                  1,
                                                                  std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*>(&p, leaf->site)),
                                                 2,
                                                 std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*>(leaf->site, &p));
        //Connecting the parent for each new Node
        newNode->left->parent = newNode;
        newNode->right->parent = newNode;
        newNode->right->left->parent = newNode->right;
        newNode->right->right->parent = newNode->right;

        //Connecting next and prev pointers
        static_cast<Leaf*>(newNode->left)->next = static_cast<Leaf*>(newNode->right->left);
        static_cast<Leaf*>(newNode->right->left)->prev = static_cast<Leaf*>(newNode->left);
        static_cast<Leaf*>(newNode->right->left)->next = static_cast<Leaf*>(newNode->right->right);
        static_cast<Leaf*>(newNode->right->right)->prev = static_cast<Leaf*>(newNode->right->left);

        //HalfEdges creation and pointers added to each breakpoint in Beachline
        size_t lastIndex = edges.size();
        edges.push_back(Voronoi::HalfEdge());
        edges.back().setTwin(lastIndex+1);
        edges.push_back(Voronoi::HalfEdge());
        edges.back().setTwin(lastIndex);

        newNode->edge = lastIndex+1;
        static_cast<InternalNode*>(newNode->right)->edge = lastIndex;

        if(!isRight(node))
            node->parent->left = newNode;
        else
            node->parent->right = newNode;

        Event* cEvent = leaf->circleEvent;

        delete node;
        node = newNode;

        return cEvent;
    }

    /**
     * @brief Beachline::addPoint add a point to the Beachline, creating a subtree and rebalancing the Beachline
     * @param p: the point to be added
     * @param newPoint: the reference to the leaf representing the new point, it will be used to check circle events
     * @param edges: the vector containing the edges of the Voronoi diagram
     * @return the Event linked to the substitued leaf to check for false alarm
     */
    Event* Beachline::addPoint(const cg3::Point2Dd& p, Leaf*& newPoint, std::vector<Voronoi::HalfEdge>& edges) {
        if(!root) {
            root = new Leaf(&p);
            return nullptr;
        } else {
            //stores the balance of the children for each node visited to find the arc (pair.first takes the
            std::vector<std::pair<int,int>> _balance;
            //stores the path to the arc (-1 for left child, 1 for right child)
            std::vector<int> path;
            //store the differences of balance, diff += (pair.first - pair.second), for each pair
            int diff = 0;
            Node* arc = findArc(p.x(), _balance, path, diff);
            Event* cEvent = makeSubtree(arc, p, edges);
            newPoint = static_cast<Leaf*>(arc->right->left);
            //Here (in this line of code) heights of the tree have not been updated yet
            if(arc != root) {
                handleRotation(arc, _balance, path, diff);
            }

            return cEvent;
        }
    }

    void Beachline::removePoint(const Leaf*& circleArc) {
        const cg3::Point2Dd* siteUpdate;
        InternalNode* _parent = static_cast<InternalNode*>(circleArc->parent),
                * _parentUpdate = static_cast<InternalNode*>(circleArc->parent->parent);
        Node* otherChild;

        //Update the tuples of breakpoints
        if(_parent->breakpoint.first == circleArc->site)
            siteUpdate = _parent->breakpoint.second;
        else
            siteUpdate = _parent->breakpoint.first;

        if(_parentUpdate->breakpoint.first == circleArc->site)
            _parentUpdate->breakpoint.first = siteUpdate;
        else
            _parentUpdate->breakpoint.second = siteUpdate;

        //Update the attributes of the other nodes (parent, left, right pointers)
        if(isRight(circleArc)) {
            otherChild = circleArc->parent->left;
            otherChild->parent = circleArc->parent->parent;
            if(isRight(circleArc->parent))
                circleArc->parent->parent->right = otherChild;
            else
                circleArc->parent->parent->left = otherChild;
        } else {
            otherChild = circleArc->parent->right;
            otherChild->parent = circleArc->parent->parent;
            if(isRight(circleArc->parent))
                circleArc->parent->parent->right = otherChild;
            else
                circleArc->parent->parent->left = otherChild;
        }

        delete circleArc->parent;
        delete circleArc;

        rebalanceCE(otherChild);
    }

    /**
     * @brief Beachline::handleRotation The rotation has been optimized with respect to the fact that for each new point
     * a subtree of height 2 needs to be created. The algorithm first categorize the cases in terms of height:
     * - height 2 if _balance is empty then no rotation is necessary, because it means arc is a leaf of the root and height of root is updated
     * - height >= 2 (if _balance is not empty):
     *      - diff <= 0: if first element of last position of balance is 2 only an update of parents is necessary
     *                   else only rotation of arc and update of heights
     *      - diff == 1: if diff == 1 because of the last element of _balance only a rotation of parent is necessary and relative heights update
     *                   else {
     *                      if first element of last position of balance is not 2 a first rotation on arc is necessary
     *                      a (second)(depending on previous if) rotation is performed on the first ancestor with diff = +1,
     *                      starting from arc->parent to the root, so arc->parent is skipped, because if the previous if is true,
     *                      then a rotation on arc isn't performed because arc->parent is balanced, and if the previous if is false,
     *                      then a rotation on arc is performed, and arc takes place of arc->parent --> a rotation is necessary in one of ancestors
     *                   }
     *      - diff == 2: if last element of _balance is (1,1) and other child of arc->parent is null, then
     *                   a first rotation is performed on arc->parent and a second rotation on one of its ancestors
     *                   else a first rotation is performed on arc and a second rotation on one of arc->parent ancestors
     * @param arc: the node where the new subtree has been added before
     * @param _balance: stores the heights of each subtree
     * @param path: stores a path to the arc with -1 means moving to left child and 1 means moving to right child
     * @param diff: stores the differences in _balance
     */
    void Beachline::handleRotation(Node *&arc,
                                   std::vector<std::pair<int, int> > &_balance, std::vector<int> &path, int diff) {
        /*if(arc->height == 1) {
            if(isRight(arc))
                rotateLeft(arc->right);         CAN'T HAPPEN THAT BEACHLINE HAS HEIGHT 1
            else
                rotateLeftRight(arc->right);
        } else {*/
        if(!_balance.empty()) {
            if(diff<=0) {
                if(_balance.back().first != 2) {
                    whichRotation(arc, path.back(), 1);
                } else
                    updateHeight(arc->parent);
            } else if(diff==1) {
                //diff is == 1 because the height of arc->parent is higher than arc->parent->parent->other_child one
                if(_balance.back().first - _balance.back().second == 1) {
                    whichRotation(arc->parent, path[path.size()-2], path.back());
                } else {
                    //a first rotation on arc
                    if(_balance.back().first != 2) {
                        whichRotation(arc, path.back(), 1);
                    }
                    //a second (depending on previous if) rotation on the first +1 in balance starting from the tail of _balance - 1
                    findPlus1AndRotate(arc->parent, _balance, path);
                }
            } else if (diff==2) {
                if(_balance.back().first == 1 && _balance.back().second == 1 && (!arc->parent->left || !arc->parent->right)) {
                            //if the children of arc->parent->parent have same balance and the balance = 1 and
                            //one the other child of arc->parent is null then the rotation is performed on arc->parent
                            whichRotation(arc->parent, path[path.size()-2], path.back());
                            //a second rotation on the first +1 in balance starting from the tail of _balance - 1
                            if (path[path.size()-1] == path.back())
                                findPlus1AndRotate(arc->parent, _balance, path);
                            else //in case of LeftRight or RightLeft rotation in previous code line, then arc becomes arc->parent
                                findPlus1AndRotate(arc, _balance, path);
                } else {
                    //if the "if" before is not true then the rotation can be performed in arc
                    whichRotation(arc, path.back(), 1);
                    if (path.back() == 1) //in case of Left rotation in previous code line, then arc becomes arc->parent
                        findPlus1AndRotate(arc, _balance, path);
                    else
                        findPlus1AndRotate(arc->parent, _balance, path);
                }
            }
        } else
            arc->parent->height += 1; //tree has height equals to 2 and arc is a child of the root
        //}
    }

    /**
     * @brief Beachline::rebalanceCE The sibling t of the arc q (arc that has been removed), once q has been removed,
     * has a new sibling s, and there are 2 cases to handle:
     *     - (s-t == 2): it's necessary just a rotation on s
     *     - (s-t == 0): a rotation could be possible on an ancestor, otherwise all ancestors heights are decreased by 1
     * With a difference equals to 1 no action needs to be performed
     * @param node: is the sibling of the removed arc
     */
    void Beachline::rebalanceCE(Node* node) {
        Node* sibling;
        if(isRight(node))
            sibling = node->parent->left;
        else
            sibling = node->parent->right;
        switch (sibling->height - node->height) {
            case 2:
                if(isRight(node))
                    rotateRight(node->parent->left);
                else
                    rotateLeft(node->parent->right);
                break;
            case 0:
                while(sibling->parent != root) {
                    sibling = sibling->parent;
                    sibling->height -= 1;
                    if(isRight(sibling)) {
                        if(sibling->parent->left->height - sibling->height == 2) {
                            if(balance(sibling->parent->left) >= 0)
                                whichRotation(sibling->parent->left, -1, 1);
                            else
                                whichRotation(sibling->parent->left, -1, -1);
                        } else if(sibling->parent->left->height - sibling->height == 1)
                            return;
                    } else {
                        if(sibling->parent->right->height - sibling->height == 2) {
                            if(balance(sibling->parent->right) >= 0)
                                whichRotation(sibling->parent->right, 1, 1);
                            else
                                whichRotation(sibling->parent->right, 1, -1);
                        } else if(sibling->parent->right->height - sibling->height == 1)
                            return;
                    }
                }
                root->height -= 1;
        }
    }

    /**
     * @brief Beachline::rotateLeft
     * @param node as center of rotation
     */
    void Beachline::rotateLeft(Node *&node) {
        //heights updates
        node->height = std::max(node->height, node->parent->height);
        node->parent->height = node->parent->left ?
                    std::max(node->parent->left->height, node->left->height) + 1 : node->left->height + 1;

        //rotation algorithm
        node->parent->right = node->left;
        if(node->left)
            node->left->parent = node->parent;
        node->left = node->parent;
        Node* temp = node->parent->parent;
        node->parent->parent = node;
        node->parent = temp;
    }

    /**
     * @brief Beachline::rotateRight
     * @param node as center of rotation
     */
    void Beachline::rotateRight(Node *&node) {
        node->height = std::max(node->height, node->parent->height);
        node->parent->height = node->parent->right ?
                    std::max(node->parent->right->height, node->right->height) + 1: node->right->height + 1;

        //rotation algorithm
        node->parent->left = node->right;
        if(node->right)
            node->right->parent = node->parent;
        node->right = node->parent;
        Node* temp = node->parent->parent;
        node->parent->parent = node;
        node->parent = temp;
    }

    /**
     * @brief Beachline::rotateLeftRight
     * @param node
     */
    void Beachline::rotateLeftRight(Node *&node) {
        rotateLeft(node);
        rotateRight(node);
    }

    /**
     * @brief Beachline::rotateRightLeft
     * @param node
     */
    void Beachline::rotateRightLeft(Node *&node) {
        rotateRight(node);
        rotateLeft(node);
    }

    /**
     * @brief Beachline::whichRotation rotate the node with respect of the values of firstPath and of secondPath
     * @param node: the node to rotate
     * @param firstPath: represent if the node is a left (-1) or right (1) child
     * @param secondPath: represent which of the children of node unbalance the tree, with -1 as left child and 1 as right child
     */
    void Beachline::whichRotation(Node *&node, const int firstPath, const int secondPath) {
        if(firstPath == 1 && secondPath == 1) {
            rotateLeft(node);
            updateHeight(node);
        } else if (firstPath == -1 && secondPath == -1) {
            rotateRight(node);
            updateHeight(node);
        } else if (firstPath == 1 && secondPath == -1) {
            rotateRightLeft(node);
            updateHeight(node->parent);
        } else {
            rotateLeftRight(node);
            updateHeight(node->parent);
        }
    }

    /**
     * @brief Beachline::findPlus1AndRotate finds the first +1 in balance starting from the tail of _balance - 1 and perform the rotation on that node
     * @param node: the search starts from this node to the root
     * @param _balance: stores the heights of each subtree
     * @param path: stores a path to the arc with -1 means moving to left child and 1 means moving to right child
     */
    void Beachline::findPlus1AndRotate(Node *&node, const std::vector<std::pair<int,int>>& _balance, const std::vector<int>& path) {
        size_t index = _balance.size()-1;
        do {
            index--;
            node = node->parent;
            if((_balance[index].first - _balance[index].second) == 1) {
                //the rotation is performed on the node with a balance higher of its sibling
                whichRotation(node, path[index], path[index+1]);
                return;
            }
        } while(index != 0);
    }

    /**
     * @brief Beachline::updateHeight updates the height of the parents of the node passed as parameter
     * @param node
     */
    void Beachline::updateHeight(Node *node) {
        int height = node->height;
        while(node) {
            node = node->parent;
            if(node->height < height + 1) {
                node->height = height + 1;
                height += 1;
            }
            else
                return;
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

    /**
     * @brief Beachline::copyBeachline Helper function for copy constructor
     * @param node
     * @return
     */
    Node* Beachline::copyBeachline(Node* const &node) {
        Node* _copy = nullptr;
        if(node) {
            if(isLeaf(node)) {
                Leaf* leaf = static_cast<Leaf*>(node);
                return new Leaf(nullptr, nullptr, nullptr, leaf->site, leaf->circleEvent);
            } else {
                InternalNode* intNode = static_cast<InternalNode*>(node);
                return new InternalNode(nullptr, copyBeachline(intNode->left), copyBeachline(intNode->right), intNode->height, intNode->edge, intNode->breakpoint);
            }
        }
        return _copy;
    }

    /**
     * @brief Beachline::min
     * @param node
     * @return minimum of the subtree with node as root
     */
    Leaf* Beachline::min(Node* node) const {
        while(node->left != nullptr)
            node = node->left;
        return static_cast<Leaf*>(node);
    }

    /**
     * @brief Beachline::max
     * @param node
     * @return maximum of the subtree with node as root
     */
    Leaf* Beachline::max(Node* node) const {
        while(node->right != nullptr)
            node = node->right;
        return static_cast<Leaf*>(node);
    }

    /**
     * @brief Beachline::next
     * @param node
     * @return successor of node
     */
    Leaf* Beachline::next(Node* node) const {
        Node* parent;

        if(node->right != nullptr)
            return min(node->right);

        parent = node->parent;

        while(parent != nullptr && node == parent->right) {
            node = parent;
            parent = parent->parent;
        }
        return static_cast<Leaf*>(parent);
    }

    /**
     * @brief Beachline::prev
     * @param node
     * @return predecessor of node
     */
    Leaf* Beachline::prev(Node* node) const {
        Node* parent;

        if(node->left != nullptr)
            return max(node->left);

        parent = node->parent;

        while(parent != nullptr && node == parent->left) {
            node = parent;
            parent = parent->parent;
        }
        return static_cast<Leaf*>(parent);
    }

    /**
     * @brief Beachline::updateParent support function for the Beachline copy; this function updates parent, prev and next pointers
     * @param node whose attributes is updated
     */
    void Beachline::updateAttributes(Node*& node) {
        if(node->left) {
            node->left->parent = node;
            updateAttributes(node->left);
        }
        if(node->right) {
            node->right->parent = node;
            updateAttributes(node->right);
        }
        if(!node->left && !node->right) {
            Leaf* leaf = static_cast<Leaf*>(node);
            leaf->prev = prev(leaf);
            leaf->next = next(leaf);
        }
    }

    /**
     * @brief Beachline::inorder Inorder visit of the Beachline
     * @param node: root of the tree to visit
     */
    void Beachline::inorder(Node *node) const {
        if(node) {
            inorder(node->left);
            inorder(node->right);
            if(isLeaf(node))
                std::cout<<static_cast<Leaf*>(node)->site;
            else {
                InternalNode* print = static_cast<InternalNode*>(node);
                std::cout<<"<"<<print->breakpoint.first<<"> <"<<print->breakpoint.second<<">";
            }
        }
    }

} 
