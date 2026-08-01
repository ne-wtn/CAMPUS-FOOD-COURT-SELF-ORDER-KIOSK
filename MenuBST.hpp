// ============================================================================
//  File        : MenuBST.hpp
//  Module      : TASK 4 - Menu Item Search and Management Module
//  Data Struct : BINARY SEARCH TREE (two co-operating trees)
//
//  WHY A BINARY SEARCH TREE?
//  -------------------------
//  The functional requirements of Task 4 are: insert an item, search an item,
//  update an item, remove an item and display every item in sorted order.
//
//      Operation        Unsorted array   Sorted array    BST
//      ---------------  --------------   -------------   -----------
//      Search by key    O(n)             O(log n)        O(log n)
//      Insert           O(1)             O(n) (shifting) O(log n)
//      Delete           O(n)             O(n) (shifting) O(log n)
//      Sorted listing   O(n log n) sort  O(n)            O(n) in-order
//
//  A sorted array searches quickly but every insert or delete must shift the
//  remaining elements. A BST is the only option that keeps ALL FIVE required
//  operations efficient at the same time, which is exactly what a kiosk menu
//  needs: prices change, items sell out, and new items are added during the
//  day, while students search the menu constantly.
//
//  WHY TWO TREES?
//  --------------
//  A BST can only be ordered by one key. The brief requires searching by ID
//  *and* by name, so this module keeps a second BST as a SECONDARY INDEX
//  (the same technique a real database uses):
//
//      idRoot   : full MenuItem records, ordered by itemID      (primary)
//      nameRoot : (lower-case name -> itemID) pairs, ordered by name (index)
//
//  The index stores only the key and the ID, never a copy of the record, so
//  the two trees can never fall out of step. A name search walks the index in
//  O(log n) and then fetches the record from the primary tree in O(log n),
//  instead of scanning all n items.
// ============================================================================

#ifndef MENU_BST_HPP
#define MENU_BST_HPP

#include "MenuItem.hpp"

// ----------------------------------------------------------------------------
//  MenuNode : a node of the PRIMARY tree. Holds the complete record.
// ----------------------------------------------------------------------------
struct MenuNode
{
    MenuItem  item;
    MenuNode* left;
    MenuNode* right;

    explicit MenuNode(const MenuItem& newItem);
};

// ----------------------------------------------------------------------------
//  NameNode : a node of the SECONDARY (name) index.
//  Two stalls may sell an item with the same name, so duplicate keys are
//  allowed: an equal key is always sent to the RIGHT subtree.
// ----------------------------------------------------------------------------
struct NameNode
{
    std::string nameKey;   // lower-case item name = the ordering key
    std::string itemID;    // pointer back into the primary tree
    NameNode*   left;
    NameNode*   right;

    NameNode(const std::string& key, const std::string& id);
};

// Selects which non-key field a filtered traversal should match on.
enum FilterField
{
    FILTER_CATEGORY,
    FILTER_STALL
};

// ----------------------------------------------------------------------------
//  MenuBST : the menu database for the whole food court.
// ----------------------------------------------------------------------------
class MenuBST
{
public:
    MenuBST();
    ~MenuBST();

    // --- Creation ----------------------------------------------------------
    // Inserts a new item. Returns false if the itemID already exists, which
    // is how the module enforces the "primary key must be unique" rule.
    bool insert(const MenuItem& item);                                  // O(h)

    // --- Searching ---------------------------------------------------------
    const MenuItem* searchByID(const std::string& itemID) const;        // O(h)
    void searchByName(const std::string& name,
                      MenuItemList& results) const;                     // O(h + k)
    void searchByPrefix(const std::string& prefix,
                        MenuItemList& results) const;                   // pruned
    void searchByField(FilterField field, const std::string& value,
                       MenuItemList& results) const;                    // O(n)
    void searchByPriceRange(double minPrice, double maxPrice,
                            MenuItemList& results) const;               // O(n)

    // --- Maintenance -------------------------------------------------------
    bool updateItem(const std::string& itemID,
                    const std::string& newName,
                    const std::string& newStall,
                    const std::string& newCategory,
                    double             newPrice,
                    bool               newAvailability);                // O(h)
    bool setAvailability(const std::string& itemID, bool isAvailable);  // O(h)
    bool remove(const std::string& itemID);                             // O(h)
    void clear();

    // --- Ordered output ----------------------------------------------------
    void toListByID(MenuItemList& output) const;      // in-order on primary
    void toListByName(MenuItemList& output) const;    // in-order on index

    // --- Tree health -------------------------------------------------------
    // Rebuilds the tree so that it is height balanced. A BST loaded from an
    // already-sorted CSV file degenerates into a linked list (height n), which
    // would turn every O(log n) search into O(n). Rebuilding from the sorted
    // in-order sequence restores the minimum possible height.
    void rebalance();

    int  size() const;
    int  height() const;
    int  optimalHeight() const;   // ceil(log2(n + 1)) - the best height possible
    bool isEmpty() const;

    // Number of key comparisons performed by the most recent search. Displayed
    // during the demonstration to prove the search really is logarithmic.
    int  getLastComparisonCount() const;

private:
    MenuNode*   idRoot;              // root of the primary tree (key = itemID)
    NameNode*   nameRoot;            // root of the secondary index (key = name)
    int         itemCount;           // number of items currently stored
    mutable int lastComparisonCount; // updated by the search routines

    // --- Primary tree helpers ---------------------------------------------
    MenuNode* insertNode(MenuNode* node, const MenuItem& item, bool& inserted);
    MenuNode* removeNode(MenuNode* node, const std::string& itemID, bool& removed);
    MenuNode* findSmallestNode(MenuNode* node) const;
    MenuNode* findNodeByID(const std::string& itemID) const;
    void      collectInOrderByID(MenuNode* node, MenuItemList& output) const;
    void      collectByField(MenuNode* node, FilterField field,
                             const std::string& value, MenuItemList& results) const;
    void      collectByPrice(MenuNode* node, double minPrice, double maxPrice,
                             MenuItemList& results) const;
    int       measureHeight(MenuNode* node) const;
    void      destroyIDTree(MenuNode* node);
    void      buildBalanced(const MenuItemList& sortedItems, int low, int high);

    // --- Secondary index helpers ------------------------------------------
    NameNode* insertNameNode(NameNode* node, const std::string& key,
                             const std::string& id);
    NameNode* removeNameNode(NameNode* node, const std::string& key,
                             const std::string& id, bool& removed);
    NameNode* findSmallestNameNode(NameNode* node) const;
    void      collectExactName(NameNode* node, const std::string& key,
                               MenuItemList& results) const;
    void      collectByPrefix(NameNode* node, const std::string& prefix,
                              MenuItemList& results) const;
    void      collectInOrderByName(NameNode* node, MenuItemList& output) const;
    void      destroyNameTree(NameNode* node);

    // Copying a tree of raw pointers would create two owners of the same
    // nodes, so copying is deliberately disabled for this class.
    MenuBST(const MenuBST& other);
    MenuBST& operator=(const MenuBST& other);
};

#endif // MENU_BST_HPP
