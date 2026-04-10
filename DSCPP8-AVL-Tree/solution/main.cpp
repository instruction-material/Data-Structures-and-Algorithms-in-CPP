#include <algorithm>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>

class AvlTree {
public:
	~AvlTree() {
		clear(root);
	}

	void add(int value) {
		root = insert(root, value);
	}

	void remove(int value) {
		root = erase(root, value);
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

	static int height(Node* node) {
		return node == nullptr ? 0 : node->height;
	}

	static int balance(Node* node) {
		return node == nullptr ? 0 : height(node->left) - height(node->right);
	}

	static void refresh(Node* node) {
		node->height = 1 + std::max(height(node->left), height(node->right));
	}

	static Node* rotateRight(Node* y) {
		Node* x = y->left;
		Node* transfer = x->right;
		x->right = y;
		y->left = transfer;
		refresh(y);
		refresh(x);
		return x;
	}

	static Node* rotateLeft(Node* x) {
		Node* y = x->right;
		Node* transfer = y->left;
		y->left = x;
		x->right = transfer;
		refresh(x);
		refresh(y);
		return y;
	}

	static Node* rebalance(Node* node) {
		refresh(node);
		const int nodeBalance = balance(node);
		if (nodeBalance > 1) {
			if (balance(node->left) < 0) {
				node->left = rotateLeft(node->left);
			}
			return rotateRight(node);
		}
		if (nodeBalance < -1) {
			if (balance(node->right) > 0) {
				node->right = rotateRight(node->right);
			}
			return rotateLeft(node);
		}
		return node;
	}

	static Node* insert(Node* node, int value) {
		if (node == nullptr) {
			return new Node{ value };
		}
		if (value == node->value) {
			return node;
		}
		if (value < node->value) {
			node->left = insert(node->left, value);
		}
		else {
			node->right = insert(node->right, value);
		}
		return rebalance(node);
	}

	static Node* erase(Node* node, int value) {
		if (node == nullptr) {
			return nullptr;
		}
		if (value < node->value) {
			node->left = erase(node->left, value);
		}
		else if (value > node->value) {
			node->right = erase(node->right, value);
		}
		else {
			if (node->left == nullptr || node->right == nullptr) {
				Node* replacement = node->left != nullptr ? node->left : node->right;
				delete node;
				return replacement;
			}

			Node* predecessor = node->left;
			while (predecessor->right != nullptr) {
				predecessor = predecessor->right;
			}
			node->value = predecessor->value;
			node->left = erase(node->left, predecessor->value);
		}

		if (node == nullptr) {
			return nullptr;
		}
		return rebalance(node);
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
	AvlTree tree;
	for (int value : { 30, 20, 10, 25, 40, 50, 22 }) {
		tree.add(value);
	}
	tree.remove(40);
	tree.remove(20);

	std::cout << "Balanced AVL view: " << tree.levelOrder() << "\n";
}

