import nessa;
import siaudio;

enum midi_note {
  MUTE = -1,
  EXTEND = 0,
  C2 = 36,
  Db2,
  D2,
  Eb2,
  E2,
  F2,
  Gb2,
  G2,
  Ab2,
  A2,
  Bb2,
  B2,
  C3,
  Db3,
  D3,
  Eb3,
  E3,
  F3,
  Gb3,
  G3,
  Ab3,
  A3,
  Bb3,
  B3,
  C4,
  Db4,
  D4,
  Eb4,
  E4,
  F4,
  Gb4,
  G4,
  Ab4,
  A4,
  Bb4,
  B4,
  C5,
  Db5,
  D5,
  Eb5,
  E5,
  F5,
  Gb5,
  G5,
  Ab5,
  A5,
  Bb5,
  B5,
  C6,
  Db6,
  D6,
  Eb6,
  E6,
  F6,
  Gb6,
  G6,
  Ab6,
  A6,
  Bb6,
  B6,
};
static_assert(A4 == 69);
static_assert(B6 == 95);

static constexpr const float note_freqs[] = {
    16.351597831287414, 17.323914436054505,
    18.354047994837977, 19.445436482630058,
    20.601722307054366, 21.826764464562746,
    23.12465141947715,  24.499714748859326,
    25.956543598746574, 27.5,
    29.13523509488062,  30.86770632850775,
    32.70319566257483,  34.64782887210901,
    36.70809598967594,  38.890872965260115,
    41.20344461410875,  43.653528929125486,
    46.2493028389543,   48.999429497718666,
    51.91308719749314,  55.0,
    58.27047018976124,  61.7354126570155,
    65.40639132514966,  69.29565774421802,
    73.41619197935188,  77.78174593052023,
    82.4068892282175,   87.30705785825097,
    92.4986056779086,   97.99885899543733,
    103.82617439498628, 110.0,
    116.54094037952248, 123.47082531403103,
    130.8127826502993,  138.59131548843604,
    146.8323839587038,  155.56349186104046,
    164.81377845643496, 174.61411571650194,
    184.9972113558172,  195.99771799087463,
    207.65234878997256, 220.0,
    233.08188075904496, 246.94165062806206,
    261.6255653005986,  277.1826309768721,
    293.6647679174076,  311.1269837220809,
    329.6275569128699,  349.2282314330039,
    369.9944227116344,  391.99543598174927,
    415.3046975799451,  440.0,
    466.1637615180899,  493.8833012561241,
    523.2511306011972,  554.3652619537442,
    587.3295358348151,  622.2539674441618,
    659.2551138257398,  698.4564628660078,
    739.9888454232688,  783.9908719634985,
    830.6093951598903,  880.0,
    932.3275230361799,  987.7666025122483,
    1046.5022612023945, 1108.7305239074883,
    1174.6590716696303, 1244.5079348883237,
    1318.5102276514797, 1396.9129257320155,
    1479.9776908465376, 1567.981743926997,
    1661.2187903197805, 1760.0,
    1864.6550460723597, 1975.533205024496,
    2093.004522404789,  2217.4610478149766,
    2349.31814333926,   2489.0158697766474,
    2637.02045530296,   2793.825851464031,
    2959.955381693075,  3135.9634878539946,
    3322.437580639561,  3520.0,
    3729.3100921447194, 3951.066410048992,
    4186.009044809578,  4434.922095629953,
    4698.63628667852,   4978.031739553295,
    5274.04091060592,   5587.651702928062,
    5919.91076338615,   6271.926975707989,
    6644.875161279122,  7040.0,
    7458.620184289437,  7902.132820097988,
};
static constexpr const auto C0_MIDI_ID = 12;
static_assert(note_freqs[A4 - C0_MIDI_ID] == 440.0f);

class sqr : public nessa::gen::square {
  float m_base_vol{1};
  float m_ref_t{};
  float m_bps{};

public:
  sqr(float bv) : m_base_vol{bv} {}

  using square::set_duty_cycle;
  using square::set_freq;

  void set_ref_time(float t) noexcept { m_ref_t = t; }
  void set_bps(float bps) noexcept { m_bps = bps; }

  [[nodiscard]] float operator()(float t) const noexcept {
    float b = (t - m_ref_t) * m_bps * 2.0f;
    if (b > 1.0f)
      b = 1.0f;

    float v = 0.9 - b * 0.4;
    return m_base_vol * v * square::operator()(t);
  }
};

class noise5 : public nessa::gen::noise {
  float m_ref_t{};
  float m_bps{};

public:
  using noise::set_freq;

  void set_ref_time(float t) noexcept { m_ref_t = t; }
  void set_bps(float bps) noexcept { m_bps = bps; }

  [[nodiscard]] float operator()(float t) const noexcept {
    float b = (t - m_ref_t) * m_bps * 8.0f;
    if (b > 1.0f)
      b = 1.0f;

    float v = 1.0 - b;
    return v * noise::operator()(t);
  }
};

class mixer {
  sqr m_sq1{1.0};
  sqr m_sq2{0.5};
  nessa::gen::triangle m_tri{};
  noise5 m_noise{};

public:
  [[nodiscard]] float operator()(float t) const noexcept {
    constexpr const auto volume = 0.125f;
    return (m_sq1(t) + m_sq2(t) + m_tri(t) + m_noise(t)) * volume;
  }

  [[nodiscard]] constexpr auto &square_1() noexcept { return m_sq1; }
  [[nodiscard]] constexpr auto &square_2() noexcept { return m_sq2; }
  [[nodiscard]] constexpr auto &triangle() noexcept { return m_tri; }
  [[nodiscard]] constexpr auto &noise() noexcept { return m_noise; }
};

class player {
  static constexpr const float bpm = 140.0;
  static constexpr const float bps = bpm / 60.0;

  mixer m{};
  volatile unsigned m_index;

  [[nodiscard]] float time(unsigned idx) const noexcept {
    constexpr const auto frate = static_cast<float>(siaudio::os_streamer::rate);
    return static_cast<float>(idx) / frate;
  }

public:
  void operator()(float *buf, unsigned len) {
    auto idx = m_index;
    for (auto i = 0; i < len; ++i, ++idx) {
      *buf++ = m(time(idx));
    }
    m_index = idx;
  }

  [[nodiscard]] auto current_note_index() const noexcept {
    constexpr const float notes_per_beat = 2;
    return static_cast<unsigned>(time(m_index) * bps * notes_per_beat);
  }

  void set_sq1_note(midi_note n) noexcept {
    if (n == EXTEND)
      return;
    m.square_1().set_freq(note_freqs[n - C0_MIDI_ID]);
    m.square_1().set_duty_cycle(0.5);
    m.square_1().set_ref_time(time(m_index));
    m.square_1().set_bps(bps);
  }
  void set_sq2_note(midi_note n) noexcept {
    if (n == EXTEND)
      return;
    m.square_2().set_freq(note_freqs[n - C0_MIDI_ID]);
    m.square_2().set_duty_cycle(0.5);
    m.square_2().set_ref_time(time(m_index));
    m.square_2().set_bps(bps);
  }
  void set_tri_note(midi_note n) noexcept {
    if (n == EXTEND)
      return;
    m.triangle().set_freq(note_freqs[n - C0_MIDI_ID]);
  }
  void set_noise_note(midi_note n) noexcept {
    if (n == EXTEND)
      return;
    m.noise().set_freq(note_freqs[n - C0_MIDI_ID]);
    m.noise().set_ref_time(time(m_index));
    m.noise().set_bps(bps);
  }
};

extern "C" auto *poc_start() {
  static siaudio::streamer s{player{}};
  return &s;
}
extern "C" bool poc_loop() {
  constexpr const auto note_count = 32;
  constexpr const midi_note inst_1[note_count] = {
      E4, EXTEND, B3, C4,     D4, EXTEND, C4,   B3,     //
      A3, EXTEND, A3, C4,     E4, EXTEND, D4,   C4,     //
      B3, EXTEND, B3, C4,     D4, EXTEND, E4,   EXTEND, //
      C4, EXTEND, A3, EXTEND, A3, EXTEND, MUTE, MUTE,   //
  };
  constexpr const midi_note inst_2[note_count] = {
      MUTE, MUTE,   Ab3,  MUTE, MUTE, MUTE,   A3,   Ab3,  //
      E3,   EXTEND, MUTE, MUTE, MUTE, MUTE,   MUTE, MUTE, //
      Ab3,  EXTEND, Ab3,  A3,   B3,   EXTEND, MUTE, MUTE, //
      MUTE, MUTE,   MUTE, MUTE, MUTE, MUTE,   MUTE, MUTE, //
  };
  constexpr const midi_note inst_3[note_count] = {
      E3,  E4,  E3,  E4,  E3,  E4,  E3,  E4,  //
      A2,  A3,  A2,  A3,  A2,  A3,  A2,  A3,  //
      Ab2, Ab3, Ab2, Ab3, Ab2, Ab3, Ab2, Ab3, //
      A2,  A3,  A2,  A3,  A2,  A3,  A2,  A3,  //
  };
  constexpr const midi_note inst_4[note_count] = {
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
  };
  auto &p = poc_start()->producer();
  auto i = p.current_note_index();
  if (i >= note_count)
    return false;

  static int last_note = -1;
  if (last_note == i)
    return true;

  p.set_sq1_note(inst_1[i]);
  p.set_sq2_note(inst_2[i]);
  p.set_tri_note((midi_note)(inst_3[i]));
  p.set_noise_note(inst_4[i]);
  last_note = i;

  return true;
}

int main() {
  auto s = poc_start();
  while (poc_loop()) {
  }
}
