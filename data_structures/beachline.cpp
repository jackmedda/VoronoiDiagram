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

    /**
     * @brief Beachline::~Beachline
     */
    Beachline::~Beachline() {
        deleteNode(root);
    }

    /**
     * @brief Beachline::findArc find the arc (leaf) where the new subtree needs to be created
     * @param x: x-coordinate of the new point that needs to be added
     * @param _balance: stores the heights of each subtree
     * @param path: stores a path to the arc with -1 means moving to left child and 1 means moving to right child
     * @param diff: stores the differences in _balance
     * @param last: stores the last difference in _balance (not 0)
     * @return
     */
    Node* Beachline::findArc(const double x,
                             std::vector<std::pair<int,int>>& _balance, std::vector<int>& path, int& diff, int& last) const {
        if(!root)
            return nullptr;
        else {
            Node* node = root;
            Node* child;
            while(!isLeaf(node)) {
                if(x < getValue(node)) {
                    child = node->left;
                    path.push_back(-1);
                    if (!isLeaf(child)) {
                        int temp = node->left->height - node->right->height;
                        if (temp == -1) {
                            diff = 0;
                            last = -1;
                        }
                        if (temp == 1) {
                            if (diff != 2)
                                diff += temp;
                            last = 1;
                        }
                        _balance.push_back(std::pair<int,int>(node->left->height, node->right->height));
                    }
                    node = child;
                } else {
                    child = node->right;
                    path.push_back(1);
                    if (!isLeaf(child)) {
                        int temp = node->right->height - node->left->height;
                        if (temp == -1) {
                            diff = 0;
                            last = -1;
                        }
                        if (temp == 1) {
                            if (diff != 2)
                                diff += temp;
                            last = 1;
                        }
                        _balance.push_back(std::pair<int,int>(node->right->height, node->left->height));
                    }
                    node = child;
                }
            }
            return node;
        }
    }

    /**
     * @brief Beachline::getValue
     * @param _node which value is returned
     * @return value of the node:
     *  - if Leaf returns site.x
     *  - if InternalNode returns intersections of parabolas
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
    CircleEvent* Beachline::makeSubtree(Node*& node, const cg3::Point2Dd& p, std::vector<Voronoi::HalfEdge>& edges) {
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

        if(leaf->prev)
            leaf->prev->next = static_cast<Leaf*>(newNode->left);
        if(leaf->next)
            leaf->next->prev = static_cast<Leaf*>(newNode->right->right);

        //HalfEdges creation and pointers added to each breakpoint in Beachline
        size_t lastIndex = edges.size();
        edges.push_back(Voronoi::HalfEdge());
        edges.back().setTwin(lastIndex+1);
        edges.push_back(Voronoi::HalfEdge());
        edges.back().setTwin(lastIndex);

        newNode->edge = lastIndex;
        static_cast<InternalNode*>(newNode->right)->edge = lastIndex+1;

        if(node->parent) {
            if(!isRight(node))
                node->parent->left = newNode;
            else
                node->parent->right = newNode;
        } else
            this->root = newNode;

        CircleEvent* cEvent = leaf->circleEvent;

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
    CircleEvent* Beachline::addPoint(const cg3::Point2Dd& p, Leaf*& newPoint, std::vector<Voronoi::HalfEdge>& edges) {
        if(!root) {
            root = new Leaf(&p);
            return nullptr;
        } else {
            //stores the balance of the children for each node visited to find the arc (pair.first takes the
            std::vector<std::pair<int,int>> _balance;
            //stores the path to the arc (-1 for left child, 1 for right child)
            std::vector<int> path;
            //store the differences of balance, diff += (pair.first - pair.second), for each pair
            int last = 0, diff = 0;
            Node* arc = findArc(p.x(), _balance, path, diff, last);
            CircleEvent* cEvent = makeSubtree(arc, p, edges);
            newPoint = static_cast<Leaf*>(arc->right->left);
            //Here (in this line of code) heights of the tree have not been updated yet
            if(arc != root) {
                handleRotation(arc, _balance, path, diff, last);
            }

            return cEvent;
        }
    }

    Leaf* Beachline::removePoint(const CircleEvent* cE, DCEL& dcel) {
        std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*> oldBreak, newBreak;
        const Leaf* circleArc = cE->getArc();
        Leaf* prev = circleArc->prev;
        Node* otherChild;
        std::vector<HalfEdge>& edges = dcel.getHalfEdges();
        size_t parentEdge, otherEdge, newEdge;
        bool isFirstBreakpoint;

        InternalNode* _parent = static_cast<InternalNode*>(circleArc->parent);
        InternalNode* _otherParent = static_cast<InternalNode*>(_parent->parent);

        //Update the tuples of breakpoints
        if(_parent->breakpoint.first == circleArc->site) {
            oldBreak = std::make_pair(circleArc->prev->site, circleArc->site);
            newBreak = std::make_pair(circleArc->prev->site, _parent->breakpoint.second);
            isFirstBreakpoint = true;
        }
        else {
            oldBreak = std::make_pair(circleArc->site, circleArc->next->site);
            newBreak = std::make_pair(_parent->breakpoint.first, circleArc->next->site);
            isFirstBreakpoint = false;
        }

        do {
            assert(_otherParent);
            if(_otherParent->breakpoint.first == oldBreak.first && _otherParent->breakpoint.second == oldBreak.second) {
                break;
            }
            _otherParent = static_cast<InternalNode*>(_otherParent->parent);
        } while(true); //While(true) -> not possible to not find oldBreak in ascendents

        /*****/
        /*****/
        /* Lines of code copied from https://github.com/dkotsur/FortuneAlgo implementation
         * Without these lines the algorithm doesn't work properly, but i changed the epsilon value
         * With a value equals to 1.0 there are no artifacts in the diagram
         */
        double v1 = getValue(_parent), v2 = getValue(_otherParent);
        if (fabs(v1 - v2) > 1.0) {
            return nullptr;
        }
        /*****/
        /*****/

        size_t lastVertex = dcel.addVertex(Vertex(cE->getCircleCenter(), _parent->edge));

        parentEdge = _parent->edge;
        edges[_parent->edge].setOrigin(lastVertex);
        edges[_otherParent->edge].setOrigin(lastVertex);
        _otherParent->breakpoint = newBreak;
        otherEdge = _otherParent->edge;

        //Create new vertex and connect halfEdges to it
        size_t lastIndex = dcel.addHalfEdge(Voronoi::HalfEdge());
        edges[lastIndex].setTwin(lastIndex+1);
        edges[lastIndex].setOrigin(lastVertex);
        dcel.addHalfEdge(Voronoi::HalfEdge());
        edges[lastIndex+1].setTwin(lastIndex);
        _otherParent->edge = lastIndex+1;
        newEdge = _otherParent->edge;

        //Connect next and prev halfEdges
        if(isFirstBreakpoint) {
            edges[parentEdge].setPrev(edges[otherEdge].getTwinID());
            edges[edges[otherEdge].getTwinID()].setNext(parentEdge);
            edges[otherEdge].setPrev(edges[newEdge].getTwinID());
            edges[edges[newEdge].getTwinID()].setNext(otherEdge);
            edges[newEdge].setPrev(edges[parentEdge].getTwinID());
            edges[edges[parentEdge].getTwinID()].setNext(newEdge);
        } else {
            edges[parentEdge].setPrev(edges[newEdge].getTwinID());
            edges[edges[newEdge].getTwinID()].setNext(parentEdge);
            edges[otherEdge].setPrev(edges[parentEdge].getTwinID());
            edges[edges[parentEdge].getTwinID()].setNext(otherEdge);
            edges[newEdge].setPrev(edges[otherEdge].getTwinID());
            edges[edges[otherEdge].getTwinID()].setNext(newEdge);
        }

        //Update the attributes of the other nodes (parent, left, right pointers)
        if(isRight(circleArc))
            otherChild = circleArc->parent->left;    
        else
            otherChild = circleArc->parent->right;
        otherChild->parent = circleArc->parent->parent;
        if(isRight(circleArc->parent))
            circleArc->parent->parent->right = otherChild;
        else
            circleArc->parent->parent->left = otherChild;

        delete circleArc->parent;
        delete circleArc;

        rebalanceCE(otherChild);

        return prev;
    }

    /**
     * @brief Beachline::handleRotation The rotation has been optimized with respect to the fact that for each new point
     * a subtree of height 2 needs to be created. The algorithm first categorize the cases in terms of height:
     * - height 2 if _balance is empty then no rotation is necessary, because it means arc is a leaf of the root and height of root is updated
     * - height >= 2 (if _balance is not empty):
     *      - diff == 0: if first element of last position of balance is 2 only an update of parents is necessary
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
     * @param last: stores the last difference in _balance (not 0)
     */
    void Beachline::handleRotation(Node *_arc,
                                   std::vector<std::pair<int, int> > &_balance, std::vector<int> &path, int diff, int last) {
        Node* arc = _arc;
        if(!_balance.empty()) {
            updateHeight(arc);
            if(diff==0 || last == -1) {
                if(_balance.back().first != 2)
                    whichRotation(arc->right, path.back(), 1);
            } else {
                size_t psize = path.size();
                bool _else = true;
                //diff is > 1 because the height of arc->parent is higher than arc->parent->parent->other_child one
                if(psize > 2) {
                    if(/**/!(path[psize-3] == -1 && path[psize-2] == -1 && path.back() == 1 &&
                         (_balance[_balance.size()-2].first - _balance[_balance.size()-2].second == 1))/**/ &&
                            _balance.back().first - _balance.back().second == 1) {
                        whichRotation(arc, path[psize-2], path.back());
                        if(diff==2 && _balance.back().first != 2) {
                            if(path[psize-2] == path.back())
                                findPlus1AndRotate(arc, _balance, path);
                            else if (path[psize-2] == 1 && path.back() == -1)
                                findPlus1AndRotate(arc->right, _balance, path);
                            else
                                findPlus1AndRotate(arc->left, _balance, path);
                        }
                        _else = false;
                    }
                } else {
                    if(_balance.back().first - _balance.back().second == 1) {
                        whichRotation(arc, path[psize-2], path.back());
                        if(diff==2 && _balance.back().first != 2) {
                            if(path[psize-2] == path.back())
                                findPlus1AndRotate(arc, _balance, path);
                            else if (path[psize-2] == 1 && path.back() == -1)
                                findPlus1AndRotate(arc->right, _balance, path);
                            else
                                findPlus1AndRotate(arc->left, _balance, path);
                        }
                        _else = false;
                    }
                }
                if (_else){
                    //a first rotation on arc
                    if(_balance.back().first != 2) {
                        whichRotation(arc->right, path.back(), 1);
                        //a second rotation on the first +1 in balance starting from the tail of _balance - 1
                        //in case of LeftRight or RightLeft rotation in previous code line, then arc becomes arc->parent
                        path.back() == 1 ? findPlus1AndRotate(arc, _balance, path) :
                                           findPlus1AndRotate(arc->parent, _balance, path);
                    } else
                        findPlus1AndRotate(arc->parent, _balance, path);
                }
            }
            resetHeight(arc->parent);
        } else
            arc->parent->height = 3; //tree has height equals to 2 and arc is a child of the root
    }

    /**
     * @brief Beachline::rebalanceCE The sibling t of the arc q (arc that has been removed), once q has been removed,
     * has a new sibling s, and there are 2 cases to handle:
     *     - (s-t == 2): it's necessary just a rotation on s if balance of t is 0, otherwise the height of ancestors changed and need to be controlled
     *     - (s-t == 0): a rotation could be possible on an ancestor, otherwise all ancestors heights are decreased by 1
     * With a difference equals to 1 no action needs to be performed
     * @param node: is the sibling of the removed arc
     */
    void Beachline::rebalanceCE(Node* node) {
        Node* sibling;

        while(node->parent) {
            if(isRight(node))
                sibling = node->parent->left;
            else
                sibling = node->parent->right;
            switch (sibling->height - node->height) {
                case 2: {
                    int path1;
                    isRight(sibling) ? path1 = 1 : path1 = -1;

                    int bal = balance(sibling);
                    if(bal == 0) {
                        path1 == 1 ? whichRotation(sibling->right, 1, 1) : whichRotation(sibling->left, -1, -1);
                        return;
                    }
                    else if(bal > 0)
                        whichRotation(sibling->right, path1, 1);
                    else
                        whichRotation(sibling->left, path1, -1);
                    //After the rotations, the new "root" of subtree is always node->parent->parent
                    node = node->parent->parent;
                    break;
                }
                case 0: {
                    //if case is 0 node->parent->height depended on subtree containing node
                    node->parent->height -= 1;
                    node = node->parent;
                    break;
                }
                default:
                    return;
            }
        }
    }

    /**
     * @brief Beachline::rotateLeft
     * @param node as center of rotation
     */
    void Beachline::rotateLeft(Node *node) {
        //heights updates
        node->parent->height = node->parent->left ?
                    std::max(node->parent->left->height, node->left->height) + 1 : node->left->height + 1;
        node->height = std::max(node->height, node->parent->height + 1);

        //rotation algorithm
        node->parent->right = node->left;
        if(node->left)
            node->left->parent = node->parent;
        node->left = node->parent;
        Node* temp = node->parent->parent;
        node->parent->parent = node;
        node->parent = temp;

        //Update of the root
        if(!temp)
            root = node;
        else
            node->parent->left == node->left ? node->parent->left = node : node->parent->right = node;
    }

    /**
     * @brief Beachline::rotateRight
     * @param node as center of rotation
     */
    void Beachline::rotateRight(Node *node) {
        node->parent->height = node->parent->right ?
                    std::max(node->parent->right->height, node->right->height) + 1: node->right->height + 1;
        node->height = std::max(node->height, node->parent->height + 1);

        //rotation algorithm
        node->parent->left = node->right;
        if(node->right)
            node->right->parent = node->parent;
        node->right = node->parent;
        Node* temp = node->parent->parent;
        node->parent->parent = node;
        node->parent = temp;

        //Update of the root
        if(!temp)
            root = node;
        else
            node->parent->right == node->right ? node->parent->right = node : node->parent->left = node;
    }

    /**
     * @brief Beachline::rotateLeftRight
     * @param node
     */
    void Beachline::rotateLeftRight(Node *node) {
        rotateLeft(node);
        rotateRight(node);
    }

    /**
     * @brief Beachline::rotateRightLeft
     * @param node
     */
    void Beachline::rotateRightLeft(Node *node) {
        rotateRight(node);
        rotateLeft(node);
    }

    /**
     * @brief Beachline::whichRotation rotate the node with respect of the values of firstPath and secondPath
     * @param node: the node to rotate
     * @param firstPath: represent if the node is a left (-1) or right (1) child
     * @param secondPath: represent which of the children of node unbalance the tree, with -1 as left child and 1 as right child
     */
    void Beachline::whichRotation(Node *node, const int firstPath, const int secondPath) {
        if(firstPath == 1 && secondPath == 1)
            rotateLeft(node->parent);
        else if (firstPath == -1 && secondPath == -1)
            rotateRight(node->parent);
        else if (firstPath == 1 && secondPath == -1) {
            if(isLeaf(node->right) && node->parent->right->height > 1)
                rotateLeft(node->parent);
            else
                rotateRightLeft(node);
        }
        else
            rotateLeftRight(node);
    }

    /**
     * @brief Beachline::findPlus1AndRotate finds the first +1 in balance starting from the tail of _balance - 1 and perform the rotation on that node
     * @param node: the search starts from this node to the root
     * @param _balance: stores the heights of each subtree
     * @param path: stores a path to the arc with -1 means moving to left child and 1 means moving to right child
     */
    void Beachline::findPlus1AndRotate(Node *node, const std::vector<std::pair<int,int>>& _balance, const std::vector<int>& path) {
        size_t index = _balance.size()-2;
        do {
            if((_balance[index].first - _balance[index].second) == 1) {
                //the rotation is performed on the node with a balance higher of its sibling
                whichRotation(node, path[index], path[index+1]);
                return;
            }
            if(index == 0)
                return;
            index--;
            node = node->parent;
        } while(true);
    }

    /**
     * @brief Beachline::updateHeight updates the height of the parents of the node passed as parameter
     * @param node
     */
    void Beachline::updateHeight(Node *node) {
        int height = node->height;
        while(node->parent) {
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
     * @brief Beachline::resetHeight heights of parents of node are set to the right value after rotations
     * @param node
     */
    void Beachline::resetHeight(Node* node) {
        while(node) {
            node->height = std::max(node->left->height, node->right->height) + 1;
            node = node->parent;
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
     * @brief Beachline::postorder Postorder visit of the Beachline
     * @param node: root of the tree to visit
     */
    void Beachline::postorder(Node *node) const {
        if(node) {
            postorder(node->left);
            postorder(node->right);
            if(isLeaf(node)) {
                std::cout<<static_cast<Leaf*>(node)->site<<" ";
            }
            else {
                InternalNode* print = static_cast<InternalNode*>(node);
                std::cout<<"<"<<print->breakpoint.first<<"> <"<<print->breakpoint.second<<"> ";
            }
        }
    }

    /**
     * @brief Beachline::printLevelOrder print tree level by level (https://www.geeksforgeeks.org/print-level-order-traversal-line-line/)
     * @param _root
     */
    void Beachline::printLevelOrder(Node *_root) const {
        if (!_root)
            return;

        std::queue<Node*> q;

        q.push(_root);

        while (q.empty() == false)
        {
            // nodeCount (queue size) indicates number
            // of nodes at current lelvel.
            size_t nodeCount = q.size();

            while (nodeCount > 0)
            {
                Node* node = q.front();
                if(isLeaf(node)) {
                    Leaf* leaf = static_cast<Leaf*>(node);
                    std::cout<<leaf->site<<" ";
                    std::cout<<leaf<<" ";
                }
                else {
                    InternalNode* print = static_cast<InternalNode*>(node);
                    std::cout<<"<"<<print->breakpoint.first<<"> <"<<print->breakpoint.second<<"> ";
                }
                q.pop();
                if (node->left)
                    q.push(node->left);
                if (node->right)
                    q.push(node->right);
                nodeCount--;
            }
            std::cout << std::endl;
        }
    }

} 
