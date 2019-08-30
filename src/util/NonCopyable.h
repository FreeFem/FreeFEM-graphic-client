#ifndef NON_COPYABLE_H
#define NON_COPYABLE_H

namespace FEM {

/**
 * @brief Inheriting classes can't be copied.
 */
struct NonCopyable {
    NonCopyable( ) = default;
    ~NonCopyable( ) = default;

    /**
     * @brief Deleted copy constructor.
     */
    NonCopyable(const NonCopyable&) = delete;

    /**
     * @brief Deleted copy operator.
     */
    NonCopyable& operator=(const NonCopyable&) = delete;
};

}    // namespace FEM
#endif    // NON_COPYABLE_H