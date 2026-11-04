#ifndef DRL_QUANTUM_H
#define DRL_QUANTUM_H

#include <iostream>
#include <type_traits>
#include <random>
#include <utility>
#include <vector>
#include <memory>
#include <concepts>
#include <compare>
#include <exception>

/*
------------------------------------------------------------------------------------------------------------------------
This library is for personal or public use, made by Derek himself.
The distribution of this library is unsupported unless it comes from the main repository.
This library includes all related methods and datatypes necessary to accomplish the desires.
If you get to use the library, please don't trust your life on it; it's not the best implemented.
------------------------------------------------------------------------------------------------------------------------
*/

namespace drk { //Start of the library

/// @brief A set of discrete states to express three distinct logical values
enum class quantum_state : int {
    false_v = 0,
    maybe_v,
    true_v
};

[[nodiscard]] inline int operator+(const quantum_state& lhs_state, const quantum_state& rhs_state){
    return static_cast<int>(lhs_state) + static_cast<int>(rhs_state);
}

[[nodiscard]] inline int operator-(const quantum_state& lhs_state, const quantum_state& rhs_state){
    return static_cast<int>(lhs_state) - static_cast<int>(rhs_state);
}

[[nodiscard]] inline quantum_state operator*(const quantum_state& lhs_state, const quantum_state& rhs_state){
    return static_cast<quantum_state>(static_cast<int>(lhs_state) * static_cast<int>(rhs_state));
}

[[nodiscard]] inline quantum_state operator/(const quantum_state& lhs_state, const quantum_state& rhs_state){
    return static_cast<quantum_state>(static_cast<int>(lhs_state) * static_cast<int>(rhs_state));
}

namespace impl{

    inline std::mt19937& _get_device_impl(){
        static std::random_device _rd;
        static std::mt19937 get(_rd());
        return get;
    }

    inline size_t _get_random_uniform_int(const size_t& _top_value){
        std::uniform_int_distribution<unsigned int> _get_rand(0, static_cast<unsigned int>(_top_value));
        return static_cast<size_t>(_get_rand(_get_device_impl()));
    }

    inline size_t _get_random_poundered_int(const std::vector<unsigned int>& _poundered_values){
        if(_poundered_values.empty()) return 0;
        std::discrete_distribution<unsigned int> _get_poundered(_poundered_values.begin(), _poundered_values.end());
        return static_cast<size_t>(_get_poundered(_get_device_impl()));
    }

}

// FORWARD DECLARATION
template<typename T>  requires std::copyable<T>
class ternary_quantum;

template<typename Own, typename Linked>
class linked_quantum;

template<typename T, class Strictness>
class uncertain_quantum;

// CONCEPTS
template<typename T>
concept is_uncertain = requires(T obj){
    { obj.observe() };
    { obj.make_uniform() };
    { obj.begin() };
    { obj.end() };
};

template<typename T>
concept is_linked = requires(T obj){
    { obj.is_linked() };
    { obj.reset() };
    { obj.get_linked_v() };
    { obj.get_own_v() };
};

template<typename T>
concept is_ternary = requires(T obj){
    { obj.get_value() };
    { obj.get_state() };
    { obj.is_defined() };
};

template<typename T, typename Own, typename Quantum>
concept is_linked_operable = std::is_same_v<T, Own> || std::is_same_v<T, Quantum>;

/*
PRELIMINARY CONCEPT; Still in testing stage
template<typename T>
concept is_observable = std::is_arithmetic_v<T> || std::is_pointer_v<T> || std::is_enum_v<T>;
*/

// DRK::QUANTUM NAMESPACE
namespace quantum {
// EXEPTIONS
class [[nodiscard]] incoherent_size : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override {
        return "Incoherent vector size between storage and pounded probability inside drk::uncertain_quantum";
    }
};

class strict_uncertain_trait {
public:
    template<typename V, typename U>
    static void Fix(const std::vector<V>& _values, const std::vector<U>& _pounded){
        if(_values.size() != _pounded.size())
        throw incoherent_size();
    }
};

class free_uncertain_trait {
public:
    template<typename V, typename U>
    static void Fix(const std::vector<V>& _values, std::vector<U>& _pounded){
        while(_values.size() != _pounded.size()){
            if(_values.size() < _pounded.size()){
                _pounded.pop_back();
            } else _pounded.push_back(0);
        }
    }
};

/// @brief Method used to get a random quantum_state.
/// @return A random quantum_state (_::false_v, _::maybe_v, _::true_v).
[[nodiscard]] inline quantum_state get_random_state(){
    return static_cast<quantum_state>(impl::_get_random_uniform_int(2));
}
} // END OF DRK::QUANTUM NAMESPACE

/// @brief A datatype that can store two distinct variables: a logical quantum_state (_::true_v, _::maybe_v, _::false_v) and a T value.
/// @brief You can change each one when needed; default state is _::true_v.
/// @tparam T Used to define any kind of datatype its value can own.
template<typename T> requires std::copyable<T>
class ternary_quantum {
public:
    using value_type = T;
    using quantum_t  = ternary_quantum<value_type>;
private:
    quantum_state m_state;
public:
    value_type value;

    ternary_quantum() : m_state{quantum_state::true_v}, value{value_type{}} {}

    ternary_quantum(const value_type& new_value) 
    : m_state{quantum_state::true_v}, value{new_value} {}

    ternary_quantum(const value_type& new_value, const quantum_state& new_state) 
    : m_state{new_state}, value{new_value} {}

    ternary_quantum(const quantum_t& new_ternary) 
    : m_state{new_ternary.get_state()}, value{new_ternary.get_value()} {}

    ternary_quantum(quantum_t&& new_ternary) noexcept {
        m_state = std::move(new_ternary.get_state());
        value = std::move(new_ternary.get_value());

        new_ternary.change_state(quantum_state::true_v);
        new_ternary.set_value(value_type{});
    }

    ~ternary_quantum() = default;

    /* MAIN METHODS */
    void change_state(quantum_state new_state) noexcept { m_state = new_state; }

    void set_value(value_type new_value) { value = new_value; }

    [[nodiscard]] constexpr quantum_state get_state() const noexcept { return m_state; }

    [[nodiscard]] constexpr value_type get_value() const noexcept requires std::is_arithmetic_v<value_type> { return value; }

    [[nodiscard]] constexpr bool is_defined() const noexcept { return m_state != quantum_state::maybe_v; }

    /* OPERATORS */
    quantum_t& operator=(const quantum_t& new_value){
        value = new_value.value;
        m_state = new_value.m_state;
        return *this;
    }

    quantum_t& operator=(quantum_t&& new_value) noexcept {
        if(this != &new_value){
            m_state = std::move(new_value.m_state);
            value = std::move(new_value.value);

            new_value.change_state(quantum_state::true_v);
            new_value.set_value(value_type{});
        }
        return *this;
    }

    [[nodiscard]] auto operator<=>(const quantum_t& other_quantum) const {
        return (value <=> other_quantum.value);
    }

    [[nodiscard]] bool operator==(const quantum_t& new_value) const {
        return (value == new_value.value) && (m_state == new_value.m_state);
    }

    [[nodiscard]] bool operator!=(const quantum_t& new_value) const {
        return (value != new_value.value) && (m_state != new_value.m_state);
    }

    [[nodiscard]] value_type operator+(const quantum_t& new_value) const requires std::is_arithmetic_v<value_type> {
        return value + new_value.value;
    }

    quantum_t& operator+=(const quantum_t& new_value) requires std::is_arithmetic_v<value_type> {
            value = value + new_value.value;
            return *this;
    }

    [[nodiscard]] value_type operator-(const quantum_t& new_value) const requires std::is_arithmetic_v<value_type> {
        return value - new_value.value;
    }

    quantum_t& operator-=(const quantum_t& new_value) requires std::is_arithmetic_v<value_type> {
        value = value - new_value.value;
        return *this;
    }

    [[nodiscard]] value_type operator*(const quantum_t& new_value) const requires std::is_arithmetic_v<value_type> {
        return value * new_value.value;
    }

    quantum_t& operator*=(const quantum_t& new_value) requires std::is_arithmetic_v<value_type> {
        value = value * new_value.value;
        return *this;
    }

    [[nodiscard]] value_type operator/(const quantum_t& new_value) const requires std::is_arithmetic_v<value_type> {
        return value / new_value.value;
    }

    quantum_t& operator/=(const quantum_t& new_value) requires std::is_arithmetic_v<value_type> {
        value = value / new_value.value;
        return *this;
    }
};

// PLEASE don't ask me about how I made this class. I've had enough if it already to also explain how it works.
// (PD: I didn't think I'd need all this recursion to solve what at first seemed like such a simple problem).
/// @brief Has an own value and a shared (linked) value with other linked_quantums
template<typename Own, typename Linked>
class linked_quantum {
public:
    using value_type     = Own;
    using linked_type    = Linked;
    using quantum_t      = linked_quantum<value_type, linked_type>;
    using quantum_ptr    = quantum_t*;
    using quantum_vector = std::vector<quantum_ptr>;
private:
    linked_type shared_value;
    value_type owned_value;
    quantum_ptr shared_obj;
    // This vector is used to relate the main node and the other ones linked to it (this)
    quantum_vector shared_ref;

    // Method used to search for the original linked_quantum node storing the shared value
    quantum_ptr get_other_shared(quantum_t& other_quantum, quantum_ptr this_quantum){
        // If the shared_ptr of other is this, returns nullptr. If not, we search in the next node.
        // Then, if the next node is nullptr, we've reached the end and, as a consequence, we return other's memory address (ptr)
        if (other_quantum.shared_obj == this_quantum) return nullptr;
        else if(other_quantum.shared_obj != nullptr) return get_other_shared(*(other_quantum.shared_obj), this_quantum);
        else return &other_quantum;
    }

public:

    linked_quantum() : shared_obj{nullptr} {}

    explicit linked_quantum(const value_type& own_v) 
    : owned_value{own_v}, shared_obj{nullptr} {}

    linked_quantum(const value_type& own_v, const linked_type& linked_v) 
    : shared_value{linked_v}, owned_value{own_v}, shared_obj{nullptr} {}

    linked_quantum(quantum_t& other_quantum)
    : shared_value{other_quantum.shared_value}, owned_value{other_quantum.owned_value},
      shared_obj{other_quantum.shared_obj} {}

    linked_quantum(quantum_t&& other_quantum) noexcept
    : owned_value{std::move(other_quantum.owned_value)}, shared_value{std::move(other_quantum.shared_value)},
      shared_obj{std::move(other_quantum.shared_obj)}, shared_ref{std::move(other_quantum.shared_ref)}
    { 
        // Deleting this from the references stored by the other_quantum.shared_ref to avoid this->this
        for(size_t i=0; i < shared_ref.size(); i++){
            if(shared_ref[i] == this){
                shared_ref.erase(shared_ref.begin() + static_cast<long long>(i));
                break;
            }
        }

        // Changing all the references from other_quantum to *this
        for(quantum_ptr& e : shared_ref) { e->shared_obj = this; }

        // If other_quantum is linked to another quantum, the reference to other_quantum inside the original node
        // is changed to this
        if(shared_obj != nullptr){
            for(quantum_ptr& e : shared_obj->shared_ref){
                if(e == &other_quantum) { e = this; break; }
            }
        }

        other_quantum.shared_value = linked_type{}; 
        other_quantum.owned_value = value_type{};
        other_quantum.shared_obj = nullptr;
        other_quantum.shared_ref.clear();
    }

    //The deletion of this constructor was made to prevent unwanted behaviour with the class thanks
    //to the const keyword and unchangeable values
    linked_quantum(const quantum_t& other_quantum) = delete;

    ~linked_quantum() {
        // Cicle made to avoid dangling pointers inside each linked_quantum linked to *this
        reset();
        for(quantum_ptr& e : shared_ref) { e->shared_obj = nullptr; }
    };

    // MAIN METHODS
    void link(quantum_t& other_quantum) {
        // Implementing this was like the most insufferable thing ever. Don't even TRY to ask me how I pulled this off
        if(this == &other_quantum) return;
        
        quantum_ptr checking_for_this = get_other_shared(other_quantum, this);
        // Checking for ciclic relationships between linked_quantums. This kind of relations can't exist.
        if(checking_for_this == this || checking_for_this == nullptr){ return; }

        // Deleting posible previous relationships with reset() and asigning the new ones
        reset();
        shared_obj = &other_quantum;
        shared_value = linked_type{};
        other_quantum.shared_ref.emplace_back(this);
    }

    void reset(){
        if(shared_obj == nullptr) return;

        // Deleting this from the references stored by the other_quantum.shared_ref to avoid
        // referencing a non-linked quantum
        for(size_t i=0; i < shared_obj->shared_ref.size(); i++){
            if(shared_obj->shared_ref[i] == this){
                shared_obj->shared_ref.erase(shared_obj->shared_ref.begin() + static_cast<long long>(i));
                break;
            }
        }

        shared_obj = nullptr;
    }

    [[nodiscard]] constexpr linked_type get_linked_v() const { 
        if(shared_obj != nullptr) return shared_obj->get_linked_v(); 
        else return shared_value;
    }

    [[nodiscard]] constexpr value_type get_own_v() const noexcept { return owned_value; }

    void set_own_v(value_type new_value) { owned_value = new_value; }

    [[nodiscard]] quantum_ptr get_linked_ptr() const { return shared_obj; }

    void set_linked_v(linked_type new_linked_v) { 
        if(shared_obj != nullptr){
            shared_obj->set_linked_v(new_linked_v);
        } else shared_value = new_linked_v;
    }

    [[nodiscard]] constexpr bool is_linked() const noexcept { return shared_obj != nullptr; }

    // OPERATORS
    quantum_t& operator=(quantum_t& other_quantum){
        owned_value = other_quantum.owned_value;
        shared_value = other_quantum.shared_value;
        shared_obj = other_quantum.shared_obj;
        return *this;
    }

    quantum_t& operator=(quantum_t&& other_quantum) noexcept {
        if(this != &other_quantum)
        {
            owned_value = std::move(other_quantum.owned_value);
            shared_value = std::move(other_quantum.shared_value);
            shared_obj = std::move(other_quantum.shared_obj);
            shared_ref = std::move(other_quantum.shared_ref);

            // Deleting this from the references stored by the other_quantum.shared_ref to avoid this->this
            for(size_t i=0; i < shared_ref.size(); i++){
                if(shared_ref[i] == this){
                    shared_ref.erase(shared_ref.begin() + static_cast<long long>(i));
                    break;
                }
            }

            // Changing all the references from other_quantum to *this
            for(quantum_ptr& e : shared_ref) { e->shared_obj = this; }

            // If other_quantum is linked to another quantum, the reference to other_quantum inside the 
            // original node is changed to this
            if(shared_obj != nullptr){
                for(quantum_ptr& e : shared_obj->shared_ref){
                    if(e == &other_quantum) { e = this; break; }
                }
            }

            other_quantum.shared_value = linked_type{}; 
            other_quantum.owned_value = value_type{};
            other_quantum.shared_obj = nullptr;
            other_quantum.shared_ref.clear();
        }
        return *this;
    }

    quantum_t& operator=(const value_type& new_value){
        owned_value = new_value;
        return *this;
    }

    [[nodiscard]] auto operator<=>(const quantum_t& other_quantum) const {
        return (owned_value <=> other_quantum.owned_value);
    }

    [[nodiscard]] bool operator==(const quantum_t& other_quantum) const {
        return (owned_value == other_quantum.owned_value);
    }

    [[nodiscard]] bool operator!=(const quantum_t& other_quantum) const {
        return (owned_value == other_quantum.owned_value);
    }
    
    [[nodiscard]] value_type operator+(const quantum_t& rhs_quantum) const requires std::is_arithmetic_v<value_type> {
        return owned_value + rhs_quantum.owned_value;
    }

    quantum_t& operator+=(const quantum_t& new_value) requires std::is_arithmetic_v<value_type> {
        owned_value = owned_value + new_value.owned_value;
        return *this;
    }

    [[nodiscard]] value_type operator-(const quantum_t& rhs_quantum) const requires std::is_arithmetic_v<value_type> {
        return owned_value - rhs_quantum.owned_value;
    }

    quantum_t& operator-=(const quantum_t& new_value) requires std::is_arithmetic_v<value_type> {
        owned_value = owned_value - new_value.owned_value;
        return *this;
    }

    [[nodiscard]] value_type operator*(const quantum_t& rhs_quantum) const requires std::is_arithmetic_v<value_type> {
        return owned_value * rhs_quantum.owned_value;
    }

    quantum_t& operator*=(const quantum_t& new_value) requires std::is_arithmetic_v<value_type> {
        owned_value = owned_value * new_value.owned_value;
        return *this;
    }

    [[nodiscard]] value_type operator/(const quantum_t& rhs_quantum) const requires std::is_arithmetic_v<value_type> {
        return owned_value / rhs_quantum.owned_value;
    }

    quantum_t& operator/=(const quantum_t& new_value) requires std::is_arithmetic_v<value_type> {
        owned_value = owned_value / new_value.owned_value;
        return *this;
    }
};

/// @brief Contains an indefinite number of variables.
/// @brief The one you use is choosen at random with the observe() method.
template<typename T, class Strictness = quantum::free_uncertain_trait>
class uncertain_quantum {
public:
    using value_type     = T;
    using iterator       = typename std::vector<value_type>::iterator;
    using const_iterator = typename std::vector<value_type>::const_iterator;
    using quantum_t      = uncertain_quantum<value_type, Strictness>;
private:
    enum class kind_of_dist : int {
        uniform = 0,
        pounded
    };

    std::vector<value_type> m_storage;
    std::vector<unsigned int> m_poundered_probability;
    kind_of_dist choosen_rand;
    
public:

    uncertain_quantum() 
    : m_storage(1, value_type{}), m_poundered_probability(1, 0), choosen_rand(kind_of_dist::uniform) {}

    explicit uncertain_quantum(const std::vector<value_type>& _storage) 
    : m_storage(std::move(_storage)), m_poundered_probability(1, 0), choosen_rand(kind_of_dist::uniform) {}

    uncertain_quantum(const std::vector<value_type>& _storage, const std::vector<unsigned int>& _poundered)
    : m_storage(std::move(_storage)), m_poundered_probability(std::move(_poundered)), choosen_rand(kind_of_dist::pounded) {
        Strictness::Fix(m_storage, m_poundered_probability);
    }

    uncertain_quantum(const quantum_t& other){
        m_storage = other.m_storage;
        m_poundered_probability = other.m_poundered_probability;
        choosen_rand = other.choosen_rand;
    }

    uncertain_quantum(quantum_t&& other) :
        m_storage{std::move(other.m_storage)},
        m_poundered_probability{std::move(other.m_poundered_probability)},
        choosen_rand{std::move(other.choosen_rand)}
    {
        other.m_storage = std::vector<value_type>(1, value_type{});
        other.m_poundered_probability = std::vector<unsigned int>(1, 0);
        other.choosen_rand = kind_of_dist::uniform;
    }

    ~uncertain_quantum() = default;

    // MAIN METHODS 
    void change_values(const quantum_t& new_storage) { 
        m_storage = new_storage.m_storage;
        m_poundered_probability = new_storage.m_poundered_probability;
        Strictness::Fix(m_storage, m_poundered_probability);
    }

    void make_uniform() noexcept { choosen_rand = kind_of_dist::uniform; }

    void make_pounded(const std::vector<unsigned int>& new_poundered){
        choosen_rand = kind_of_dist::pounded;
        m_poundered_probability = new_poundered;
        Strictness::Fix(m_storage, m_poundered_probability);
    }

    [[nodiscard]] value_type observe(){
        if(m_storage.size() < 1) return value_type{};
        if(choosen_rand == kind_of_dist::uniform) {
            return m_storage[impl::_get_random_uniform_int(m_storage.size()-1)];
        }
        return m_storage[impl::_get_random_poundered_int(m_poundered_probability)];
    }

    // ITERATORS
    [[nodiscard]] iterator begin() { return m_storage.begin(); }

    [[nodiscard]] iterator end() { return m_storage.end(); }

    [[nodiscard]] const_iterator begin() const { return m_storage.begin(); }

    [[nodiscard]] const_iterator end() const { return m_storage.end(); }

};

// Template Specialization for uncertain_quantum
template<typename T>
using strict_uncertain = uncertain_quantum<T, quantum::strict_uncertain_trait>;

// Specialized functions for all quantums
template<typename T>
auto auto_observe(T quantum) requires is_uncertain<T> {
    return quantum.observe();
}

template<typename T>
[[nodiscard]] ternary_quantum<T> make_ternary(
    T _value = T{},
    quantum_state _state = quantum_state::true_v)
{
    return ternary_quantum<T>(_value, _state);
}

template<typename Own, typename Linked>
[[nodiscard]] linked_quantum<Own, Linked> make_linked(
    Own _own_value = Own{},
    Linked _shared_value = Linked{})
{
    return linked_quantum<Own, Linked>(_own_value, _shared_value);
}

template<typename T>
[[nodiscard]] uncertain_quantum<T> make_uncertain(
    const std::vector<T>& _storage = std::vector<T>(1, T{}),
    const std::vector<unsigned int>& _poundered = std::vector<unsigned int>(1, 0))
{
    return uncertain_quantum<T>(_storage, _poundered);
}

// ostream operators for each quantum
inline std::ostream& operator<<(std::ostream& ostream, drk::quantum_state state){
    switch(state){
        case drk::quantum_state::false_v: ostream << static_cast<int>(drk::quantum_state::false_v); break;
        case drk::quantum_state::maybe_v: ostream << static_cast<int>(drk::quantum_state::maybe_v); break;
        case drk::quantum_state::true_v: ostream << static_cast<int>(drk::quantum_state::true_v); break;
        default: ostream << static_cast<int>(drk::quantum_state::false_v);
    }
    return ostream;
}

template<typename Own, typename Linked>
std::ostream& operator<<(std::ostream& ostream, drk::linked_quantum<Own, Linked> quantum){
    ostream << "{" << quantum.get_own_v() << ", " << quantum.get_linked_v() << "}";
    return ostream;
}

template<typename T>
std::ostream& operator<<(std::ostream& ostream, drk::ternary_quantum<T> quantum){
    ostream << "{" << quantum.get_value() << ", " << quantum.get_state() << "}";
    return ostream;
}

} //End of the library

#endif