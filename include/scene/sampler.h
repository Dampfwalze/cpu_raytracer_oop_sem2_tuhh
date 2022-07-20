#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <rtmath.h>
#include <memory>
#include <resources.h>
#include <scene/scene_object.h>

namespace rt
{
    namespace m = math;

    enum class SampleInfoType
    {
        None,
        UV,
        Direction,
        Index,
    };

    struct SampleInfo
    {
        SampleInfoType type = SampleInfoType::None;
        union
        {
            m::fvec2 asUV;
            m::fvec3 asDirection;
            size_t asIndex;
        };
    };

    class Sampler : public SceneObject
    {
    protected:
        static const m::Color<float> invalidColor;

    public:
        Sampler(const std::string_view &name) : SceneObject(name) {}
        virtual ~Sampler() = default;

        m::Color<float> sample(const SampleInfo &info) const;

        virtual m::Color<float> sampleUV(const m::fvec2 &uv) const { return m::Color<float>(1, 0, 1); }
        virtual m::Color<float> sampleDirection(const m::fvec3 &direction) const { return m::Color<float>(1, 0, 1); }
        virtual m::Color<float> sampleIndex(size_t index) const { return m::Color<float>(1, 0, 1); }
    };

    namespace Samplers
    {
        class ColorSampler : public Sampler
        {
        public:
            m::Color<float> color;

        public:
            ColorSampler(const m::Color<float> &color = m::Color<float>(0.9f, 0.9f, 0.9f))
                : color(color), Sampler("Color Sampler") {}
            ColorSampler(const std::string_view &name, const m::Color<float> &color = m::Color<float>(0.9f, 0.9f, 0.9f))
                : color(color), Sampler(name) {}

            virtual m::Color<float> sampleUV(const m::fvec2 &uv) const override { return color; }
            virtual m::Color<float> sampleDirection(const m::fvec3 &direction) const override { return color; }
            virtual m::Color<float> sampleIndex(size_t index) const override { return color; }

            virtual bool onInspectorGUI() override;

            virtual std::ostream &toString(std::ostream &stream) const;
        };

        class TextureSampler : public Sampler
        {
        public:
            ResourceRef<Resources::TextureResource> texture;

            enum class FilterMethod
            {
                Linear,
                Nearest,
                COUNT,
            } filterMethod = FilterMethod::Linear;

            enum class WrapMethod
            {
                Repeat,
                MirroredRepeat,
                Clamp,
                COUNT,
            } wrapMethod = WrapMethod::Repeat;

        public:
            TextureSampler() : Sampler("Texture Sampler") {}
            TextureSampler(const ResourceRef<Resources::TextureResource> &texture,
                           FilterMethod filterMethod,
                           WrapMethod wrapMethod = WrapMethod::Repeat)
                : texture(texture), filterMethod(filterMethod), wrapMethod(wrapMethod), Sampler("Texture Sampler") {}
            TextureSampler(const ResourceRef<Resources::TextureResource> &texture,
                           WrapMethod wrapMethod)
                : texture(texture), wrapMethod(wrapMethod), Sampler("Texture Sampler") {}
            TextureSampler(const ResourceRef<Resources::TextureResource> &texture)
                : texture(texture), Sampler("Texture Sampler") {}

            TextureSampler(const std::string_view &name, const ResourceRef<Resources::TextureResource> &texture,
                           FilterMethod filterMethod,
                           WrapMethod wrapMethod = WrapMethod::Repeat)
                : texture(texture), filterMethod(filterMethod), wrapMethod(wrapMethod), Sampler(name) {}
            TextureSampler(const std::string_view &name, const ResourceRef<Resources::TextureResource> &texture,
                           WrapMethod wrapMethod)
                : texture(texture), wrapMethod(wrapMethod), Sampler(name) {}
            TextureSampler(const std::string_view &name, const ResourceRef<Resources::TextureResource> &texture)
                : texture(texture), Sampler(name) {}

        protected:
            m::Color<float> samplePoint(m::ivec2 texCoords, const m::uvec2 &size) const;

            virtual m::Color<float> sampleUV(const m::fvec2 &uv) const override;
            virtual m::Color<float> sampleDirection(const m::fvec3 &direction) const override;

            virtual bool onInspectorGUI() override;

            virtual std::ostream &toString(std::ostream &stream) const;
        };
    } // namespace Samplers

    template <class T = Sampler>
    class SamplerRef
    {
        static_assert(std::is_convertible<T, Sampler>::value || std::is_same<T, Sampler>::value, "");

    private:
        std::unique_ptr<T> m_ptr;

    public:
        SamplerRef(T *ptr) : m_ptr(ptr) {}
        template <class F>
        SamplerRef(F *ptr) : m_ptr(static_cast<T *>(ptr)) {}
        SamplerRef(std::unique_ptr<T> &&ptr) : m_ptr(std::move(ptr)) {}
        template <class F>
        SamplerRef(std::unique_ptr<F> &&ptr)
            : m_ptr(static_cast<T *>(ptr.release())) {}

        inline operator bool() const { return m_ptr.operator bool(); }
        inline T *operator->() { return m_ptr.get(); }
        inline const T *operator->() const { return m_ptr.get(); }
        inline T &operator*() { return *m_ptr; }
        inline const T &operator*() const { return *m_ptr; }

        inline bool onInspectorGUI() { return m_ptr->onInspectorGUI(); }
    };

    template <>
    class SamplerRef<Sampler>
    {
    private:
        std::unique_ptr<Sampler> m_ptr;

    public:
        SamplerRef() = default;
        SamplerRef(Sampler *ptr) : m_ptr(ptr) {}
        template <class F>
        SamplerRef(F *ptr) : m_ptr(static_cast<Sampler *>(ptr)) {}
        SamplerRef(std::unique_ptr<Sampler> &&ptr) : m_ptr(std::move(ptr)) {}
        template <class F>
        SamplerRef(std::unique_ptr<F> &&ptr)
            : m_ptr(static_cast<Sampler *>(ptr.release())) {}

        inline operator bool() const { return m_ptr.operator bool(); }
        inline Sampler *operator->() { return m_ptr.get(); }
        inline const Sampler *operator->() const { return m_ptr.get(); }
        inline Sampler &operator*() { return *m_ptr; }
        inline const Sampler &operator*() const { return *m_ptr; }

        bool onInspectorGUI();
    };

    template <class T>
    inline std::ostream &operator<<(std::ostream &stream, const SamplerRef<T> &ref)
    {
        if (ref)
            return stream << *ref;
        else
            return stream << "null";
    }
} // namespace rt

#endif // SAMPLER_HPP
