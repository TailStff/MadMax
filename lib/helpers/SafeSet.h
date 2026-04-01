// Inline function to safely set a value to a pointer if it's not null
template<typename T>
inline void SafeSet(T* ptr, const T& value)
{
    if (ptr) *ptr = value;
}