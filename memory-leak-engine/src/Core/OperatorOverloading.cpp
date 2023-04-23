#ifdef TRACY_ENABLE

void *operator new(std::size_t count) {
    auto ptr = malloc(count);
    TracySecureAlloc (ptr, count);
    return ptr;
}

void operator delete(void *ptr) noexcept {
    TracySecureFree (ptr);
    free(ptr);
}

#endif