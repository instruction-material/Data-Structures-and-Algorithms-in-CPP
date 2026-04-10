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
		(void) value;
		// TODO: implement recursive removal using the in-order predecessor.
		return false;
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
	for (int value : { 8, 4, 12, 2, 6, 10, 14 }) {
		tree.add(value);
	}

	std::cout << "Starter BST: " << tree.levelOrder() << "\n";
}

