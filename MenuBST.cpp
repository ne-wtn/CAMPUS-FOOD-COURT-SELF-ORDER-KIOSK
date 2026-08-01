// ============================================================================
//  File        : MenuBST.cpp
//  Module      : TASK 4 - Menu Item Search and Management Module
//  Description : Implementation of the menu Binary Search Tree and of the
//                secondary name index that sits beside it.
//
//  In the complexity notes below, n = number of items and h = tree height.
//  For a balanced tree h = O(log n); the rebalance() operation is provided so
//  the tree can be forced back to that ideal shape.
// ============================================================================

#include "MenuBST.hpp"

// ============================================================================
//  Node constructors
// ============================================================================

MenuNode::MenuNode(const MenuItem& newItem)
    : item(newItem), left(0), right(0)
{
}

NameNode::NameNode(const std::string& key, const std::string& id)
    : nameKey(key), itemID(id), left(0), right(0)
{
}

// ============================================================================
//  Construction and destruction
// ============================================================================

MenuBST::MenuBST()
    : idRoot(0), nameRoot(0), itemCount(0), lastComparisonCount(0)
{
}

MenuBST::~MenuBST()
{
    clear();
}

// Post-order deletion: both children are destroyed before their parent, so a
// node is never freed while it is still needed to reach the rest of the tree.
void MenuBST::destroyIDTree(MenuNode* node)
{
    if (node == 0)
    {
        return;
    }
    destroyIDTree(node->left);
    destroyIDTree(node->right);
    delete node;
}

void MenuBST::destroyNameTree(NameNode* node)
{
    if (node == 0)
    {
        return;
    }
    destroyNameTree(node->left);
    destroyNameTree(node->right);
    delete node;
}

void MenuBST::clear()
{
    destroyIDTree(idRoot);
    destroyNameTree(nameRoot);
    idRoot              = 0;
    nameRoot            = 0;
    itemCount           = 0;
    lastComparisonCount = 0;
}

// ============================================================================
//  INSERTION
// ============================================================================

// Recursive insertion into the primary tree.
// Returning the (possibly new) subtree root is what lets the parent re-link
// itself in a single assignment - no separate "parent pointer" bookkeeping.
MenuNode* MenuBST::insertNode(MenuNode* node, const MenuItem& item, bool& inserted)
{
    if (node == 0)
    {
        inserted = true;
        return new MenuNode(item);          // empty spot found - grow here
    }

    lastComparisonCount++;

    if (item.itemID < node->item.itemID)
    {
        node->left = insertNode(node->left, item, inserted);
    }
    else if (item.itemID > node->item.itemID)
    {
        node->right = insertNode(node->right, item, inserted);
    }
    else
    {
        inserted = false;                   // duplicate primary key - rejected
    }
    return node;
}

// Insertion into the secondary index. Equal names are allowed (two stalls may
// both sell "Teh Tarik"), and an equal key always goes to the RIGHT subtree.
NameNode* MenuBST::insertNameNode(NameNode* node, const std::string& key,
                                  const std::string& id)
{
    if (node == 0)
    {
        return new NameNode(key, id);
    }

    if (key < node->nameKey)
    {
        node->left = insertNameNode(node->left, key, id);
    }
    else
    {
        node->right = insertNameNode(node->right, key, id);
    }
    return node;
}

// Adds one item to the database: the record goes into the primary tree and a
// matching entry goes into the name index. Both trees stay synchronised.
bool MenuBST::insert(const MenuItem& item)
{
    bool inserted       = false;
    lastComparisonCount = 0;

    idRoot = insertNode(idRoot, item, inserted);

    if (!inserted)
    {
        return false;                       // itemID already exists
    }

    nameRoot = insertNameNode(nameRoot, TextUtil::toLower(item.name), item.itemID);
    itemCount++;
    return true;
}

// ============================================================================
//  SEARCHING
// ============================================================================

// Iterative search on the primary key. At every node one comparison discards
// an entire subtree, which is where the O(log n) behaviour comes from.
MenuNode* MenuBST::findNodeByID(const std::string& itemID) const
{
    MenuNode* current = idRoot;

    while (current != 0)
    {
        lastComparisonCount++;

        if (itemID < current->item.itemID)
        {
            current = current->left;        // discard the right subtree
        }
        else if (itemID > current->item.itemID)
        {
            current = current->right;       // discard the left subtree
        }
        else
        {
            return current;                 // exact match
        }
    }
    return 0;                               // key is not in the tree
}

const MenuItem* MenuBST::searchByID(const std::string& itemID) const
{
    lastComparisonCount = 0;

    MenuNode* found = findNodeByID(itemID);
    if (found == 0)
    {
        return 0;
    }
    return &(found->item);
}

// Collects every index entry whose key equals 'key'.
// Duplicates are stored down the right-hand path, so the walk simply carries
// on after a hit instead of stopping at the first one.
void MenuBST::collectExactName(NameNode* node, const std::string& key,
                               MenuItemList& results) const
{
    NameNode* current = node;

    while (current != 0)
    {
        lastComparisonCount++;

        if (key < current->nameKey)
        {
            current = current->left;
        }
        else if (key > current->nameKey)
        {
            current = current->right;
        }
        else
        {
            MenuNode* record = findNodeByID(current->itemID);
            if (record != 0)
            {
                results.add(record->item);
            }
            current = current->right;       // keep looking for duplicates
        }
    }
}

void MenuBST::searchByName(const std::string& name, MenuItemList& results) const
{
    lastComparisonCount = 0;
    results.clear();
    collectExactName(nameRoot, TextUtil::toLower(TextUtil::trim(name)), results);
}

// ----------------------------------------------------------------------------
//  PREFIX SEARCH (the "type-ahead" feature of the kiosk)
//
//  Because the index is sorted by name, whole subtrees can be pruned:
//    prefix < node's opening characters -> every match must be on the LEFT
//    prefix > node's opening characters -> every match must be on the RIGHT
//    equal                              -> this node matches, and further
//                                          matches may sit on BOTH sides
//  A plain array would have to test all n names; this visits only the band of
//  nodes that can possibly match.
// ----------------------------------------------------------------------------
void MenuBST::collectByPrefix(NameNode* node, const std::string& prefix,
                              MenuItemList& results) const
{
    if (node == 0)
    {
        return;
    }

    lastComparisonCount++;
    int comparison = TextUtil::comparePrefix(prefix, node->nameKey);

    if (comparison < 0)
    {
        collectByPrefix(node->left, prefix, results);        // prune the right
    }
    else if (comparison > 0)
    {
        collectByPrefix(node->right, prefix, results);       // prune the left
    }
    else
    {
        // In-order around the matching node keeps the results alphabetical.
        collectByPrefix(node->left, prefix, results);

        MenuNode* record = findNodeByID(node->itemID);
        if (record != 0)
        {
            results.add(record->item);
        }

        collectByPrefix(node->right, prefix, results);
    }
}

void MenuBST::searchByPrefix(const std::string& prefix, MenuItemList& results) const
{
    lastComparisonCount = 0;
    results.clear();

    std::string key = TextUtil::toLower(TextUtil::trim(prefix));
    if (key.empty())
    {
        return;                             // an empty prefix matches nothing
    }
    collectByPrefix(nameRoot, key, results);
}

// ----------------------------------------------------------------------------
//  Filtered traversal on a NON-KEY field (category or stall).
//  The tree is not ordered by these fields, so no pruning is possible and the
//  whole tree must be visited: O(n). An in-order walk is still used so that
//  the results come out sorted by Item ID.
// ----------------------------------------------------------------------------
void MenuBST::collectByField(MenuNode* node, FilterField field,
                             const std::string& value, MenuItemList& results) const
{
    if (node == 0)
    {
        return;
    }

    collectByField(node->left, field, value, results);

    lastComparisonCount++;
    const std::string& fieldValue = (field == FILTER_CATEGORY)
                                        ? node->item.category
                                        : node->item.stallName;
    if (TextUtil::equalsIgnoreCase(fieldValue, value))
    {
        results.add(node->item);
    }

    collectByField(node->right, field, value, results);
}

void MenuBST::searchByField(FilterField field, const std::string& value,
                            MenuItemList& results) const
{
    lastComparisonCount = 0;
    results.clear();
    collectByField(idRoot, field, TextUtil::trim(value), results);
}

void MenuBST::collectByPrice(MenuNode* node, double minPrice, double maxPrice,
                             MenuItemList& results) const
{
    if (node == 0)
    {
        return;
    }

    collectByPrice(node->left, minPrice, maxPrice, results);

    lastComparisonCount++;
    if (node->item.price >= minPrice && node->item.price <= maxPrice)
    {
        results.add(node->item);
    }

    collectByPrice(node->right, minPrice, maxPrice, results);
}

void MenuBST::searchByPriceRange(double minPrice, double maxPrice,
                                 MenuItemList& results) const
{
    lastComparisonCount = 0;
    results.clear();
    collectByPrice(idRoot, minPrice, maxPrice, results);
}

// ============================================================================
//  UPDATING
// ============================================================================

// Updates every editable field of one item.
// The itemID is never changed: it is the primary key and it decides where the
// node sits in the tree. If the NAME changes, the secondary index entry must
// be deleted and re-inserted, otherwise the index would still be sorted under
// the old name and name searches would silently fail.
bool MenuBST::updateItem(const std::string& itemID,
                         const std::string& newName,
                         const std::string& newStall,
                         const std::string& newCategory,
                         double             newPrice,
                         bool               newAvailability)
{
    lastComparisonCount = 0;

    MenuNode* target = findNodeByID(itemID);
    if (target == 0)
    {
        return false;
    }

    std::string oldKey = TextUtil::toLower(target->item.name);
    std::string newKey = TextUtil::toLower(newName);

    if (oldKey != newKey)
    {
        bool removed = false;
        nameRoot = removeNameNode(nameRoot, oldKey, itemID, removed);
        nameRoot = insertNameNode(nameRoot, newKey, itemID);
    }

    target->item.name      = newName;
    target->item.stallName = newStall;
    target->item.category  = newCategory;
    target->item.price     = newPrice;
    target->item.available = newAvailability;
    return true;
}

// Marks an item as sold out / back on sale. The key and the name are not
// touched, so neither tree has to be restructured.
bool MenuBST::setAvailability(const std::string& itemID, bool isAvailable)
{
    lastComparisonCount = 0;

    MenuNode* target = findNodeByID(itemID);
    if (target == 0)
    {
        return false;
    }
    target->item.available = isAvailable;
    return true;
}

// ============================================================================
//  DELETION
// ============================================================================

// The left-most node of a subtree holds its smallest key.
MenuNode* MenuBST::findSmallestNode(MenuNode* node) const
{
    MenuNode* current = node;
    while (current != 0 && current->left != 0)
    {
        current = current->left;
    }
    return current;
}

// Classic BST deletion, covering all three cases:
//   Case 1 - leaf              : detach and delete it.
//   Case 2 - one child         : the child takes the node's place.
//   Case 3 - two children      : copy the IN-ORDER SUCCESSOR (smallest key of
//                                the right subtree) into this node, then delete
//                                that successor from the right subtree. The
//                                successor is chosen because it is the only
//                                key that is still larger than everything on
//                                the left and smaller than everything else on
//                                the right, so the BST ordering survives.
MenuNode* MenuBST::removeNode(MenuNode* node, const std::string& itemID, bool& removed)
{
    if (node == 0)
    {
        removed = false;
        return 0;                                   // key not found
    }

    lastComparisonCount++;

    if (itemID < node->item.itemID)
    {
        node->left = removeNode(node->left, itemID, removed);
    }
    else if (itemID > node->item.itemID)
    {
        node->right = removeNode(node->right, itemID, removed);
    }
    else
    {
        removed = true;

        if (node->left == 0)                        // covers case 1 and case 2
        {
            MenuNode* replacement = node->right;
            delete node;
            return replacement;
        }
        if (node->right == 0)                       // one child on the left
        {
            MenuNode* replacement = node->left;
            delete node;
            return replacement;
        }

        MenuNode* successor = findSmallestNode(node->right);   // case 3
        node->item = successor->item;

        bool successorRemoved = false;
        node->right = removeNode(node->right, successor->item.itemID, successorRemoved);
    }
    return node;
}

NameNode* MenuBST::findSmallestNameNode(NameNode* node) const
{
    NameNode* current = node;
    while (current != 0 && current->left != 0)
    {
        current = current->left;
    }
    return current;
}

// Deletes ONE entry from the name index. Because duplicate names are allowed,
// the itemID is used as a tie-breaker: on an equal key whose ID does not match,
// the search continues to the right, which is where duplicates were inserted.
NameNode* MenuBST::removeNameNode(NameNode* node, const std::string& key,
                                  const std::string& id, bool& removed)
{
    if (node == 0)
    {
        removed = false;
        return 0;
    }

    if (key < node->nameKey)
    {
        node->left = removeNameNode(node->left, key, id, removed);
        return node;
    }
    if (key > node->nameKey || node->itemID != id)
    {
        node->right = removeNameNode(node->right, key, id, removed);
        return node;
    }

    removed = true;

    if (node->left == 0)
    {
        NameNode* replacement = node->right;
        delete node;
        return replacement;
    }
    if (node->right == 0)
    {
        NameNode* replacement = node->left;
        delete node;
        return replacement;
    }

    NameNode* successor = findSmallestNameNode(node->right);
    node->nameKey = successor->nameKey;
    node->itemID  = successor->itemID;

    bool successorRemoved = false;
    node->right = removeNameNode(node->right, successor->nameKey,
                                 successor->itemID, successorRemoved);
    return node;
}

// Removes an item from BOTH trees. The name is read before the record is
// deleted, because after deletion the index key could no longer be worked out.
bool MenuBST::remove(const std::string& itemID)
{
    lastComparisonCount = 0;

    MenuNode* target = findNodeByID(itemID);
    if (target == 0)
    {
        return false;
    }

    std::string key = TextUtil::toLower(target->item.name);

    bool nameRemoved = false;
    nameRoot = removeNameNode(nameRoot, key, itemID, nameRemoved);

    bool recordRemoved = false;
    idRoot = removeNode(idRoot, itemID, recordRemoved);

    if (recordRemoved)
    {
        itemCount--;
    }
    return recordRemoved;
}

// ============================================================================
//  ORDERED OUTPUT
// ============================================================================

// In-order traversal (left -> node -> right) visits a BST in ascending key
// order, so the sorted listing needs no sorting algorithm at all: O(n).
void MenuBST::collectInOrderByID(MenuNode* node, MenuItemList& output) const
{
    if (node == 0)
    {
        return;
    }
    collectInOrderByID(node->left, output);
    output.add(node->item);
    collectInOrderByID(node->right, output);
}

void MenuBST::toListByID(MenuItemList& output) const
{
    output.clear();
    collectInOrderByID(idRoot, output);
}

// In-order on the index gives the items in alphabetical order. Each entry is
// resolved back to its record through the primary tree.
void MenuBST::collectInOrderByName(NameNode* node, MenuItemList& output) const
{
    if (node == 0)
    {
        return;
    }
    collectInOrderByName(node->left, output);

    MenuNode* record = findNodeByID(node->itemID);
    if (record != 0)
    {
        output.add(record->item);
    }

    collectInOrderByName(node->right, output);
}

void MenuBST::toListByName(MenuItemList& output) const
{
    lastComparisonCount = 0;
    output.clear();
    collectInOrderByName(nameRoot, output);
}

// ============================================================================
//  TREE HEALTH
// ============================================================================

// Height measured in nodes: an empty tree is 0, a single node is 1.
int MenuBST::measureHeight(MenuNode* node) const
{
    if (node == 0)
    {
        return 0;
    }

    int leftHeight  = measureHeight(node->left);
    int rightHeight = measureHeight(node->right);

    return 1 + ((leftHeight > rightHeight) ? leftHeight : rightHeight);
}

int MenuBST::height() const
{
    return measureHeight(idRoot);
}

// Smallest height that itemCount nodes could possibly occupy. Level 1 holds
// 1 node, level 2 holds 2, level 3 holds 4 ... so the levels are filled until
// the nodes run out. Comparing this with height() shows how skewed the tree is.
int MenuBST::optimalHeight() const
{
    int levels        = 0;
    int nodesLeft     = itemCount;
    int nodesPerLevel = 1;

    while (nodesLeft > 0)
    {
        levels++;
        nodesLeft     -= nodesPerLevel;
        nodesPerLevel *= 2;
    }
    return levels;
}

// Rebuilds a perfectly balanced tree from the sorted sequence: the middle
// element becomes the root, the left half becomes the left subtree and the
// right half becomes the right subtree, applied recursively.
void MenuBST::buildBalanced(const MenuItemList& sortedItems, int low, int high)
{
    if (low > high)
    {
        return;
    }

    int middle = low + (high - low) / 2;
    insert(sortedItems.get(middle));            // rebuilds both trees together

    buildBalanced(sortedItems, low, middle - 1);
    buildBalanced(sortedItems, middle + 1, high);
}

void MenuBST::rebalance()
{
    if (itemCount <= 2)
    {
        return;                                 // already as short as possible
    }

    MenuItemList sortedItems;
    toListByID(sortedItems);                    // in-order = sorted by itemID

    clear();                                    // discard the skewed shape
    buildBalanced(sortedItems, 0, sortedItems.size() - 1);
}

int MenuBST::size() const
{
    return itemCount;
}

bool MenuBST::isEmpty() const
{
    return itemCount == 0;
}

int MenuBST::getLastComparisonCount() const
{
    return lastComparisonCount;
}
