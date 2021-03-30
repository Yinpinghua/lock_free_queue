#ifndef lock_free_queue_h__
#define lock_free_queue_h__

#include <atomic>
template <typename T>
class lock_free_queue
{
public:
    lock_free_queue()
        :head_(new buffer_node)
        ,tail_(head_.load(std::memory_order_relaxed))
    {
    }
    ~lock_free_queue()
    {
        T output;
        while (this->dequeue(output));
        buffer_node* front = head_.load(std::memory_order_relaxed);
        delete front;
        front = nullptr;
    }

    /**
     * @brief Put a item into the queue.
     *
     * @param input
     * @note This method can be called in multiple threads.
     */
    void enqueue(T&& input)
    {
        buffer_node* node{ new buffer_node(std::move(input)) };
        buffer_node* prevhead{ head_.exchange(node, std::memory_order_acq_rel) };
        prevhead->next_.store(node, std::memory_order_release);
    }
    void enqueue(const T& input)
    {
        buffer_node* node{ new buffer_node(input) };
        buffer_node* prevhead{ head_.exchange(node, std::memory_order_acq_rel) };
        prevhead->next_.store(node, std::memory_order_release);
    }

    /**
     * @brief Get a item from the queue.
     *
     * @param output
     * @return false if the queue is empty.
     * @note This method must be called in a single thread.
     */
    bool dequeue(T& output)
    {
        buffer_node* tail = tail_.load(std::memory_order_relaxed);
        buffer_node* next = tail->next_.load(std::memory_order_acquire);

        if (next == nullptr) {
            return false;
        }

        output = std::move(*(next->data_ptr_));
        delete next->data_ptr_;
        next->data_ptr_ = nullptr;
        tail_.store(next, std::memory_order_release);
        delete tail;
        tail = nullptr;
        return true;
    }

    bool empty()
    {
        buffer_node* tail = tail_.load(std::memory_order_relaxed);
        buffer_node* next = tail->next_.load(std::memory_order_acquire);
        return next == nullptr;
    }

private:
    struct buffer_node
    {
        buffer_node() = default;
        buffer_node(const T& data) : data_ptr_(new T(data))
        {
        }
        buffer_node(T&& data) : data_ptr_(new T(std::move(data)))
        {
        }
        T* data_ptr_;
        std::atomic<buffer_node*> next_{ nullptr };
    };

    std::atomic<buffer_node*> head_;
    std::atomic<buffer_node*> tail_;
};
#endif // lock_free_queue_h__
