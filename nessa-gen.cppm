export module nessa:gen;

namespace nessa::gen {
class freq_gen {
protected:
  float m_period;

  [[nodiscard]] constexpr float f(float t) const noexcept {
    auto n = t / m_period;
    return n - (int)n;
  }

public:
  constexpr void set_freq(float f) noexcept { m_period = 1.0f / f; }
};
export class saw_tooth : freq_gen {
public:
  using freq_gen::set_freq;

  [[nodiscard]] constexpr float operator()(float t) const noexcept {
    return f(t) * 2.0f - 1.0f;
  }
};
export class square : freq_gen {
  float m_duty = 0.5f;

public:
  using freq_gen::set_freq;
  constexpr void set_duty_cycle(float d) noexcept { m_duty = d; }

  [[nodiscard]] constexpr float operator()(float t) const noexcept {
    return f(t) < m_duty ? -1 : 1;
  }
};
export class triangle : freq_gen {
public:
  using freq_gen::set_freq;

  [[nodiscard]] constexpr float operator()(float t) const noexcept {
    const auto ft = f(t);
    if (ft < 0.5f)
      return ft * 4.0f - 1.0f;
    return -4.0f * ft + 3.0f;
  }
};
} // namespace nessa::gen
