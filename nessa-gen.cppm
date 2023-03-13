export module nessa:gen;

export namespace nessa::gen {
class saw_tooth {
  float m_period;

public:
  constexpr void set_freq(float f) noexcept { m_period = 1.0f / f; }

  [[nodiscard]] constexpr float operator()(float t) const noexcept {
    auto n = t / m_period;
    auto fr = n - (int)n;
    return fr * 2.0f - 1.0f;
  }
};
class square {
  float m_period;
  float m_duty = 0.5f;

public:
  constexpr void set_freq(float f) noexcept { m_period = 1.0f / f; }
  constexpr void set_duty_cycle(float d) noexcept { m_duty = d; }

  [[nodiscard]] constexpr float operator()(float t) const noexcept {
    auto n = t / m_period;
    auto fr = n - (int)n;
    return fr < m_duty ? -1 : 1;
  }
};
} // namespace nessa::gen
