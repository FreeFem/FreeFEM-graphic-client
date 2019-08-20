#ifndef NON_COPYABLE_H
#define NON_COPYABLE_H

struct NonCopyable {
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

#endif // NON_COPYABLE_H