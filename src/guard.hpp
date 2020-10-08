#pragma once

#include <mutex>
#include <shared_mutex>


template <typename T>
class guardedVar{
    private:
        T var;
        std::shared_mutex mutex_var;
    public:
        guardedVar(T val);
        guardedVar() = delete;

        /**
         * @brief Use this function to safely set the value of this variable (same as using =).
         * 
         * @param newVal The new value this variable should have.
         */
        void set(T newVal);

        /**
         * @brief Use this to safely retrieve the value insode of the container.
         * 
         * @return T The value in the container.
         */
        T get();

        void lock();
        void lock_shared();

        void unlock();
        void unlock_shared();

        /**
         * @brief Get the value without any locking.
         * @warning This should only be used if you manually manage the locking otherwise you will get nasty issues.
         * 
         * @return T The value stored in the container.
         */
        T get_unsafe();

        guardedVar<T> operator=(const T& other);
        guardedVar<T> operator=(guardedVar<T> other);

        void operator++(int);
        void operator++();

        void operator--(int);
        void operator--();

        T operator!();
        T operator-();

        T operator+(guardedVar<T>);
        T operator+(T);

        T operator-(guardedVar<T>);
        T operator-(T);

        T operator*(guardedVar<T>);
        T operator*(T);

        T operator/(guardedVar<T>);
        T operator/(T);

        T operator+=(guardedVar<T>);
        T operator+=(T);

        T operator-=(guardedVar<T>);
        T operator-=(T);

        T operator*=(guardedVar<T>);
        T operator*=(T);

        T operator/=(guardedVar<T>);
        T operator/=(T);

        bool operator<(T);
        bool operator>(T);

        bool operator<=(T);
        bool operator>=(T);

        bool operator==(T);
        bool operator!=(T);

        bool operator<(guardedVar<T>);
        bool operator>(guardedVar<T>);

        bool operator<=(guardedVar<T>);
        bool operator>=(guardedVar<T>);

        bool operator==(guardedVar<T>);
        bool operator!=(guardedVar<T>);

};


template <typename T>
guardedVar<T>::guardedVar(T val){
    set(val);
}

template <typename T>
void guardedVar<T>::lock(){
    mutex_var.lock();
}

template <typename T>
void guardedVar<T>::lock_shared(){
    mutex_var.lock_shared();
}

template <typename T>
void guardedVar<T>::unlock(){
    mutex_var.unlock();
}

template <typename T>
void guardedVar<T>::unlock_shared(){
    mutex_var.unlock_shared();
}

template <typename T>
T guardedVar<T>::get(){
    std::shared_lock lock(mutex_var);
    return var;
}

template <typename T>
T guardedVar<T>::get_unsafe(){
    return var;
}

template <typename T>
void guardedVar<T>::set(T newVal){
    std::scoped_lock<std::shared_mutex> lock(mutex_var);
    var = newVal;
};

template <typename T>
guardedVar<T> guardedVar<T>::operator=(guardedVar<T> other){
    set(other.get());
}

template <typename T>
guardedVar<T> guardedVar<T>::operator=(const T& other){
    set(other);
    return get();
}

template <typename T>
void guardedVar<T>::operator++(int){
    std::scoped_lock<std::shared_mutex> lock(mutex_var);
    var++;
}

template <typename T>
void guardedVar<T>::operator++(){
    std::scoped_lock<std::shared_mutex> lock(mutex_var);
    var++;
}

template <typename T>
void guardedVar<T>::operator--(int){
    std::scoped_lock<std::shared_mutex> lock(mutex_var);
    var++;
}

template <typename T>
void guardedVar<T>::operator--(){
    std::scoped_lock<std::shared_mutex> lock(mutex_var);
    var++;
}

template <typename T>
T guardedVar<T>::operator!(){
    return !get();
}

template <typename T>
T guardedVar<T>::operator-(){
    return -get();
}


template <typename T>
T guardedVar<T>::operator+(guardedVar<T> other){
    return this->get() + other.get();
}

template <typename T>
T guardedVar<T>::operator+(T other){
    return this->get() + other;
}

template <typename T>
T guardedVar<T>::operator-(guardedVar<T> other){
    return this->get() - other.get();
}

template <typename T>
T guardedVar<T>::operator-(T other){
    return this->get() - other;
}

template <typename T>
T guardedVar<T>::operator*(guardedVar<T> other){
    return this->get() * other.get();
}

template <typename T>
T guardedVar<T>::operator*(T other){
    return this->get() * other;
}

template <typename T>
T guardedVar<T>::operator/(guardedVar<T> other){
    return this->get() / other.get();
}

template <typename T>
T guardedVar<T>::operator/(T other){
    return this->get() / other;
}


template <typename T>
T guardedVar<T>::operator+=(guardedVar<T> other){
    auto tmp = this->get() + other.get();
    this->set(tmp);
    return this->get();
}

template <typename T>
T guardedVar<T>::operator+=(T other){
    auto tmp = this->get() + other;
    this->set(tmp);
    return this->get();
}

template <typename T>
T guardedVar<T>::operator-=(guardedVar<T> other){
    auto tmp = this->get() - other.get();
    this->set(tmp);
    return this->get();
}

template <typename T>
T guardedVar<T>::operator-=(T other){
    auto tmp = this->get() - other;
    this->set(tmp);
    return this->get();
}

template <typename T>
T guardedVar<T>::operator*=(guardedVar<T> other){
    auto tmp = this->get() * other.get();
    this->set(tmp);
    return this->get();
}

template <typename T>
T guardedVar<T>::operator*=(T other){
    auto tmp = this->get() * other;
    this->set(tmp);
    return this->get();
}

template <typename T>
T guardedVar<T>::operator/=(guardedVar<T> other){
    auto tmp = this->get() / other.get();
    this->set(tmp);
    return this->get();
}

template <typename T>
T guardedVar<T>::operator/=(T other){
    auto tmp = this->get() / other;
    this->set(tmp);
    return this->get();
}


template <typename T>
bool guardedVar<T>::operator<(T other){
    return this->get() < other;
}

template <typename T>
bool guardedVar<T>::operator>(T other){
    return this->get() > other;
}

template <typename T>
bool guardedVar<T>::operator<=(T other){
    return this->get() <= other;
}

template <typename T>
bool guardedVar<T>::operator>=(T other){
    return this->get() >= other;
}

template <typename T>
bool guardedVar<T>::operator==(T other){
    return this->get() == other;
}

template <typename T>
bool guardedVar<T>::operator!=(T other){
    return this->get() != other;
}


template <typename T>
bool guardedVar<T>::operator<(guardedVar<T> other){
    return this->get() < other.get();
}

template <typename T>
bool guardedVar<T>::operator>(guardedVar<T> other){
    return this->get() > other.get();
}

template <typename T>
bool guardedVar<T>::operator<=(guardedVar<T> other){
    return this->get() <= other.get();
}

template <typename T>
bool guardedVar<T>::operator>=(guardedVar<T> other){
    return this->get() >= other.get();
}

template <typename T>
bool guardedVar<T>::operator==(guardedVar<T> other){
    return this->get() == other.get();
}

template <typename T>
bool guardedVar<T>::operator!=(guardedVar<T> other){
    return this->get() != other.get();
}
