#ifndef MULTITHREADEDLIST_INCLUDE_LIST_HPP_
#define MULTITHREADEDLIST_INCLUDE_LIST_HPP_

#include <unistd.h>
#include <atomic>
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <thread>
#include <vector>
#include <utility>
#include <mutex>

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
                    return T();
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
                n_threads_(1), n_hazard_ptr_(1) {
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
            if (head_ != nullptr) {
                Node* prev = head_;
                for (Node* _next = head_->next; _next != nullptr;
                        _next = _next->next) {
                    retire_(prev);
                    prev = _next;
                }
                retire_(prev);
            }
            // scan_();
        }

        // Output all elements
        void output() {
            Node* auxiliary = head_->next.load(std::memory_order_acquire);

            if (auxiliary == tail_) {
                std::cout << "No elements" << std::endl;
            }
            while (auxiliary->next != nullptr) {
                std::cout << auxiliary->data << std::endl;
                auxiliary = auxiliary->next;
            }
            std::cout << std::endl;
        }

        void swapYield() {
            Node* first = head_->next.load(std::memory_order_acquire);
            // Adding node to the HP
            addToHp(first);

            /* std::stringstream s;
            s << "Swap " << first->data << " and "
                << first->next.load(std::memory_order_relaxed) << std::endl;
            std::cout << s.str();
            s.clear();*/

            Node* tmp = first;
            first = first->next;
            first->next = tmp;
            // std::swap(first, first->next);
            __asm volatile ("pause" ::: "memory");
            deleteFromHp(first);
            return;
        }

        void swapSleep() {
            plugThread();

            Node* first = head_->next.load(std::memory_order_acquire);
            Node* second = first->next.load(std::memory_order_acquire);
            // Adding node to the HP
            addToHp(first);
            addToHp(second);

            std::stringstream s;
            s << "Swap " << first->data << " and "
                << second->data
                << std::endl;
            std::cout << s.str();
            s.clear();

            Node* tmp = first;
            first = second;
            second = tmp;
            sleep(2);
            deleteFromHp(second);
            deleteFromHp(first);
            return;
        }

        // Push element the the back of the list
        void push_back(const T& data) {
            plugThread();
            Node* new_node = new Node(data);
            // Value checks whether the CAS is successfully done
            bool is_CAS_done = false;

            std::stringstream s;
            s << "Push " << data << " to the back" << std::endl;
            std::cout << s.str();
            s.clear();
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
            plugThread();
            Node* new_node = new Node(data);
            // Value checks whether the CAS is successfully done
            bool is_CAS_done = false;

            std::stringstream s;
            s << "Push " << data << " at the front" << std::endl;
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
            plugThread();
            // List is empty
            if (head_->next == tail_) {
                // std::cout << "List is empty. We cannot pop it!\n";
                return;
            }

            // List is not empty
            // Node* tmp;
            // size_t data;
            bool is_CAS_done = false;
            while (!is_CAS_done) {
                Node* tmp = head_->next.load(std::memory_order_acquire);
                // Node* aux = tmp->next.load(std::memory_order_relaxed);
                is_CAS_done = head_->next.compare_exchange_weak(tmp, tmp->next,
                    std::memory_order_relaxed);
                if (is_CAS_done) {
                    retire_(tmp);
                }
            }

            --size_;
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

        std::mutex threadMutex;

        // Удаление данных
        // Помещает данные в map retired_nodes_
        void retire_(Node* wasted) {
            if (wasted == nullptr) {
                // std::cout << "Trying to delete nullptr" << std::endl;
                return;
            }
            plugThread();
            // Adding nodes to the vector of those which we want to delete
            std::thread::id id = std::this_thread::get_id();
            retired_nodes_[id].push_back(wasted);

            // batch size, R = 2*P*K
            // Если массив заполнен – вызываем основную функцию Scan
            if (retired_nodes_[id].size() >= 2 * n_threads_ * n_hazard_ptr_) {
                // std::cout << "Time to delete nodes\n";
                /* std::stringstream s;
                s << "Pop " << wasted->data << std::endl;
                std::cout << s.str();
                s.clear(); */
                scan_();
            }
        }

        // Основная функция
        // Удаляет все элементы retired_nodes_, которые не объявлены
        // как Hazard Pointer
        void scan_() {
            // plugThread();
            // Stage 1 – проходим по всем HP всех потоков
            // Собираем общий массив plist защищенных указателей
            std::vector< Node* > common_hazards;
            for (auto HP : hazard_ptrs_) {
                // Adding hazard nodes of all threads
                for (auto nodes : HP.second) {
                    if (nodes != nullptr) {
                        common_hazards.push_back(nodes);
                    }
                }
            }

            // Stage 2 – сортировка hazard pointer'ов
            // Сортировка нужна для последующего бинарного поиска
            std::sort(common_hazards.begin(), common_hazards.end());

            // Stage 3 – удаление элементов, не объявленных как hazard
            std::thread::id id = std::this_thread::get_id();
            // Stage 4 – формирование нового массива отложенных элементов
            std::vector< Node* > updated_retired;
            auto it = hazard_ptrs_.find(id);
            if (it != hazard_ptrs_.end()) {
                for (auto rejected : it->second) {
                    if (std::binary_search(common_hazards.begin(),
                            common_hazards.end(), rejected)) {
                        updated_retired.push_back(rejected);
                    } else {
                        delete rejected;
                    }
                }
            }
            retired_nodes_[id] = updated_retired;
        }

        void addToHp(Node* dangerous) {
            if (dangerous == nullptr) {
                // std::cout << "Trying to add nullptr to HP\n";
                return;
            }

            std::thread::id id = std::this_thread::get_id();
            if (hazard_ptrs_[id].size() < n_hazard_ptr_) {
                hazard_ptrs_[id].push_back(dangerous);
            } else {
                // std::cout << "Hazard pointer stack is too big\n";
            }
            return;
        }

        void deleteFromHp(Node* dangerous) {
            if (dangerous == nullptr) {
                // std::cout << "Trying to delete nullptr from HP\n";
                return;
            }

            std::thread::id id = std::this_thread::get_id();
            // Test would it work with plugThread() method
            /* auto it = hazard_ptrs_.find(id);
            if (it != hazard_ptrs_.end()) {
                for (auto i : it->second) {
                    if (i == dangerous) {
                        hazard_ptrs_[id].erase(
                            std::remove(hazard_ptrs_[id].begin(),
                            hazard_ptrs_[id].end(), dangerous),
                            hazard_ptrs_[id].end());
                    }
                }
                // hazard_ptrs_[id].push_back(dangerous);
            }*/
            hazard_ptrs_[id].pop_back();
            return;
        }

        // Savely adding our thread to the hazard_ptrs_ and retired_nodes_ maps
        void plugThread() {
            std::thread::id id = std::this_thread::get_id();

            // Save addition
            std::lock_guard<std::mutex> guard(threadMutex);
            // Adding if necessary to the hazard_ptrs_ map
            if ((hazard_ptrs_.find(id) == hazard_ptrs_.end()) ||
                    (retired_nodes_.find(id) == retired_nodes_.end())) {
                ++n_threads_;
                // std::cout << "#" << id << " is pluged" << std::endl;
            }
            if (hazard_ptrs_.find(id) == hazard_ptrs_.end()) {
                hazard_ptrs_.insert(
                std::pair<std::thread::id, std::vector< Node* >>
                (id, std::vector< Node* >()));
            }
            // Adding if necessary to the retired_nodes_ map
            if (retired_nodes_.find(id) == retired_nodes_.end()) {
                retired_nodes_.insert(
                std::pair<std::thread::id, std::vector< Node* >>
                (id, std::vector< Node* >()));
            }
            return;
        }
};

#endif  // MULTITHREADEDLIST_INCLUDE_LIST_HPP_
