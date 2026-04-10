#include <iostream>
#include <queue>
#include <sstream>
#include <string>

class BinarySearchTree {
public:
	~BinarySearchTree() {
		clear(root);
	}

	bool add(int value) {
		return add(root, value);
	}

	bool remove(int value) {
		return remove(root, value);
	}

	std::string levelOrder() const {
		if (root == nullptr) {
			return "empty";
		}

		std::queue<Node*> pending;
		pending.push(root);
		std::ostringstream out;
		while (!pending.empty()) {
			Node* next = pending.front();
			pending.pop();
			out << next->value << ' ';
			if (next->left != nullptr) {
				pending.push(next->left);
			}
			if (next->right != nullptr) {
				pending.push(next->right);
			}
		}
		return out.str();
	}

private:
	struct Node {
		int value;
		Node* left = nullptr;
		Node* right = nullptr;
	};

	static bool add(Node*& node, int value) {
		if (node == nullptr) {
			node = new Node{ value };
			return true;
		}
		if (value == node->value) {
			return false;
		}
		if (value < node->value) {
			return add(node->left, value);
		}
		return add(node->right, value);
	}

	static bool remove(Node*& node, int value) {
		if (node == nullptr) {
			return false;
		}
		if (value < node->value) {
			return remove(node->left, value);
		}
		if (value > node->value) {
			return remove(node->right, value);
		}

		if (node->left == nullptr && node->right == nullptr) {
			delete node;
			node = nullptr;
			return true;
		}
		if (node->left == nullptr) {
			Node* doomed = node;
			node = node->right;
			delete doomed;
			return true;
		}
		if (node->right == nullptr) {
			Node* doomed = node;
			node = node->left;
			delete doomed;
			return true;
		}

		Node* predecessorParent = node;
		Node* predecessor = node->left;
		while (predecessor->right != nullptr) {
			predecessorParent = predecessor;
			predecessor = predecessor->right;
		}
		node->value = predecessor->value;
		if (predecessorParent == node) {
			predecessorParent->left = predecessor->left;
		}
		else {
			predecessorParent->right = predecessor->left;
		}
		delete predecessor;
		return true;
	}

	static void clear(Node* node) {
		if (node == nullptr) {
			return;
		}
		clear(node->left);
		clear(node->right);
		delete node;
	}

	Node* root = nullptr;
};

int main() {
	BinarySearchTree tree;
	for (int value : { 8, 4, 12, 2, 6, 10, 14, 5, 7 }) {
		tree.add(value);
	}
	tree.remove(4);
	tree.remove(12);

	std::cout << "BST after removals: " << tree.levelOrder() << "\n";
}

