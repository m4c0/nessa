#pragma leco app
import nessa;
import siaudio;
import sith;
import sitime;

using namespace nessa;

static constexpr const float bpm = 140.0;
static constexpr const float bps = bpm / 60.0;

class sqr {
  gen::square m_gen{};
  float m_base_vol{1};

public:
  sqr(float bv) : m_base_vol{bv} { m_gen.set_duty_cycle(0.5); }

  void set_note(midi::note n) {
    if (n == midi::EXTEND)
      return;
    m_gen.set_freq(midi::note_freq(n));
  }

  [[nodiscard]] float operator()(float t) const noexcept {
    float b = t * bps * 2.0f;
    if (b > 1.0f)
      b = 1.0f;

    float v = 0.9 - b * 0.4;
    return m_base_vol * v * m_gen(t);
  }
};
class noise5 {
  gen::noise m_gen{};

public:
  void set_note(midi::note n) {
    if (n == midi::EXTEND)
      return;
    m_gen.set_freq(midi::note_freq(n));
  }

  [[nodiscard]] float operator()(float t) const noexcept {
    float b = t * bps * 8.0f;
    if (b > 1.0f)
      b = 1.0f;

    float v = 1.0 - b;
    return v * m_gen(t);
  }
};

class player {
  sqr m_sq1{1.0};
  sqr m_sq2{0.5};
  gen::triangle m_tri{};
  noise5 m_noise{};
  volatile unsigned m_index;

  float m_ref_t{};

  [[nodiscard]] float time(unsigned idx) const noexcept {
    constexpr const auto frate = static_cast<float>(siaudio::os_streamer::rate);
    return static_cast<float>(idx) / frate;
  }

  void set_sq1_note(midi::note n) noexcept { m_sq1.set_note(n); }
  void set_sq2_note(midi::note n) noexcept { m_sq2.set_note(n); }
  void set_tri_note(midi::note n) noexcept {
    if (n == midi::EXTEND)
      return;
    m_tri.set_freq(midi::note_freq(n));
  }
  void set_noise_note(midi::note n) noexcept { m_noise.set_note(n); }

public:
  void operator()(float *buf, unsigned len) {
    constexpr const auto volume = 0.125f;

    auto idx = m_index;
    for (auto i = 0; i < len; ++i, ++idx) {
      float t = time(idx) - m_ref_t;
      float v = (m_sq1(t) + m_sq2(t) + m_tri(t) + m_noise(t)) * volume;
      *buf++ = v;
    }
    m_index = idx;
  }

  void set_notes(const midi::note (&n)[4]) noexcept {
    set_sq1_note(n[0]);
    set_sq2_note(n[1]);
    set_tri_note(n[2]);
    set_noise_note(n[3]);

    m_ref_t = time(m_index);
  }
};

class streamer : public siaudio::streamer<player> {
public:
  streamer() : siaudio::streamer<player>{player{}} {}
};

using namespace nessa::midi;
class thread : sith::thread {
  static constexpr const auto bpm = 140.0;
  static constexpr const auto bps = bpm / 60.0;
  static constexpr const auto notes_per_beat = 2.0;
  static constexpr const auto ms_per_note = 1000.0 / (bps * notes_per_beat);

  static constexpr const auto note_count = 32;
  static constexpr const midi::note inst_1[note_count] = {
      E4, EXTEND, B3, C4,     D4, EXTEND, C4,   B3,     //
      A3, EXTEND, A3, C4,     E4, EXTEND, D4,   C4,     //
      B3, EXTEND, B3, C4,     D4, EXTEND, E4,   EXTEND, //
      C4, EXTEND, A3, EXTEND, A3, EXTEND, MUTE, MUTE,   //
  };
  static constexpr const midi::note inst_2[note_count] = {
      MUTE, MUTE,   Ab3,  MUTE, MUTE, MUTE,   A3,   Ab3,  //
      E3,   EXTEND, MUTE, MUTE, MUTE, MUTE,   MUTE, MUTE, //
      Ab3,  EXTEND, Ab3,  A3,   B3,   EXTEND, MUTE, MUTE, //
      MUTE, MUTE,   MUTE, MUTE, MUTE, MUTE,   MUTE, MUTE, //
  };
  static constexpr const midi::note inst_3[note_count] = {
      E3,  E4,  E3,  E4,  E3,  E4,  E3,  E4,  //
      A2,  A3,  A2,  A3,  A2,  A3,  A2,  A3,  //
      Ab2, Ab3, Ab2, Ab3, Ab2, Ab3, Ab2, Ab3, //
      A2,  A3,  A2,  A3,  A2,  A3,  A2,  A3,  //
  };
  static constexpr const midi::note inst_4[note_count] = {
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
  };

public:
  using sith::thread::start;

  void run();
};

void thread::run() {
  streamer s{};
  for (auto i = 0; i < note_count; i++) {
    s.producer().set_notes({inst_1[i], inst_2[i], inst_3[i], inst_4[i]});
    sitime::sleep_ms(ms_per_note);
  }
}
int main() {
  thread t{};
  t.start();

  // Give the thread some time for proper startup. Then, it's a matter of
  // joining it. Since we don't check for interruptions in the thread itself,
  // this will exit right at the end of the song
  sitime::sleep(1);
}
