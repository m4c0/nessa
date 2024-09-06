#pragma leco app
import nessa;
import siaudio;
import sith;
import sitime;

using namespace nessa;

static constexpr auto clamp(float b) { return b > 1.0f ? 1.0 : b; }

static constexpr auto sqr_env(float t_b) noexcept {
  float b = clamp(t_b * 2.0f);
  return 0.9 - b * 0.4;
}
static constexpr auto noise_env(float t_b) noexcept {
  float b = clamp(t_b * 8.0f);
  return 1.0 - b;
}

using namespace nessa::midi;
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

class player : public midi::player {
protected:
  constexpr float vol_at(float t) const noexcept override {
    constexpr const auto volume = 0.125f;

    float t_b = t * bps();

    float vsq1 = 1.0 * sqr_env(t_b) * gen::square(t * note_freq(0));
    float vsq2 = 0.5 * sqr_env(t_b) * gen::square(t * note_freq(1));
    float vtri = gen::triangle(t * note_freq(2));
    float vnoi = noise_env(t_b) * gen::noise(t * note_freq(3));

    return (vsq1 + vsq2 + vtri + vnoi) * volume;
  }
};

void play(auto) {
  ::player p{};
  siaudio::filler([&](float * f, unsigned n) { p.fill_buffer(f, n); });
  siaudio::rate(44100);
  p.set_bpm(140);
  for (auto i = 0; i < note_count; i++) {
    p.play_notes({inst_1[i], inst_2[i], inst_3[i], inst_4[i]});
    p.set_bpm(140 + i * 2);
  }
}

int main() {
  sith::stateless_thread t{play};
  sith::run_guard r { &t };

  // Give the thread some time for proper startup. Then, it's a matter of
  // joining it. Since we don't check for interruptions in the thread itself,
  // this will exit right at the end of the song
  sitime::sleep(1);
}
