#ifndef RESOURCE_CONTAINER_HPP
#define RESOURCE_CONTAINER_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include <memory>
#include <typeindex>
#include <future>
#include <mutex>

#include <thread_pool.h>

namespace rt
{
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

        std::filesystem::path path;
        std::type_index type;
        std::unique_ptr<Resource> ptr;
        std::exception_ptr exception;
        ResourceContainer *container;
        std::mutex mutex;
        std::set<std::type_index> failedTypes;

    private:
        _SharedResourceState(const std::filesystem::path &path, ResourceContainer *container)
            : state(State::NotLoaded), path(path), type(typeid(void)), ptr(nullptr), exception(nullptr), container(container) {}

        friend class ResourceContainer;
    };

    template <typename T>
    class ResourceRef
    {
    private:
        std::shared_ptr<_SharedResourceState> m_ptr;

    private:
        void requestLoad();

    public:
        ResourceRef(const std::shared_ptr<_SharedResourceState> &ptr)
            : m_ptr(ptr) { requestLoad(); }

        inline operator bool() const { return m_ptr->state == _SharedResourceState::State::Loaded && m_ptr->ptr; }
        inline T *operator->() { return (T *)m_ptr->ptr.get(); };
        inline T *operator->() const { return (T *)m_ptr->ptr.get(); }
        inline T &operator*() { return *(m_ptr->ptr); };
        inline T &operator*() const { return *(m_ptr->ptr); }

        inline const bool &hasException() const { return m_ptr->state == _SharedResourceState::State::Failed; }
        inline const std::filesystem::path &getPath() const { return m_ptr->path; }
        inline const std::exception_ptr getException() const { return m_ptr->exception; }
        inline const std::type_index getType() const { return m_ptr->type; }
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

    class ResourceContainer
    {
    private:
        using resource_collection_type = std::vector<std::shared_ptr<_SharedResourceState>>;
        resource_collection_type m_resources;

        std::map<std::type_index, std::unique_ptr<ResourceLoader>> m_loaders;

        ThreadPool<std::packaged_task<void()>> *m_threadPool;

        std::filesystem::path m_appDir;

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
            inline Resource *operator->() { return (*m_it)->ptr.get(); }
            inline ResourceRef<Resource> operator*() { return ResourceRef<Resource>(*m_it); }
            inline bool operator==(const _Iterator &other) const { return m_it == other.m_it; }
            inline bool operator!=(const _Iterator &other) const { return m_it != other.m_it; }
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

        inline ResourceRef<void> operator+=(const std::filesystem::path &path);

        // This function takes ownership of loader
        template <class Type, class T,
                  std::enable_if_t<std::is_base_of<ResourceLoader, T>::value, bool> = true,
                  std::enable_if_t<std::is_base_of<Resource, Type>::value, bool> = true>
        inline T *add(T *loader);

        inline Iterator begin() { return _Iterator(m_resources.begin()); }
        inline Iterator end() { return _Iterator(m_resources.end()); }
        inline ReverseIterator rbegin() { return _Iterator(m_resources.rbegin()); }
        inline ReverseIterator rend() { return _Iterator(m_resources.rend()); }

    private:
        inline void requestLoad(_SharedResourceState &state, std::type_index type);
        inline void loadTask(ResourceLoader *loader, std::filesystem::path path, const ResourceRef<void> &resource);

        template <class T>
        friend class ResourceRef;
    };

    // ---------- Implementation ----------

    template <class T>
    void ResourceRef<T>::requestLoad()
    {
        if (m_ptr->state == _SharedResourceState::State::NotLoaded)
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

    inline void ResourceContainer::loadTask(ResourceLoader *loader, std::filesystem::path path, const ResourceRef<void> &resource)
    {
        try
        {
            if (!std::filesystem::exists(path))
            {
                path = m_appDir / path;
                if (!std::filesystem::exists(path))
                {
                    throw std::ios::failure("File not found");
                }
            }
            loader->load(resource, path);
        }
        catch (...)
        {
            auto r = resource.m_ptr.lock();
            if (!r)
                return;
            r->exception = std::current_exception();
            r->failedTypes.insert(r->type);
            r->state = _SharedResourceState::State::Failed;
        }
    }

    inline void ResourceContainer::requestLoad(_SharedResourceState &state, std::type_index type)
    {
        if (state.state != _SharedResourceState::State::NotLoaded && (state.state != _SharedResourceState::State::Failed || state.failedTypes.contains(type)))
            return;
        {
            std::lock_guard lk(state.mutex);
            if (state.state != _SharedResourceState::State::NotLoaded && (state.state != _SharedResourceState::State::Failed || state.failedTypes.contains(type)))
                return;

            state.state = _SharedResourceState::State::Loading;
        }
        auto res = m_loaders.find(type);
        if (res == m_loaders.end())
        {
            state.failedTypes.insert(type);
            state.exception = std::make_exception_ptr(std::logic_error("No resource loader found for requested type"));
            state.state = _SharedResourceState::State::Failed;
            rethrow_exception(state.exception);
        }

        state.type = type;

        auto loader = res->second.get();
        auto &path = state.path;
        ResourceRef<void> resource(state.weak_from_this());

        auto task = std::packaged_task<void()>([loader, path, resource, this]
                                               { loadTask(loader, path, resource); });
        *m_threadPool << std::move(task);
    }

    inline ResourceRef<void> ResourceContainer::operator+=(const std::filesystem::path &path)
    {
        std::optional<std::filesystem::path> _path;
        if (!std::filesystem::exists(path))
            _path = m_appDir / path;

        for (auto &&resource : m_resources)
            if (std::filesystem::equivalent(resource->path, _path ? *_path : path))
                return ResourceRef<void>(resource);

        return std::move(ResourceRef<void>(m_resources.emplace_back(new _SharedResourceState(_path ? *_path : path, this))));
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
