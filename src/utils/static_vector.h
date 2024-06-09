// from Stromphrax
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>

template <typename T, std::size_t Capacity> class StaticVector {
  public:
    StaticVector()  = default;
    ~StaticVector() = default;

    StaticVector(const StaticVector<T, Capacity> &other) { *this = other; }

    auto push(const T &elem) {
        assert(m_size < Capacity);
        m_data[m_size++] = elem;
    }

    auto pop_back() {
        assert(m_size > 0);
        --m_size;
    }

    void clear() { m_size = 0; }

    [[nodiscard]] auto size() const noexcept { return m_size; }

    [[nodiscard]] auto empty() const { return m_size == 0; }

    [[nodiscard]] auto operator[](std::size_t i) const -> const auto & {
        assert(i < m_size);
        return m_data[i];
    }

    [[nodiscard]] auto begin() { return m_data.begin(); }
    [[nodiscard]] auto end() { return m_data.begin() + static_cast<std::ptrdiff_t>(m_size); }

    [[nodiscard]] auto operator[](std::size_t i) -> auto & {
        assert(i < m_size);
        return m_data[i];
    }

    [[nodiscard]] auto begin() const { return m_data.begin(); }
    [[nodiscard]] auto end() const { return m_data.begin() + static_cast<std::ptrdiff_t>(m_size); }

    auto fill(const T &v) { m_data.fill(v); }

    auto resize(std::size_t size) {
        assert(size <= Capacity);
        m_size = size;
    }

    auto operator=(const StaticVector<T, Capacity> &other) -> auto & {
        std::copy(other.begin(), other.end(), begin());
        m_size = other.m_size;
        return *this;
    }

  private:
    std::array<T, Capacity> m_data{};
    std::size_t m_size{0};
};
