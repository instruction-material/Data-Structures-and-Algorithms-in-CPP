#include <algorithm>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>

class AvlTreeStarter {
public:
	~AvlTreeStarter() {
		clear(root);
	}

	void add(int value) {
		add(root, value);
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
			out << next->value << "(h=" << next->height << ") ";
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
		int height = 1;
		Node* left = nullptr;
		Node* right = nullptr;
	};

	static void add(Node*& node, int value) {
		if (node == nullptr) {
			node = new Node{ value };
			return;
		}
		if (value == node->value) {
			return;
		}
		if (value < node->value) {
			add(node->left, value);
		}
		else {
			add(node->right, value);
		}
		node->height = 1 + std::max(height(node->left), height(node->right));
		// TODO: detect imbalance and rotate instead of leaving the tree as a plain BST.
	}

	static int height(Node* node) {
		return node == nullptr ? 0 : node->height;
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
	AvlTreeStarter tree;
	for (int value : { 30, 20, 10, 25, 40, 50 }) {
		tree.add(value);
	}

	std::cout << "Starter AVL view: " << tree.levelOrder() << "\n";
}

