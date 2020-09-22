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

        void set(T newVal);

        T get();

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
guardedVar<T> guardedVar<T>::operator=(guardedVar<T> other){
    set(other.get());
}

template <typename T>
guardedVar<T> guardedVar<T>::operator=(const T& other){
    set(other);
    return get();
}

template <typename T>
T guardedVar<T>::get(){
    std::shared_lock lock(mutex_var);
    return var;
}

template <typename T>
void guardedVar<T>::set(T newVal){
    std::scoped_lock<std::shared_mutex> lock(mutex_var);
    var = newVal;
};

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
