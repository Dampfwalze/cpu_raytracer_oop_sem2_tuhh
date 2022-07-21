#include <scene/sampler.h>
#include <rt_imgui.h>
#include <typeinfo>
#include <map>

namespace rt
{
    const m::Color<float> Sampler::invalidColor = m::Color<float>(1, 0, 1);

    m::Color<float> Sampler::sample(const SampleInfo &info) const
    {
        if (!this)
            return invalidColor;
        switch (info.type)
        {
        case SampleInfoType::UV:
            return sampleUV(info.asUV);
        case SampleInfoType::Index:
            return sampleIndex(info.asIndex);
        case SampleInfoType::Direction:
            return sampleDirection(info.asDirection);
        default:
            return invalidColor;
        }
    }

    bool Samplers::ColorSampler::onInspectorGUI()
    {
        return ImGui::ColorEdit3("Color", (float *)&color);
    }

    std::ostream &Samplers::ColorSampler ::toString(std::ostream &stream) const
    {
        return stream << "ColorSampler { name: \"" << name
                      << "\", color: " << color
                      << " }";
    }

    m::Color<float> Samplers::TextureSampler::samplePoint(m::ivec2 texCoords, const m::uvec2 &size) const
    {
        switch (wrapMethod)
        {
        case WrapMethod::Repeat:
            texCoords.x = texCoords.x % size.x;
            texCoords.y = texCoords.y % size.y;
            break;
        case WrapMethod::MirroredRepeat:
            texCoords.x = texCoords.x % (size.x * 2);
            texCoords.y = texCoords.y % (size.y * 2);
            texCoords.x = texCoords.x >= (signed)size.x ? (size.x * 2) - texCoords.x - 1 : texCoords.x;
            texCoords.y = texCoords.y >= (signed)size.y ? (size.y * 2) - texCoords.y - 1 : texCoords.y;
            break;
        case WrapMethod::Clamp:
            texCoords = m::clamp(texCoords, m::ivec2(0), (m::ivec2)size - m::ivec2(1));
            break;
        default:
            return invalidColor;
        }
        return texture->pixelAt(texCoords);
    }

    m::Color<float> Samplers::TextureSampler::sampleUV(const m::fvec2 &uv) const
    {
        if (!texture)
            return invalidColor;
        auto size = texture->getSize();

        switch (filterMethod)
        {
        case FilterMethod::Nearest:
        {
            m::ivec2 texCoords = m::round(uv * (m::fvec2)(size - m::uvec2(1)));
            return samplePoint(texCoords, size);
        }

        case FilterMethod::Linear:
        {
            auto texPos = uv * (m::fvec2)(size - m::uvec2(1));
            m::ivec2 t1 = m::floor(texPos);
            m::ivec2 t2 = m::ceil(texPos);
            m::ivec2 t3(t1.x, t2.y);
            m::ivec2 t4(t2.x, t1.y);

            auto c1 = m::mix(samplePoint(t1, size), samplePoint(t3, size), m::fract(texPos.y));
            auto c2 = m::mix(samplePoint(t4, size), samplePoint(t2, size), m::fract(texPos.y));
            return m::mix(c1, c2, m::fract(texPos.x));
        }
        default:
            return invalidColor;
        }
    }

    m::Color<float> Samplers::TextureSampler::sampleDirection(const m::fvec3 &direction_) const
    {
        if (!texture)
            return invalidColor;

        auto direction = glm::normalize(direction_);

        double pitch = m::asin(direction.y);
        double yaw = m::atan(direction.x / direction.z);
        if (direction.z == 0)
            yaw = m::pi<double>() / 2;
        else if (direction.z < 0)
            yaw += m::pi<double>();

        yaw = m::map(yaw, -m::pi<double>() / 2, m::pi<double>() / 2 * 3, 0.0, 1.0);
        pitch = m::map(pitch, -m::pi<double>() / 2, m::pi<double>() / 2, 1.0, 0.0);

        return sampleUV({yaw, pitch});
    }

    using FilterMethod = Samplers::TextureSampler::FilterMethod;
    using WrapMethod = Samplers::TextureSampler::WrapMethod;

    inline const char *filterMethodToString(FilterMethod m)
    {
        switch (m)
        {
        case FilterMethod::Linear:
            return "Linear";
        case FilterMethod::Nearest:
            return "Nearest";
        }
        return "None";
    }
    inline const char *wrapMethodToString(WrapMethod m)
    {
        switch (m)
        {
        case WrapMethod::Repeat:
            return "Repeat";
        case WrapMethod::MirroredRepeat:
            return "MirroredRepeat";
        case WrapMethod::Clamp:
            return "Clamp";
        }
        return "None";
    }

    bool Samplers::TextureSampler::onInspectorGUI()
    {
        bool changed = false;

        changed |= rtImGui::ResourceBox("Texture", texture);

        if (ImGui::BeginCombo("Filter Method", filterMethodToString(filterMethod)))
        {
            for (size_t i = 0; i < (size_t)FilterMethod::COUNT; i++)
                if (ImGui::Selectable(filterMethodToString((FilterMethod)i), (FilterMethod)i == filterMethod))
                {
                    filterMethod = (FilterMethod)i;
                    changed = true;
                }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Wrap Method", wrapMethodToString(wrapMethod)))
        {
            for (size_t i = 0; i < (size_t)WrapMethod::COUNT; i++)
                if (ImGui::Selectable(wrapMethodToString((WrapMethod)i), (WrapMethod)i == wrapMethod))
                {
                    wrapMethod = (WrapMethod)i;
                    changed = true;
                }
            ImGui::EndCombo();
        }
        return changed;
    }

    std::ostream &Samplers::TextureSampler::toString(std::ostream &stream) const
    {
        return stream << "TextureSampler { name: \"" << name
                      << "\", texture: " << texture
                      << ", filterMethod: " << filterMethodToString(filterMethod)
                      << ", wrapMethod: " << wrapMethodToString(wrapMethod)
                      << " }";
    }

    bool SamplerRef<Sampler>::onInspectorGUI()
    {
        static const std::map<std::type_index, const char *> namesMap = {
            {typeid(Samplers::ColorSampler), "Color Sampler"},
            {typeid(Samplers::TextureSampler), "Texture Sampler"},
        };

        auto &currentType = m_ptr ? typeid(*m_ptr) : typeid(void);

        auto currentName = m_ptr ? namesMap.find(currentType)->second : "None";

        bool changed = false;

        if (ImGui::BeginCombo("##Sampler Type", currentName))
        {
            for (auto &&[type, name] : namesMap)
            {
                if (ImGui::Selectable(name, type == currentType) && type != currentType)
                {
                    changed = true;
                    if (type == typeid(Samplers::ColorSampler))
                        m_ptr = std::make_unique<Samplers::ColorSampler>();
                    else if (type == typeid(Samplers::TextureSampler))
                        m_ptr = std::make_unique<Samplers::TextureSampler>();
                }
            }
            ImGui::EndCombo();
        }

        if (m_ptr)
            changed |= m_ptr->onInspectorGUI();

        return changed;
    }
} // namespace rt
