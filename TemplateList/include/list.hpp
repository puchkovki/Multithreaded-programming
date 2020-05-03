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
		ListNode< T >* next;

		// Default constructor
		ListNode(): data{}, next(nullptr)
		{}
		// Constructor by the element
		ListNode(const T& value): data(value), next(nullptr)
		{}

		/*ListNode operator*() const {
			return data;
		}*/
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
			return ++Iterator(tail_);
		};
		Iterator cbegin() const{
			return Iterator(head_);
		};
		Iterator cend() {
			return ++Iterator(tail_);
		};

		// Default constructor
		List(size_t n_threads = 1, size_t n_hazard_ptr = 1) :
				head_(nullptr), tail_(nullptr), size_(0),
				n_threads_(n_threads), n_hazard_ptr_(n_hazard_ptr) {
			// std::cout << "List counstructed;\n" << std::endl;
		}

		// Fill constructor
		List(size_t n, const T& val = T(), size_t n_threads = 1, size_t n_hazard_ptr = 1) :
				head_(nullptr), tail_(nullptr), size_(n), n_threads_(1),
				n_hazard_ptr_(1) {
			Node* prev = nullptr;
			for (size_t i = 0; i < n; ++i) {
				Node* node_ = new Node(val);
				if (prev != nullptr) {
					node_->next = prev;
				}
				prev = node;
			}
			Node* last = new Node;
			prev->next = last;
		}

		// Default destructor
		~List() {
			if(head_ != nullptr) {
				Node* prev = head_;
				for (Node* _next = head_; _next->next != nullptr; _next = _next->next) {
					if (prev != _next) {
						delete(prev);
					}
					prev = _next;
				}
			}
		};

		// Output all elements
		void output() {
			Node* auxiliary = head_.load(std::memory_order_acquire);

			while(auxiliary != nullptr) {
				std::cout << auxiliary->data << std::endl;
				auxiliary = auxiliary->next;
			}
			std::cout << std::endl;
		}

		// Push element the the list's back
		void push_back(const T& data) {
			Node* new_node = new Node(data);

			if(head_ == nullptr) {
				head_ = new_node;
				tail_ = new_node;
			} else {
				Node* now_tail = tail_.load(std::memory_order_acquire);
				do {
					now_tail->next = new_node;
				} while(!std::atomic_compare_exchange_weak_explicit(&tail_, &now_tail, new_node, std::memory_order_release, std::memory_order_relaxed));			
			}
		}

		void push_front(const T& data) {
			Node* new_node = new Node(data);

			if(head_ == nullptr) {
				head_ = new_node;
				tail_ = new_node;
			} else {
				Node* now_head = head_.load(std::memory_order_acquire);

				do {
					new_node->next = now_head;
				} while(!std::atomic_compare_exchange_weak_explicit(&head_, &now_head, new_node, std::memory_order_release, std::memory_order_relaxed));
			}
		}

		// TODO pop_front, pop_back;

	private:
		std::atomic< Node* > head_;
		std::atomic< Node* > tail_;
		
		// Size of the list
		size_t size_;
		// Number of the concurrent threads
		size_t n_threads_;
		// Size of the hazard pointers
		size_t n_hazard_ptr_;
};

#endif