#ifndef MULTITHREADEDLIST_INCLUDE_LIST_HPP_
#define MULTITHREADEDLIST_INCLUDE_LIST_HPP_

#include <atomic>
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <thread>
#include <vector>

template< typename T>
struct ListNode {
        T data;
        std::atomic< ListNode< T >* > next;

        // Default constructor
        ListNode(): data{}, next(nullptr) {}
        // Constructor by the element
        explicit ListNode(const T& value): data(value), next(nullptr) {}
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
                // For the case we try to get value of tail_
                Node* iterator_tail;

            public:
                // So we cannot call default constructor
                Iterator() = delete;
                explicit Iterator(Node* node, Node* tail):
                    cur(node), iterator_tail(tail) {}

                Iterator operator++() {
                    cur = cur->next;
                    return *this;
                }

                bool operator==(const Iterator& other) const {
                    return cur == other.cur;
                }
                bool operator!=(const Iterator& other) const {
                    return !(cur == other.cur);
                }

                T operator*() const {
                    if (cur != iterator_tail) {
                        return cur->data;
                    }
                }
        };

        Iterator begin() {
            return Iterator(head_->next, tail_);
        }
        Iterator end() {
            return Iterator(tail_, tail_);
        }
        Iterator cbegin() const {
            return Iterator(head_->next, tail_);
        }
        Iterator cend() {
            return Iterator(tail_, tail_);
        }

        // Default constructor
        explicit List(size_t n_threads = 1, size_t n_hazard_ptr = 1) :
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
                head_(nullptr), last_(nullptr), tail_(nullptr), size_(n),
                n_threads_(1), n_hazard_ptr_(1) {
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
            scan_();
            /*if (head_ != nullptr) {
                Node* prev = head_;
                for (Node* _next = head_->next; _next != nullptr;
                        _next = _next->next) {
                    delete(prev);
                    prev = _next;
                }
                delete(prev);
            }*/
        }

        // Output all elements
        void output() {
            Node* auxiliary = head_->next.load(std::memory_order_acquire);

            while (auxiliary->next != nullptr) {
                std::cout << auxiliary->data << std::endl;
                auxiliary = auxiliary->next;
            }
            std::cout << std::endl;
        }

        // Push element the the back of the list
        void push_back(const T& data) {
            Node* new_node = new Node(data);
            // Value checks whether the CAS is successfully done
            bool is_CAS_done = false;

            while (!is_CAS_done) {
                // Store the present tail
                new_node->next = last_->next.load(std::memory_order_acquire);

                is_CAS_done = last_->next.compare_exchange_weak(tail_,
            new_node, std::memory_order_relaxed);
            }
            // Move the last element
            last_ = new_node;

            // Increment the list's size
            ++size_;
        }

        // Push element to the top of the list
        void push_front(const T& data) {
            Node* new_node = new Node(data);
            // Value checks whether the CAS is successfully done
            bool is_CAS_done = false;

            std::stringstream s;
            s << "Adding element " << data << std::endl;
            std::cout << s.str();
            s.clear();
            while (!is_CAS_done) {
                // Store the present first element
                new_node->next = head_->next.load(std::memory_order_acquire);
                // Auxuliary variable has another proccess intervened
                Node* head = new_node->next.load(std::memory_order_acquire);

                is_CAS_done = head_->next.compare_exchange_weak(head,
            new_node, std::memory_order_relaxed);
            }

            // Add to empty list
            if (head_ == last_) {
                last_ = new_node;
            }

            ++size_;
        }

        void pop_front() {
            // List is empty
            if (head_->next == tail_) {
                std::cout << "List is empty. We cannot pop it!\n";
                return;
            }

            //  Remained only one element
            if (head_->next == last_) {
                std::cout << "Remains only one element\n";
                head_->next.compare_exchange_weak(last_, tail_,
                    std::memory_order_relaxed);
                --size_;
                return;
            }
            Node* tmp;
            bool done = false;
            while (!done) {
                tmp = head_->next.load(std::memory_order_relaxed);
                // Node* auxiliary = tmp->next.load(std::memory_order_relaxed);
                // size_t _data = tmp->data;
                done = head_->next.compare_exchange_weak(tmp, tmp->next,
                    std::memory_order_relaxed);
                // if (done) {
                    /*std::stringstream s;
                    s << "Deleting element " << tmp->data << std::endl;
                    std::cout << s.str();
                    s.clear();*/
                    // delete(tmp);
                    /*s << "Now first element is "
                        << head_->next.load(std::memory_order_relaxed)->data
                        << std::endl;
                    std::cout << s.str();*/
                // }
            }

            --size_;
            retire_(tmp);
        }

    private:
        // Fake node refers to the first element of the list
        Node* head_;
        // Last node with the element
        Node* last_;
        // Fake node to which refers last element of the list
        Node* tail_;
        // Size of the list
        std::atomic< size_t > size_;
        // Number of the concurrent threads P
        size_t n_threads_;
        // Size of the hazard pointers K
        size_t n_hazard_ptr_;
        // Vector hazard nodes for each proccess
        std::map<std::thread::id, std::vector< Node* >> hazard_ptrs_;
        // Vector nodes we want to delete
        // массив готовых к удалению данных
        std::map<std::thread::id, std::vector< Node* >> retired_nodes_;

        // Удаление данных
        // Помещает данные в map retired_nodes_
        void retire_(Node* wasted) {
            // Adding nodes to the vector of those which we want to delete
            std::thread::id id = std::this_thread::get_id();
            retired_nodes_[id].push_back(wasted);

            // batch size, R = 2*P*K
            // Если массив заполнен – вызываем основную функцию Scan
            if (retired_nodes_[id].size() == 2 * n_threads_ * n_hazard_ptr_) {
                //std::cout << "Time to delete nodes\n";
                scan_();
            }
        }

        // Основная функция
        // Удаляет все элементы retired_nodes_, которые не объявлены
        // как Hazard Pointer
        void scan_() {
            // Stage 1 – проходим по всем HP всех потоков
            // Собираем общий массив plist защищенных указателей
            std::vector< Node* > common_hazards;
            for (auto i : hazard_ptrs_) {
                // Adding hazard nodes of all threads
                for (auto curr : i.second) {
                    common_hazards.push_back(curr);
                }
            }

            // Stage 2 – сортировка hazard pointer'ов
            // Сортировка нужна для последующего бинарного поиска
            std::sort(common_hazards.begin(), common_hazards.end());

            // Stage 3 – удаление элементов, не объявленных как hazard
            std::thread::id id = std::this_thread::get_id();
            // Stage 4 – формирование нового массива отложенных элементов
            std::vector< Node* > updated_retired;
            for (auto rejected : retired_nodes_[id]) {
                if (std::binary_search(common_hazards.begin(),
                        common_hazards.end(), rejected)) {
                    updated_retired.push_back(rejected);
                } else {
                    delete rejected;
                }
            }
        }
};

#endif  // MULTITHREADEDLIST_INCLUDE_LIST_HPP_
