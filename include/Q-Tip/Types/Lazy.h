#ifndef QTIP_LAZY_H
#define QTIP_LAZY_H

#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <optional>

template <typename T>
class Lazy {
    class Dependency {
    public:
        virtual ~Dependency() = default;

        [[nodiscard]] virtual bool changed() const = 0;
        virtual void update() = 0;
    };

    template <typename U>
    class TypedDependency : public Dependency {
    public:
        explicit TypedDependency(U* value)
            : _value(value),
              _old(*value) {}

        [[nodiscard]] bool changed() const override {
            return *_value != _old;
        }

        void update() override {
            _old = *_value;
        }

    private:
        U* _value;
        U _old;
    };

public:
    template <typename F, typename... Deps>
    Lazy(F function, Deps*... deps)
        : _function(std::move(function)) {
        (_deps.push_back(
            std::make_unique<TypedDependency<Deps>>(deps)
        ), ...);
    }

    template <typename F, typename... Deps>
    void init(F function, Deps*... deps) {
        _function = std::move(function);

        _deps.clear();

        (_deps.push_back(
            std::make_unique<TypedDependency<Deps>>(deps)
        ), ...);

        _value.reset();
    }

    T get() const {
        if (!_value || dependenciesChanged()) {
            _value = _function();

            for (auto& dependency : _deps)
                dependency->update();
        }

        return *_value;
    }

    operator T() const {
        return get();
    }

    T operator()() const {
        return get();
    }

private:
    std::function<T()> _function;
    mutable std::vector<std::unique_ptr<Dependency>> _deps;

    mutable std::optional<T> _value;

    bool dependenciesChanged() const {
        for (const auto& dependency : _deps) {
            if (dependency->changed())
                return true;
        }

        return false;
    }
};

#endif // QTIP_LAZY_H