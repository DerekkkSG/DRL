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
#include <stdexcept>

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

/* CONCEPTS */
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

    ternary_quantum() : m_state{quantum_state::true_v}, value(value_type{}) {}

    ternary_quantum(const value_type& new_value) 
    : m_state{quantum_state::true_v}, value{new_value} {}

    ternary_quantum(const value_type& new_value, const quantum_state& new_state) 
    : m_state{new_state}, value{new_value} {}

    ternary_quantum(const quantum_t& new_ternary) : m_state{new_ternary.get_state()}, value{new_ternary.get_value()} {};
    ternary_quantum(quantum_t&& new_ternary) noexcept {
        if(this != &new_ternary){
            m_state = std::move(new_ternary.get_state());
            value = std::move(new_ternary.get_value());

            new_ternary.change_state(quantum_state::true_v);
            new_ternary.set_value(value_type{});
        }
    };

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
        m_state = new_value.get_state();
        return *this;
    }

    void operator=(quantum_t&& new_value) noexcept {
        if(this != &new_value){
            m_state = std::move(new_value.get_state());
            value = std::move(new_value.value);

            new_value.change_state(quantum_state::true_v);
            new_value.set_value(value_type{});
        }
    }

    [[nodiscard]] auto operator<=>(const quantum_t& other_quantum) const {
        return (value <=> other_quantum.get_value());
    }

    [[nodiscard]] bool operator==(const quantum_t& new_value) const {
        return (value == new_value.get_value()) && (m_state == new_value.get_state());
    }

    [[nodiscard]] bool operator!=(const quantum_t& new_value) const {
        return (value != new_value.get_value()) && (m_state != new_value.get_state());
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

/* One shared static variable; one data independant of all of them stores by they're ID */
template<typename Own, typename Linked>
class linked_quantum {
public:
    using value_type  = Own;
    using linked_type = Linked;
    using quantum_t   = linked_quantum<value_type, linked_type>;
    
private:
    std::shared_ptr<linked_type> shared_value;
    value_type owned_value;

public:

    linked_quantum() : shared_value{nullptr} {}
    explicit linked_quantum(value_type own_v) : shared_value{nullptr}, owned_value{own_v} {}
    linked_quantum(value_type own_v, linked_type linked_v){
        owned_value = own_v;
        shared_value = std::make_shared<linked_type>(linked_v);        
    }

    linked_quantum(quantum_t& other_quantum){
        owned_value = other_quantum.owned_value;
        shared_value = other_quantum.shared_value;
    }

    linked_quantum(quantum_t&& other_quantum) noexcept
      : owned_value{std::move(other_quantum.owned_value)},
        shared_value{std::move(other_quantum.shared_value)} 
    { 
        other_quantum.shared_value = nullptr; 
        other_quantum.owned_value = value_type{};
    }

    linked_quantum(const quantum_t& other_quantum) = delete;

    /* MAIN METHODS */
    void link(quantum_t& other_quantum) {
        shared_value = other_quantum.shared_value;
        std::cout << "linked!" << std::endl;
    }

    void reset(){
        shared_value = nullptr;
        std::cout << "no linked value stored!" << std::endl;
    }

    [[nodiscard]] constexpr linked_type get_linked_v() const { 
        if(shared_value != nullptr) return *shared_value; 
        return linked_type{};
    }

    [[nodiscard]] constexpr value_type get_own_v() const noexcept { return owned_value; }

    void set_own_v(value_type new_value) noexcept { owned_value = new_value; }

    void set_linked_v(linked_type new_linked_v) { 
        if(shared_value != nullptr) *shared_value = new_linked_v; 
    }

    [[nodiscard]] constexpr bool is_linked() const noexcept { return shared_value != nullptr; }

    /* OPERATORS */
    quantum_t& operator=(quantum_t& other_quantum){
        owned_value = other_quantum.owned_value;
        shared_value = other_quantum.shared_value;
        return *this;
    }

    void operator=(quantum_t&& other_quantum) noexcept {
        if(this != &other_quantum)
        {
            owned_value = other_quantum.owned_value;
            shared_value = other_quantum.shared_value;

            other_quantum.shared_value = nullptr; 
            other_quantum.owned_value = value_type{};
        }
    }

    [[nodiscard]] auto operator<=>(const quantum_t& other_quantum) const {
        return (owned_value <=> other_quantum.owned_value);
    }

    [[nodiscard]] bool operator==(const quantum_t& other_quantum) const {
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

namespace quantum {
class strict_uncertain_trait {
public:
    template<typename V, typename U>
    static void Fix(const std::vector<V>& _values, const std::vector<U>& _pounded){
        if(_values.size() != _pounded.size())
        throw std::range_error("Incoherent vector size between storage and pounded probability inside drk::uncertain_quantum");
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
}

/* Contains an indefinite number of variables; the one you use is choosen at random */
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

    void fix_vectors(){
        while(m_storage.size() != m_poundered_probability.size()){
            if(m_storage.size() < m_poundered_probability.size()){
                m_poundered_probability.pop_back();
            } else m_poundered_probability.push_back(0);
        }
    }

public:

    uncertain_quantum() : m_storage(1, value_type{}), m_poundered_probability(1, 0), choosen_rand(kind_of_dist::uniform) {}

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

    /* MAIN METHODS */
    void change_values(const std::vector<value_type>& new_storage) { m_storage = new_storage; }

    void make_uniform() noexcept { choosen_rand = kind_of_dist::uniform; }

    void make_pounded(const std::vector<unsigned int>& new_poundered){
        choosen_rand = kind_of_dist::pounded;
        m_poundered_probability = new_poundered;
        Strictness::Fix(m_storage, m_poundered_probability);
    }

    [[nodiscard]] value_type observe(){
        if(m_storage.size() < 1) return value_type{};
        if(choosen_rand == kind_of_dist::uniform) {
            std::cout << "uniform" << std::endl;
            return m_storage[impl::_get_random_uniform_int(m_storage.size()-1)];
        }
        std::cout << "pounded" << std::endl;
        return m_storage[impl::_get_random_poundered_int(m_poundered_probability)];
    }

    /* ITERATORS */
    [[nodiscard]] iterator begin() { return m_storage.begin(); }

    [[nodiscard]] iterator end() { return m_storage.end(); }

    [[nodiscard]] const_iterator begin() const { return m_storage.begin(); }

    [[nodiscard]] const_iterator end() const { return m_storage.end(); }

    /* OPERATORS */
    [[nodiscard]] auto operator<=>(const quantum_t& other_quantum) const {
        return (m_storage <=> other_quantum.m_storage);
    }

    [[nodiscard]] bool operator==(const quantum_t& other_quantum) const {
        return (m_storage == other_quantum.m_storage);
    }
    
};

template<typename T>
using strict_uncertain = uncertain_quantum<T, quantum::strict_uncertain_trait>;

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

/// @brief Namespace used to store all specialized methods
namespace quantum{

    /// @brief Method used to get a random quantum_state.
    /// @return A random quantum_state (_::false_v, _::maybe_v, _::true_v).
    [[nodiscard]] inline quantum_state get_random_state(){
        return static_cast<quantum_state>(impl::_get_random_uniform_int(2));
    }

}

/* drk::temporal; mantains the last state used by the object */

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