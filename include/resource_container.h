#ifndef RESOURCE_CONTAINER_HPP
#define RESOURCE_CONTAINER_HPP

#include <filesystem>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <typeindex>
#include <vector>

#include <thread_pool.h>

namespace rt
{
    class IOException : public std::system_error
    {
    public:
        enum Type
        {
            NotFound,
            WrongType,
            FileCorrupt,
        };

        class Category : public std::error_category
        {
        public:
            static Category instance;

            virtual const char *
            name() const noexcept override { return "IOException"; }

            virtual std::string message(int code) const override;
        };

        explicit IOException(Type code, const std::string &str)
            : std::system_error(code, Category::instance, str) {}
        explicit IOException(Type code)
            : std::system_error(code, Category::instance) {}

        // virtual const char *what() const noexcept override;
    };

    template <typename T>
    class ResourceRef;
    class ResourceContainer;

    class Resource
    {
    private:
    public:
        Resource() = default;
        virtual ~Resource() = default;
    };

    class ResourceLoader
    {
    private:
    public:
        ResourceLoader() = default;
        virtual ~ResourceLoader() = default;

        virtual void load(ResourceRef<void> resource, const std::filesystem::path &path) const = 0;
    };

    struct _SharedResourceState : public std::enable_shared_from_this<_SharedResourceState>
    {
        enum class State
        {
            NotLoaded,
            Loading,
            Loaded,
            Failed,
        } state;

        std::filesystem::path     path;
        std::type_index           type;
        std::unique_ptr<Resource> ptr;
        std::exception_ptr        exception;
        ResourceContainer        *container;
        std::mutex                mutex;
        std::set<std::type_index> failedTypes;

    private:
        _SharedResourceState(const std::filesystem::path &path, ResourceContainer *container)
            : state(State::NotLoaded), path(path), type(typeid(void)), ptr(nullptr), exception(nullptr), container(container) {}

        friend class ResourceContainer;
    };

    template <typename T>
    class ResourceRef
    {
        static_assert(std::is_convertible<T, Resource>::value, "ResourceRef can only reference objects that inherit from rt::Reference");

    private:
        std::shared_ptr<_SharedResourceState> m_ptr;

    private:
        void requestLoad();

    public:
        ResourceRef() = default;

        ResourceRef(const std::shared_ptr<_SharedResourceState> &ptr)
            : m_ptr(ptr) { requestLoad(); }
        ResourceRef(void *&ptr)
            : ResourceRef(((_SharedResourceState *)ptr)->shared_from_this()) {}

        inline      operator bool() const { return m_ptr && m_ptr->state == _SharedResourceState::State::Loaded && m_ptr->ptr && m_ptr->type == typeid(T); }
        inline bool resourceAttached() const { return m_ptr.operator bool(); }
        inline T   *operator->() { return (T *)m_ptr->ptr.get(); };
        inline T   *operator->() const { return (T *)m_ptr->ptr.get(); }
        inline T   &operator*() { return *(m_ptr->ptr); };
        inline T   &operator*() const { return *(m_ptr->ptr); }

        inline const bool                   hasException() const { return m_ptr->state == _SharedResourceState::State::Failed; }
        inline const std::filesystem::path &getPath() const { return m_ptr->path; }
        inline const std::exception_ptr     getException() const { return m_ptr->exception; }
        inline const std::type_index        getType() const { return m_ptr->type; }
    };

    template <>
    class ResourceRef<Resource>
    {
    private:
        std::shared_ptr<_SharedResourceState> m_ptr;

    public:
        ResourceRef(const std::shared_ptr<_SharedResourceState> &ptr)
            : m_ptr(ptr) {}
        ResourceRef(void *&ptr)
            : ResourceRef(((_SharedResourceState *)ptr)->shared_from_this()) {}

        // template<class T>
        inline           operator Resource *() const { return (*this) ? m_ptr->ptr.get() : nullptr; }
        inline           operator bool() const { return m_ptr && m_ptr->state == _SharedResourceState::State::Loaded && m_ptr->ptr; }
        inline bool      resourceAttached() const { return m_ptr.operator bool(); }
        inline Resource *operator->() { return (Resource *)m_ptr->ptr.get(); };
        inline Resource *operator->() const { return (Resource *)m_ptr->ptr.get(); }
        inline Resource &operator*() { return *(m_ptr->ptr); };
        inline Resource &operator*() const { return *(m_ptr->ptr); }
        inline bool      operator<(const ResourceRef<Resource> &other) const { return m_ptr < other.m_ptr; }
        inline bool      operator>(const Resource *other) const { return m_ptr->ptr.get() > other; }
        inline bool      operator==(const ResourceRef<Resource> &other) const { return m_ptr == other.m_ptr; }
        inline bool      operator!=(const ResourceRef<Resource> &other) const { return m_ptr != other.m_ptr; }

        inline const bool                   hasException() const { return (m_ptr->state == _SharedResourceState::State::Failed); }
        inline const std::filesystem::path &getPath() const { return m_ptr->path; }
        inline const std::exception_ptr     getException() const { return m_ptr->exception; }
        inline const std::type_index        getType() const { return m_ptr->type; }
        inline const void                  *getPtr() const { return m_ptr.get(); }

        template <class T>
        inline operator ResourceRef<T>() { return ResourceRef<T>(m_ptr); }
    };

    template <>
    class ResourceRef<void>
    {
    private:
        std::weak_ptr<_SharedResourceState> m_ptr;

    public:
        ResourceRef(const std::weak_ptr<_SharedResourceState> &ptr)
            : m_ptr(ptr) {}

        template <class T>
        operator ResourceRef<T>();

        template <class T, std::enable_if_t<std::is_base_of<Resource, T>::value, bool> = true>
        void submit(std::unique_ptr<T> &&resource);

        friend class ResourceContainer;
    };

    template <class T>
    std::ostream &operator<<(std::ostream &stream, const ResourceRef<T> &resource);

    class ResourceContainer
    {
    private:
        using resource_collection_type = std::vector<std::shared_ptr<_SharedResourceState>>;
        resource_collection_type m_resources;

        std::map<std::type_index, std::unique_ptr<ResourceLoader>> m_loaders;

        ThreadPool<std::packaged_task<void()>> *m_threadPool;

        std::filesystem::path m_appDir;

        std::vector<std::future<void>> m_loadingFutures;

    private:
        template <class T>
        class _Iterator
        {
        private:
            T m_it;

        public:
            _Iterator(const T &it)
                : m_it(it) {}

        public:
            inline _Iterator &operator++()
            {
                ++m_it;
                return *this;
            }
            inline _Iterator operator++(int)
            {
                auto t = *this;
                ++m_it;
                return t;
            }
            inline _Iterator &operator--()
            {
                --m_it;
                return *this;
            }
            inline _Iterator operator--(int)
            {
                auto t = *this;
                --m_it;
                return t;
            }
            inline Resource             *operator->() { return (*m_it)->ptr.get(); }
            inline ResourceRef<Resource> operator*() { return ResourceRef<Resource>(*m_it); }
            inline bool                  operator==(const _Iterator &other) const { return m_it == other.m_it; }
            inline bool                  operator!=(const _Iterator &other) const { return m_it != other.m_it; }
        };

    public:
        using Iterator = _Iterator<resource_collection_type::iterator>;
        using ReverseIterator = _Iterator<resource_collection_type::reverse_iterator>;

    public:
        ResourceContainer(ThreadPool<std::packaged_task<void()>> *threadPool, const std::filesystem::path &m_appDir)
            : m_threadPool(threadPool), m_appDir(m_appDir) {}
        ~ResourceContainer()
        {
            assert(std::all_of(m_resources.begin(), m_resources.end(), [](std::shared_ptr<_SharedResourceState> &r)
                               { return r.use_count() == 1; }));
        }

        ResourceRef<void> operator+=(const std::filesystem::path &path);

        // This function takes ownership of loader
        template <class Type, class T,
                  std::enable_if_t<std::is_base_of<ResourceLoader, T>::value, bool> = true,
                  std::enable_if_t<std::is_base_of<Resource, Type>::value, bool> = true>
        inline T *add(T *loader);

        inline Iterator        begin() { return _Iterator(m_resources.begin()); }
        inline Iterator        end() { return _Iterator(m_resources.end()); }
        inline ReverseIterator rbegin() { return _Iterator(m_resources.rbegin()); }
        inline ReverseIterator rend() { return _Iterator(m_resources.rend()); }

        void waitForFinishLoading();

    private:
        void requestLoad(_SharedResourceState &state, std::type_index type);
        void loadTask(ResourceLoader *loader, std::filesystem::path path, const ResourceRef<void> &resource);

        template <class T>
        friend class ResourceRef;
    };

    // ---------- Implementation ----------

    template <class T>
    void ResourceRef<T>::requestLoad()
    {
        m_ptr->container->requestLoad(*m_ptr, typeid(T));
    }

    template <class T>
    ResourceRef<void>::operator ResourceRef<T>()
    {
        std::shared_ptr ptr = m_ptr.lock();
        if (!ptr)
            throw std::bad_weak_ptr();
        return std::move(ResourceRef<T>(ptr));
    }

    template <class T, std::enable_if_t<std::is_base_of<Resource, T>::value, bool>>
    void ResourceRef<void>::submit(std::unique_ptr<T> &&resource)
    {
        std::shared_ptr ptr = m_ptr.lock();
        if (!ptr)
            return;
        assert(ptr->type == typeid(T) && "Resource loader returned resource of wrong type");
        ptr->state = _SharedResourceState::State::Loaded;
        ptr->ptr = std::move(resource);
    }

    template <class T>
    std::ostream &operator<<(std::ostream &stream, const ResourceRef<T> &resource)
    {
        return stream << (resource ? resource.getPath().filename() : "null");
    }

    template <class Type, class T,
              std::enable_if_t<std::is_base_of<ResourceLoader, T>::value, bool>,
              std::enable_if_t<std::is_base_of<Resource, Type>::value, bool>>
    inline T *ResourceContainer::add(T *loader)
    {
        auto result = m_loaders.emplace(typeid(Type), loader);
        if (!result.second)
            delete loader;
        return result.second ? loader : (T *)nullptr;
    }

} // namespace rt

#endif // RESOURCE_CONTAINER_HPP
