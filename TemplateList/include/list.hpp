#ifndef LIST_HPP
#define LIST_HPP

#include <atomic>

#ifndef OSTREAM_
#define OSTREAM_

#include <iostream>

#endif


template< typename T>
struct ListNode {
		T data;
		std::atomic< ListNode< T >* > next;

		// Default constructor
		ListNode(): data{}, next(nullptr)
		{}
		// Constructor by the element
		ListNode(const T& value): data(value), next(nullptr)
		{}
	};

template< typename T>
class List {
	public:
		ListNode< T >* node;

		// Default typedefs for the list container
		typedef T value_type;
		typedef ListNode< value_type > Node;

        typedef value_type&         reference;
        typedef const value_type&   const_reference;

        typedef value_type*         pointer;
        typedef const value_type*   const_pointer;

        typedef size_t              size_type;
        typedef ptrdiff_t           difference_type;

		class Iterator {
			private:
				Node* cur;

			public:
				Iterator(Node* node): cur(node)
				{}

				Iterator operator++() {
					cur = cur->next;
					return *this;
				}

				bool operator==(Iterator other) const {
					return cur == other.cur;
				}
				bool operator!=(Iterator other) const {
					return !(cur == other.cur);
				}

				Node operator*() const {
					return *cur;
				}
		};
		//typedef Iterator<value_type> iterator;
        //typedef __list_const_iterator<value_type>       const_iterator;
        //typedef std::reverse_iterator<iterator> reverse_iterator;
        //typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;


		Iterator begin() {
			return Iterator(head_);
		};
		Iterator end() {
			return Iterator(tail_);
		};
		Iterator cbegin() const{
			return Iterator(head_);
		};
		Iterator cend() {
			return Iterator(tail_);
		};

		// Default constructor
		List(size_t n_threads = 1, size_t n_hazard_ptr = 1) :
				head_(nullptr),  tail_(nullptr), size_(0),
				n_threads_(n_threads), n_hazard_ptr_(n_hazard_ptr) {
			head_ = new Node(T());
			last_ = head_;
			tail_ = new Node(T());
			last_->next = tail_;
		}

		// Fill constructor
		List(size_t n, const T& val = T(), size_t n_threads = 1, 
		size_t n_hazard_ptr = 1) :
				head_(nullptr), last_(nullptr), tail_(nullptr), size_(n), n_threads_(1), n_hazard_ptr_(1) {			
			head_ = new Node(val);
			tail_ = new Node(val);
			Node* prev = nullptr;
			for (size_t i = 0; i < n; ++i) {
				Node* node_ = new Node(val);
				if (prev != nullptr) {
					prev->next = node_;
				} else {
					head_->next.store(node_, std::memory_order_release);
				}
				prev = node_;
			}
			last_ = prev;
			prev->next = tail_;
		}

		// Default destructor
		~List() {
			if(head_ != nullptr) {
				Node* prev = head_;
				for (Node* _next = head_; _next->next != nullptr;
				_next = _next->next) {
					if (prev != _next) {
						delete(prev);
					}
					prev = _next;
				}
			}
		};

		// Output all elements
		void output() {
			Node* auxiliary = head_->next.load(std::memory_order_acquire);

			while(auxiliary != nullptr) {
				std::cout << auxiliary->data << std::endl;
				auxiliary = auxiliary->next;
			}
			std::cout << std::endl;
		}

		// Push element the the list's back
		void push_back(const T& data) {
			Node* new_node = new Node(data);
			Node* tail = new Node();

			do {
				tail = last_->next;
				new_node->next = tail;
			} while(!last_->next.compare_exchange_weak(tail,
			new_node, std::memory_order_relaxed));

			size_++;
		}

		void push_front(const T& data) {
			Node* new_node = new Node(data);
			Node* head = new Node();

			do {
				head = head_->next.load(std::memory_order_acquire);
				new_node->next = head;
			} while(!head_->next.compare_exchange_weak(head,
			new_node, std::memory_order_relaxed));

			size_++;
		}

	private:
		//Node* first_;
		Node* head_;
		// Last node with the element
		Node* last_;
		Node* tail_;
		// Fake node for the iterators
		
		// Size of the list
		std::atomic< size_t > size_;
		// Number of the concurrent threads
		size_t n_threads_;
		// Size of the hazard pointers
		size_t n_hazard_ptr_;
};

#endif