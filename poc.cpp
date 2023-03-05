import siaudio;

struct square_gen {
  [[nodiscard]] float operator()(unsigned n) {
    return ((n / 40) % 2) * 2.0f - 1.0f;
  }
};

class mixer {
  square_gen m_g;
  volatile unsigned m_index;

public:
  [[nodiscard]] float operator()() noexcept {
    auto i = m_index;
    m_index = i + 1;
    return m_g(i);
  }

  [[nodiscard]] unsigned index() const noexcept { return m_index; }
};

int main() {
  mixer m{};
  siaudio::streamer s{[&](float *buf, unsigned len) {
    for (auto i = 0; i < len; ++i) {
      *buf++ = m();
    }
  }};
  while (m.index() < 44100) {
  }
}
