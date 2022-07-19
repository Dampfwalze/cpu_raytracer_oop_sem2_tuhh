#include <resource_container.h>

namespace rt
{
    void ResourceContainer::loadTask(ResourceLoader *loader, std::filesystem::path path, const ResourceRef<void> &resource)
    {
        try
        {
            if (!std::filesystem::exists(path))
                throw IOException(IOException::Type::NotFound);

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

    void ResourceContainer::requestLoad(_SharedResourceState &state, std::type_index type)
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

    ResourceRef<void> ResourceContainer::operator+=(const std::filesystem::path &path)
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
    T *ResourceContainer::add(T *loader)
    {
        auto result = m_loaders.emplace(typeid(Type), loader);
        if (!result.second)
            delete loader;
        return result.second ? loader : (T *)nullptr;
    }
} // namespace rt
