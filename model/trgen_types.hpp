#ifndef TRGEN_TYPES_HPP

#define TRGEN_TYPES_HPP
struct kernel_release {
    int kernel;
    int major;
    int minor;
    int patch;

    bool operator>=(const kernel_release& other) const {
        if(kernel == other.kernel && major == other.major && other.minor == minor && other.patch >= patch) return true;
        if(kernel == other.kernel && major == other.major && other.minor >= minor) return true;
        if(kernel == other.kernel && major >= other.major) return true;
        if(kernel > other.kernel) return true;
        
        return false;
    }

    bool operator<=(const kernel_release& other) const {
        if(kernel == other.kernel && major == other.major && other.minor == minor && other.patch <= patch) return true;
        if(kernel == other.kernel && major == other.major && other.minor <= minor) return true;
        if(kernel == other.kernel && major <= other.major) return true;
        if(kernel < other.kernel) return true;
        
        return false;
    }
    bool operator==(const kernel_release& other) const {
        return (other.kernel == kernel && other.major == major &&
                other.minor == minor && other.patch == patch);
    }

};

enum class exit_types {
    NEEDED_ROOT_PRIVILAGES = 1,

};
#endif //TRGEN_TYPES_HPP