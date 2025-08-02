#include <utility>
namespace trrt {

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

/// A smart pointer with value semantics.
/**

Analogous to std::unique_ptr but have deep
copy constructor & operator=. Doesn't take custom
deleter.

operator== also checks for equality deeply, i.e. *ptr_a == *ptr_b;

*/
template <typename T> class deep_ptr {
    T* ptr = nullptr;

    public:
    constexpr deep_ptr() = default;

    explicit deep_ptr(T* ptr_) : ptr{ ptr_ } {}

    deep_ptr(const deep_ptr& other) : ptr{ new T(*other.ptr) } {}
    deep_ptr(deep_ptr&& other) noexcept : ptr{ other.ptr } {
        other.ptr = nullptr;
    }

    deep_ptr& operator=(const deep_ptr& other) {
        if(this == &other)
            return *this;
        auto old = ptr;
        ptr = new T(other.ptr);
        delete old;
        return *this;
    }

    deep_ptr& operator=(deep_ptr&& other) noexcept {
        if(this == &other)
            return *this;
        delete ptr;
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }


    ~deep_ptr() { delete ptr; }

    T& operator*() const { return *ptr; }
    T* operator->() const noexcept { return ptr; }

    bool operator==(const deep_ptr<T>& other) const {
        if(other.ptr != nullptr && this->ptr != nullptr)
            return *ptr == *other.ptr;
        return other.ptr == nullptr && this->ptr == nullptr;
    }
};
// NOLINTEND(cppcoreguidelines-owning-memory)


template <typename T, typename... Args> deep_ptr<T> make_deep(Args&&... args) {
    return deep_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace trrt