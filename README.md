# Campus Food Court Self-Order Kiosk System
**CT077-3-2-DSTR — Lab Evaluation Work #2 — Group `<number>`**

This is the shared codebase for our group. Read this file **before** you write any code.
It exists so that four people can work on one C++ program at the same time without breaking each other's work.

---

## 1. Quick start (5 minutes)

You need **g++** (MinGW-w64 on Windows). Check it works:

```
g++ --version
```

Build and run:

| | Command |
|---|---|
| Windows (double-click or terminal) | `build.bat` |
| Windows, build + run | `build.bat run` |
| macOS / Linux / WSL | `make run` |
| VS Code | press **Ctrl+Shift+B** |
| By hand | `g++ -std=c++11 -Wall -Wextra *.cpp -o kiosk.exe` |

Run it:

```
kiosk.exe            full integrated system
kiosk.exe 4          jump straight into Task 4 (use this for your recording)
```

**If the build fails on a fresh clone, tell the group immediately.** The rule is that `main` always compiles.

---

## 2. Who owns what

| Task | Module | Data structure | Owner | Files you create |
|---|---|---|---|---|
| 1 | Order Queue Management | **Queue** (FIFO) | `<name / TP>` | `OrderQueue.hpp/.cpp` + fill in `OrderModule.hpp/.cpp` |
| 2 | Stall Assignment | **Circular Queue** | `<name / TP>` | `StallCircularQueue.hpp/.cpp` + fill in `StallModule.hpp/.cpp` |
| 3 | Session History & Navigation | **Stack** (LIFO) | `<name / TP>` | `SessionStack.hpp/.cpp` + fill in `SessionModule.hpp/.cpp` |
| 4 | Menu Item Search & Management | **Binary Search Tree** | `<name / TP>` | ✅ done — `MenuItem.*`, `MenuBST.*`, `MenuModule.*` |
| 5 | Food Court Layout *(optional)* | **Tree** | `<name / TP>` or nobody | `LayoutTree.hpp/.cpp` + fill in `LayoutModule.hpp/.cpp` |

Queue, Circular Queue, Stack and BST are **compulsory** — each must appear at least once across the team. Task 5 is optional; if nobody takes it, leave the skeleton exactly as it is and the system still builds.

**Shared files (leader owns, nobody edits alone):** `Common.hpp/.cpp`, `KioskSystem.hpp/.cpp`, `main.cpp`, `README.md`, `build.bat`, `Makefile`.

---

## 3. The five golden rules

1. **Only edit files you own.** If you need something changed in someone else's file, message them — do not edit it yourself.
2. **No STL containers.** No `<vector>`, `<list>`, `<deque>`, `<map>`, `<set>`, `<queue>`, `<stack>`, `<array>`, `<forward_list>`, and no `std::sort`. Every container is written by hand. (`std::string`, `<iostream>`, `<iomanip>`, `<fstream>`, `<sstream>`, `<cmath>` are fine — they are not containers.)
3. **There is exactly one `main()`**, in `main.cpp`. A second one anywhere breaks the build for everyone with "multiple definition of main".
4. **Never change a contract method's name or signature** without telling the group first (section 6).
5. **Keep the folder flat.** No subfolders for source files — Moodle wants the `.cpp`, `.hpp` and `.csv` files uploaded individually, so they all live in the root.

---

## 4. File map

```
LabWork2/
├── main.cpp                 entry point                       [leader]
├── KioskSystem.hpp/.cpp     integration + main menu           [leader]
├── Common.hpp/.cpp          Order, TextUtil, CsvUtil, ConsoleUI  [leader]
│
├── OrderModule.hpp/.cpp     Task 1  ← skeleton, fill me in
├── StallModule.hpp/.cpp     Task 2  ← skeleton, fill me in
├── SessionModule.hpp/.cpp   Task 3  ← skeleton, fill me in
├── MenuModule.hpp/.cpp      Task 4  ✅ complete
├── MenuBST.hpp/.cpp         Task 4  ✅ the BST itself
├── MenuItem.hpp/.cpp        Task 4  ✅ record + dynamic array
├── LayoutModule.hpp/.cpp    Task 5  ← optional skeleton
│
├── menu_items.csv           Task 4 data  ✅
├── stalls.csv               Task 2 data
├── orders.csv               Task 1 data
├── layout.csv               Task 5 data
│
├── build.bat / Makefile     build scripts
├── .vscode/                 VS Code build + debug config
└── README.md                this file
```

**Naming convention:** `<Thing>Module.*` = the screens and file handling for one task. `<Thing>Queue / Stack / BST / Tree.*` = the data structure itself. Keep the two separate, the way Task 4 splits `MenuBST` (pure data structure) from `MenuModule` (menus and CSV). It reads better and it is much easier to explain in your recording.

---

## 5. How to build your module (step by step)

1. **Open your skeleton** — e.g. `OrderModule.hpp` / `OrderModule.cpp`. Put your **name and TP number** in the header comment at the top.
2. **Create your data structure files** — e.g. `OrderQueue.hpp` / `OrderQueue.cpp`. This is where your queue/stack/tree lives, with no `cout` in it.
3. **Include `Common.hpp`** and use what is already there:
   * `Order`, `OrderLine`, `OrderStatus` — the record that travels between modules
   * `ConsoleUI::readInteger / readYesNo / readRequiredText / printLine / pause` — validated input, so your menu can never get stuck in an infinite loop
   * `CsvUtil::nextField` — CSV field splitting
   * `TextUtil::trim / toLower / toText / formatPrice`
4. **Write your module's sub-menu** in `run()`. Copy the layout style from `MenuModule::displayMenuOptions()` so all five modules look the same.
5. **Fill in the contract methods** (the ones `KioskSystem.cpp` already calls). Return real values instead of the stub `false`.
6. **Build and test after every method.** `build.bat` takes two seconds — use it constantly.
7. **Add a comment above every function** saying what it does and its Big-O. You will be asked about this in the Q&A.

**Look at Task 4 for the pattern** — `MenuBST.cpp` is a worked example of a fully documented data structure with edge cases handled.

---

## 6. The integration contract

`KioskSystem.cpp` is the **only** place where one module calls another. Inside your own module you never include another member's header.

These methods already have calls written against them. Implement them; don't rename them.

```cpp
// Task 1 - OrderModule
bool enqueueOrder(const Order& order);       // order joins the queue
bool peekNextOrder(Order& output) const;     // who is served next
bool dequeueNextOrder(Order& output);        // remove the front order
bool completeOrder(int orderID);             // mark fulfilled
int  pendingCount() const;
int  completedCount() const;
void displayPendingOrders() const;

// Task 2 - StallModule
bool assignNextStall(Order& order);          // rotate, skip closed/full,
                                             // write order.assignedStall
bool releaseStall(const std::string& stallName);
void displayStallStatus() const;
int  availableStallCount() const;
int  stallCount() const;

// Task 3 - SessionModule
void startSession(const std::string& studentID);
void recordStep(const std::string& description);
bool goBack(std::string& restoredStep);      // the back button
void endSession();
bool hasActiveSession() const;
int  stepCount() const;
void displayHistory() const;
std::string currentStudentID() const;

// Task 4 - MenuModule  (done)
const MenuBST& getDatabase() const;          // searchByID, searchByPrefix, ...
```

Every module also has `bool loadFromCSV(const std::string&)`, `bool saveToCSV(const std::string&) const` and `void run()`.

**Returning `false` is fine while you are still working** — the system reports "[Task N pending]" and carries on. That is how the project stays runnable for everybody from day one.

### The end-to-end flow (the 7 steps in the brief)
```
1. student scans ID          → SessionModule::startSession()
2. order created and queued  → MenuModule lookup + OrderModule::enqueueOrder()
3. stall assigned by rotation→ StallModule::assignNextStall()
4. menu item located         → MenuModule::getDatabase().searchByID()
5. session steps recorded    → SessionModule::recordStep()
6. student navigates back    → SessionModule::goBack()
7. order fulfilled + removed → OrderModule::completeOrder()
```
Try it now: run `kiosk.exe`, choose **1**, type `nasi`, and watch Task 4 feed the order. The steps that print "[Task N pending]" are the ones still waiting on their owner.

---

## 7. Coding standards (part of the marks)

The rubric explicitly grades *"clarity and structural design of the code, quality of comments, indentation, meaningful identifier names"*. Match the existing style:

* **4 spaces** for indentation, never tabs. Braces on their own line.
* `camelCase` for variables and functions, `PascalCase` for classes and structs, `UPPER_CASE` for constants.
* **Meaningful names.** `currentStallIndex`, not `i2`. `assignNextStall()`, not `doIt()`.
* **A header comment on every file**: file name, module, owner, description.
* **A comment above every function** — what it does, and *why* it is written that way. The Big-O belongs here too.
* **Header guards** in every `.hpp`: `#ifndef ORDER_QUEUE_HPP` / `#define` / `#endif`.
* Keep functions short. If one does not fit on a screen, split it.
* **Delete what you `new`.** Every data structure needs a destructor that frees its nodes.
* Build with `-Wall -Wextra` and fix every warning. Warnings are bugs you have not met yet.

---

## 8. Working together without breaking things

### Recommended: a private GitHub repo
```bash
# Leader, once:
git init
git add .
git commit -m "Shared codebase: Common, KioskSystem, Task 4, skeletons"
git branch -M main
git remote add origin https://github.com/<leader>/dstr-labwork2.git
git push -u origin main
# then add the other three as collaborators on GitHub

# Everyone else, once:
git clone https://github.com/<leader>/dstr-labwork2.git
cd dstr-labwork2

# Every working session:
git pull                      # ALWAYS pull before you start
# ...write code, build, test...
git add OrderModule.cpp OrderQueue.cpp OrderQueue.hpp    # only YOUR files
git commit -m "Task 1: enqueue and dequeue working"
git push
```
Because everyone edits different files, conflicts are almost impossible. **Never `git add .`** — that is how you commit someone else's half-finished work or your `kiosk.exe`.

### If the group will not use git
1. One shared folder (Google Drive / OneDrive) holds the **master copy**.
2. You work on your **own local copy**, never directly in the shared folder.
3. When a feature works and builds, copy **only your own files** into the shared folder and message the group.
4. Before you start each session, copy the master copy down again.
5. **Never** put `kiosk.exe` or `.o` files in the shared folder.

⚠️ Do not let OneDrive/Drive sync the folder live while two people edit — it silently creates "conflicted copy" files and you will lose work.

---

## 9. "Done" checklist for your module

Before you say your task is finished:

- [ ] It builds with `-Wall -Wextra` and **zero warnings**
- [ ] Your data structure is hand-written; no STL container anywhere
- [ ] Insert / remove / search / display all work from your sub-menu
- [ ] **Edge cases handled and demonstrated:** empty structure, full structure, item not found, duplicate entry, invalid typing at every prompt
- [ ] Every contract method in section 6 returns real data
- [ ] Data loads from your `.csv` and saves back to it
- [ ] Destructor frees everything you allocated
- [ ] Every function has a comment; your file headers carry your name and TP number
- [ ] You can **explain every line out loud** without reading from the screen
- [ ] You can answer: *why this data structure, and what is its Big-O for each operation?*

---

## 10. Submission checklist (Monday of Week 14, before 5:00 pm)

**A — Group code submission**
- [ ] Every `.cpp`, `.hpp` and `.csv` uploaded **individually** — the brief says *do not compress or zip these files* for this part
- [ ] The leader also uploads the ZIP named exactly:
      `G<no>_<TPleader>_<TPmember1>_<TPmember2>_<TPmember3>.zip`
      e.g. `G1_TP012345_TP012344_TP012123_TP012111.zip`
- [ ] **No `.exe`, no `.o`** in either submission
- [ ] `main.cpp` header lists the group number and every member with their task
- [ ] A fresh copy of the folder builds and runs on a machine that has never seen it

**B — Individual recording (30 marks)**
- [ ] ~5 minutes each, 30 minutes total for the team — anything past 30 minutes is not marked
- [ ] Cover: what your module does → why you chose that data structure → a live demo → your algorithms and their complexity → edge cases
- [ ] No PowerPoint required — screen record the code and the running program
- [ ] Use `kiosk.exe <your task number>` so you land straight in your module
- [ ] Uploaded to the Moodle link. **No recording = 0 marks for this assignment.**

---

## 11. AI usage policy — read this

This is an **Amber** assessment. AI may be used to *understand* concepts, syntax, error messages and terminology, and to polish comments. It may **not** be used to generate code, algorithms, program logic, functions, classes or data structure implementations.

You must be able to **explain and justify every line** you submit — the lecturer may ask you to trace code live or answer questions on the spot. If there is a line in your file you cannot explain, rewrite it until you can.

If you used AI for any permitted purpose, declare it accurately.
