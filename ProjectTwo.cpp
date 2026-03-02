//============================================================================
// Name        : ProjecTwo.cpp
// Author      : Rob McGuine
// Version     : 1.0
// Copyright   : Copyright � 2026 Rob McGuine
// Description : Project Two ABCU Program
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

struct Course {
    string courseId;
    string title;
    vector<string> prerequisites;

    Course() {}
};

struct Node {
    Course course;
    Node* left;
    Node* right;

    Node() : left(nullptr), right(nullptr) {}
    Node(Course aCourse) : Node() {
        this->course = aCourse;
    }
};

//============================================================================
// Binary Search Tree class definition
//============================================================================

class BinarySearchTree {
private:
    Node* root;
    int size;

    void addNode(Node* node, Course course);
    void inOrder(Node* node);
    void destroyTree(Node* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void Insert(Course course);
    void PrintSampleSchedule();
    Course Search(string courseId);
    int Size() { return size; }
};

BinarySearchTree::BinarySearchTree() {
    root = nullptr;
    size = 0;
}

BinarySearchTree::~BinarySearchTree() {
    destroyTree(root);
}

void BinarySearchTree::destroyTree(Node* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

void BinarySearchTree::Insert(Course course) {
    if (root == nullptr) {
        root = new Node(course);
    } else {
        addNode(root, course);
    }
    size++;
}

void BinarySearchTree::addNode(Node* node, Course course) {
    // Standard BST insertion logic using alphanumeric comparison
    if (node->course.courseId.compare(course.courseId) > 0) {
        if (node->left == nullptr) {
            node->left = new Node(course);
        } else {
            addNode(node->left, course);
        }
    } else {
        if (node->right == nullptr) {
            node->right = new Node(course);
        } else {
            addNode(node->right, course);
        }
    }
}

// Requirement 2: Alphanumeric print using In-Order Traversal
void BinarySearchTree::PrintSampleSchedule() {
    this->inOrder(root);
}

void BinarySearchTree::inOrder(Node* node) {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->course.courseId << ", " << node->course.title << endl;
        inOrder(node->right);
    }
}

// Requirement 3: Search for specific course
Course BinarySearchTree::Search(string courseId) {
    Node* curr = root;
    while (curr != nullptr) {
        if (curr->course.courseId.compare(courseId) == 0) {
            return curr->course;
        } else if (courseId.compare(curr->course.courseId) < 0) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    return Course(); // Return empty course if not found
}

// --- Helper Functions ---

void loadCourses(string csvPath, BinarySearchTree* bst) {
    ifstream file(csvPath);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << csvPath << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        vector<string> tokens;

        // Requirement: Robust parsing using stringstream
        while (getline(ss, token, ',')) {
            // Remove carriage returns or whitespace that might break IDs
            token.erase(remove(token.begin(), token.end(), '\r'), token.end());
            tokens.push_back(token);
        }

        // Basic validation: must have ID and Title
        if (tokens.size() < 2) continue;

        Course course;
        course.courseId = tokens[0];
        course.title = tokens[1];

        for (size_t i = 2; i < tokens.size(); i++) {
            // Check if the token is not empty and not just a newline/carriage return
            // Pesky commas in csv!
            if (!tokens[i].empty() && tokens[i] != "\r" && tokens[i] != "\n") {
                course.prerequisites.push_back(tokens[i]);
            }
        }
        bst->Insert(course);
    }
    cout << bst->Size() << " courses loaded." << endl;
    file.close();
}

//============================================================================
// Main Method
//============================================================================

int main() {
    BinarySearchTree* bst = new BinarySearchTree();
    string csvPath = "CS 300 ABCU_Advising_Program_Input.csv";
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << "\n1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit." << endl;
        cout << "\nWhat would you like to do? ";
        
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number." << endl;

            // clear error state of cin
            cin.clear();

            cin.ignore(100, '\n'); // 100 is an arbitrary value to help cleanup input buffer (100 would be max number of chars to ignore)
            // Alternative solution would be to #include <limits> header and do something like cin.ignore(numeric_limits<streamsize>::max(), '\n');

            continue;
        }

        switch (choice) {
            case 1:
                loadCourses(csvPath, bst);
                break;

            case 2:
                if (bst->Size() == 0) {
                    cout << "Error: Load data first (Option 1)." << endl;
                } else {
                    cout << "Here is a sample schedule:\n" << endl;
                    bst->PrintSampleSchedule();
                }
                break;

            case 3: {
                if (bst->Size() == 0) {
                    cout << "Error: Load data first (Option 1)." << endl;
                    break;
                }
                string searchId;
                cout << "What course do you want to know about? ";
                cin >> searchId;
                
                // Convert search to uppercase for consistency
                transform(searchId.begin(), searchId.end(), searchId.begin(), ::toupper);
                
                Course course = bst->Search(searchId);
                if (!course.courseId.empty()) {
                    cout << course.courseId << ", " << course.title << endl;
                    cout << "Prerequisites: ";

                    if (course.prerequisites.empty()) {
                        cout << "None";
                    } else {
                        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
                            cout << course.prerequisites[i];
                            // Only add a comma if it's NOT the last item in the vector
                            if (i < course.prerequisites.size() - 1) {
                                cout << ", ";
                            }
                        }
                    }
                    cout << endl;
                } else {
                    cout << "Course not found." << endl;
                }
                break;
            }

            case 9:
                cout << "Thank you for using the course planner!" << endl;
                break;

            default:
                cout << choice << " is not a valid option." << endl;
                break;
        }
    }

    delete bst;
    return 0;
}