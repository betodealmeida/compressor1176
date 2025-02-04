#include "lv2/core/lv2.h"
#include <cmath>

enum Ports {
    IN = 0,
    OUT,
    RATIO,
    ATTACK,
    RELEASE,
    INPUT_PAD
};

class Compressor1176 {
private:
    const float* input;
    float* output;
    const float* ratio;
    const float* attack;
    const float* release;
    const float* input_pad;
    
    float sample_rate;
    float envelope;
    float last_gain;
    
    // FET curve coefficients (modeled after actual 1176 measurements)
    const float fet_k = 0.9f;
    const float fet_exp = 0.35f;

    float fet_curve(float x) {
        // Approximate JFET response curve
        return fet_k * (1.0f - expf(-fet_exp * fabsf(x))) * (x > 0 ? 1.0f : -1.0f);
    }
    
    float program_dependent_release(float env_db) {
        // Release time gets longer as signal level increases
        return *release * (1.0f + 0.5f * env_db / 60.0f);
    }
    
    float compute_attack_coeff() {
        return expf(-1.0f / (sample_rate * *attack));
    }
    
    float compute_release_coeff(float env_db) {
        return expf(-1.0f / (sample_rate * program_dependent_release(env_db)));
    }

public:
    Compressor1176(double rate) : sample_rate(rate), envelope(0.0f), last_gain(1.0f) {}
    
    void connect_port(uint32_t port, void* data) {
        switch (port) {
            case IN: input = (const float*)data; break;
            case OUT: output = (float*)data; break;
            case RATIO: ratio = (const float*)data; break;
            case ATTACK: attack = (const float*)data; break;
            case RELEASE: release = (const float*)data; break;
            case INPUT_PAD: input_pad = (const float*)data; break;
        }
    }

    void run(uint32_t n_samples) {
        for (uint32_t i = 0; i < n_samples; i++) {
            // Input stage with padding
            float in = input[i] * powf(10.0f, -*input_pad / 20.0f);
            
            // FET distortion on input
            float distorted = fet_curve(in);
            
            // Level detection (feed-forward topology)
            float input_level = fabsf(distorted);
            float env_db = 20.0f * log10f(input_level + 1e-10f);
            
            // Dynamic envelope following
            float attack_coeff = compute_attack_coeff();
            float release_coeff = compute_release_coeff(env_db);
            
            if (input_level > envelope) {
                envelope = attack_coeff * envelope + (1.0f - attack_coeff) * input_level;
            } else {
                envelope = release_coeff * envelope + (1.0f - release_coeff) * input_level;
            }
            
            // Compression curve
            float env_db_out = 20.0f * log10f(envelope + 1e-10f);
            float gain_reduction = 0.0f;
            
            if (env_db_out > -30.0f) {  // Compression threshold around -30dB
                gain_reduction = (env_db_out + 30.0f) * (1.0f - 1.0f / *ratio);
            }
            
            // Smooth gain changes
            float gain = powf(10.0f, -gain_reduction / 20.0f);
            gain = 0.9f * last_gain + 0.1f * gain;  // Simple smoothing
            last_gain = gain;
            
            // Output with makeup gain (fixed at +6dB like original 1176)
            output[i] = distorted * gain * 2.0f;
        }
    }
};

static LV2_Handle instantiate(const LV2_Descriptor*, double rate, const char*, const LV2_Feature* const*) {
    return new Compressor1176(rate);
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
    ((Compressor1176*)instance)->connect_port(port, data);
}

static void run(LV2_Handle instance, uint32_t n_samples) {
    ((Compressor1176*)instance)->run(n_samples);
}

static void cleanup(LV2_Handle instance) {
    delete (Compressor1176*)instance;
}

static const LV2_Descriptor descriptor = {
    "http://your-domain.com/plugins/compressor1176",
    instantiate,
    connect_port,
    nullptr,
    run,
    nullptr,
    cleanup,
    nullptr
};

extern "C" LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) {
    return index == 0 ? &descriptor : nullptr;
}
