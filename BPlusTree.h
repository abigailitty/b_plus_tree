/* ASSUMPTIONS:
1. When a node must be split and has an odd number of keys, the extra key will go to the new right-hand node.
2. There will never be a duplicate key passed to insert.
*/

#include <vector>

#ifndef DS_BPLUSTREE
#define DS_BPLUSTREE

using namespace std;
///////////////////////////////////////////////////////////////////////////////
//DO NOT CHANGE THIS CODE
///////////////////////////////////////////////////////////////////////////////

//Do not implement the class here, this is a forward declaration. Implement at
//the bottom of the .h file
template <class T> class BPlusTree; 

template <class T>
class BPlusTreeNode{
public:
	BPlusTreeNode() : parent(NULL) {};
	bool is_leaf();
	bool contains(const T& key);

	//For grading only. This is bad practice to have, because
	//it exposes a private member variable.
	BPlusTreeNode* get_parent() { return parent; } 

	//We need this to let BPlusTree access private members
	friend class BPlusTree<T>;
private:
	bool contains(const T& key,std::size_t low,std::size_t high);
	std::vector<T> keys;
	std::vector<BPlusTreeNode*> children;
	BPlusTreeNode* parent;
};

template <class T>
bool BPlusTreeNode<T>::is_leaf(){
	for(unsigned int i=0; i<children.size(); i++)
	{
		if(children[i])
			return false;
	}
	return true;
}

template <class T>
bool BPlusTreeNode<T>::contains(const T& key){
	return contains(key,0,keys.size()-1);
}

//Private method takes advantage of the fact keys are sorted
template <class T>
bool BPlusTreeNode<T>::contains(const T& key,std::size_t low,std::size_t high){
	if(low>high){
		return false;
	}
	if(low==high){
		return key == keys[low];
	}
	std::size_t mid = (low+high)/2;
	if(key<=keys[mid]){
		return contains(key,0,mid);
	}
	else{
		return contains(key,mid+1,high);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//Your BPlusTree implementation goes below this point.
///////////////////////////////////////////////////////////////////////////////////////


template <class T>
class BPlusTree
{
	public:
		BPlusTree(int children)
		{
			root = NULL;
			b = children;
		}

		BPlusTree(const BPlusTree<T>& old)
		{
			b = old.b;
			root = copy_tree(old.root); //this-> ?
		}

		BPlusTreeNode<T>* find(T val)
		{
			return my_find(val, root);
		}

		void insert(T val);

		BPlusTree<T>& operator=(const BPlusTree<T>& old) 
		{
			b = old.b;
		    if (&old != this) 
		    {
		      this->destroy_tree(root);
		      root = this->copy_tree(old.root);
		    }
		    return *this;
		}

		void print_sideways(std::ofstream& outfile)
		{
			my_print_sideways(outfile, root, 0);
		}

		void print_BFS(std::ofstream& outfile)
		{
			my_print_BFS(outfile, root);
		}

		~BPlusTree() { this->destroy_tree(root);  root = NULL; }

	private:
		//member variables
		BPlusTreeNode<T>* root;
		int b;

		//my helper functions
		BPlusTreeNode<T>* my_find(T val, BPlusTreeNode<T>* root);
		void leaf_split(BPlusTreeNode<T>*& root);
		void node_split(BPlusTreeNode<T>*& root); // come back? by reference??
		BPlusTreeNode<T>* copy_tree(BPlusTreeNode<T>* root);
		void my_print_sideways(std::ofstream& outfile, BPlusTreeNode<T>* root, int depth);
		void my_print_BFS(std::ofstream& outfile, BPlusTreeNode<T>* root);

		//operator
		bool operator==(const BPlusTree<T>& old) const { return (old.root == this->root); }

		//destructor
		void destroy_tree(BPlusTreeNode<T>* root)
		{
			if(!root)
				return;

			for(unsigned int i = 0; i < root->children.size(); i++)
				destroy_tree(root->children[i]);

			delete root;
		}
};

template <class T>
BPlusTreeNode<T>* BPlusTree<T>::my_find(T value, BPlusTreeNode<T>* root)
{
	if(root == NULL)
		return NULL;

	if(root->is_leaf()) //end of the line
	{
		return root;
	}
	
	unsigned int i = 0; //need this later
	for(i = 0; i < root->keys.size(); i++) //look through children
	{
		if(value < root->keys[i]) //if value less than root value
		{
			return my_find(value, root->children[i]);
		}
	}
	return my_find(value, root->children[i]); //value greater than last child value, so calling last child
}

template <class T>
void BPlusTree<T>::insert(T value)
{
	if(root == NULL)
	{
		root = new BPlusTreeNode<T>();
		root->keys.push_back(value);
		return;
	}

	BPlusTreeNode<T>* spot = my_find(value, root);

	typename vector<T>::iterator key_it = spot->keys.begin(); //find where to put the value
	while(key_it != spot->keys.end() && *key_it < value)
		key_it++;
	
	if(key_it == spot->keys.end())
	{
		spot->keys.push_back(value);
	}
	else	
		spot->keys.insert(key_it, value);

	if(int(spot->keys.size()) < b)
		return;
	else
	{
		leaf_split(spot);
	}
}

template <class T>
void BPlusTree<T>::leaf_split(BPlusTreeNode<T>*& root)
{
	if(root->get_parent() == NULL)
	{
		BPlusTreeNode<T>* first = new BPlusTreeNode<T>;
		BPlusTreeNode<T>* second = new BPlusTreeNode<T>;

		for(unsigned int i = 0; i < root->keys.size() / 2; i++)
			first->keys.push_back(root->keys[i]);

		for(unsigned int i = root->keys.size() / 2; i < root->keys.size(); i++)
			second->keys.push_back(root->keys[i]);

		root->keys.clear();
		root->keys.push_back(second->keys[0]);

		first->parent = root;
		second->parent = root;

		root->children.push_back(first);
		root->children.push_back(second);

		return;
	}

	BPlusTreeNode<T>* first = new BPlusTreeNode<T>;
	BPlusTreeNode<T>* second = new BPlusTreeNode<T>;

	for(unsigned int i = 0; i < root->keys.size() / 2; i++)
	{
		first->keys.push_back(root->keys[i]);
	}

	for(unsigned int i = root->keys.size() / 2; i < root->keys.size(); i++)
	{
		second->keys.push_back(root->keys[i]);
	}

	BPlusTreeNode<T>* root_parent = root->get_parent();
	first->parent = root_parent;
	second->parent = root_parent;

	root_parent->keys.push_back(second->keys[0]);

	root_parent->children.pop_back();
	delete root;

	root_parent->children.push_back(first);
	root_parent->children.push_back(second);

	if(int(root_parent->keys.size()) > b-1)
		node_split(root_parent);
}

template <class T>
void BPlusTree<T>::node_split(BPlusTreeNode<T>*& root)
{
	BPlusTreeNode<T>* new_root = new BPlusTreeNode<T>;
	for(unsigned int i = 0; i < root->keys.size(); i++)
		new_root->keys.push_back(root->keys[i]);

	leaf_split(new_root);

	new_root->children[1]->keys.erase(new_root->children[1]->keys.begin());

	int j = 0;
	for(unsigned int i = 0; i < new_root->children.size(); i++)
	{
		while(root->children[j]->keys[root->children[j]->keys.size()-1] <= new_root->children[i]->keys[0])
		{
			new_root->children[i]->children.push_back(root->children[j]);
			j++;
		}
	}
	new_root->children[new_root->children.size()-1]->children.push_back(root->children[root->children.size()-1]);

	this->destroy_tree(root);
	root = new_root;
}

template <class T>
BPlusTreeNode<T>* BPlusTree<T>::copy_tree(BPlusTreeNode<T>* root)
{
	if(root == NULL)
		return NULL;

	BPlusTreeNode<T>* temp = new BPlusTreeNode<T>;

	for(unsigned int i = 0; i < root->keys.size(); i++)
		temp->keys.push_back(root->keys[i]);

	for(unsigned int i = 0; i < root->children.size(); i++)
	{
		BPlusTreeNode<T>* child = copy_tree(root->children[i]);
		temp->children.push_back(child);
		child->parent = temp;
	}

	return temp;
}

template <class T>
void BPlusTree<T>::my_print_sideways(std::ofstream& outfile, BPlusTreeNode<T>* root, int depth)
{
	if(root == NULL)
	{
		outfile << "Tree is empty." << endl;
		return;
	}

	for(unsigned int i = 0; i < root->children.size()/2; i++)
		my_print_sideways(outfile, root->children[i], depth + 1);

	for(int i = 0; i < depth; i++)
		outfile << "    ";

	for(unsigned int i = 0; i < root->keys.size(); i++)
		outfile << root->keys[i] << ",";

	outfile << endl;

	for(unsigned int i = root->children.size()/2; i < root->children.size(); i++)
	{
		my_print_sideways(outfile, root->children[i], depth + 1);
	}
}

template <class T>
void BPlusTree<T>::my_print_BFS(std::ofstream& outfile, BPlusTreeNode<T>* root)
{
	if(root == NULL)
	{
		outfile << "Tree is empty." << endl;
		return;
	}

	for(unsigned int i = 0; i < root->keys.size(); i++)
		outfile << root->keys[i] << ", ";

	for(unsigned int i = 0; i < root->children.size(); i++)
		my_print_BFS(outfile, root->children[i]);
}

#endif
