#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Actor.h"

#include "Helper.h"

class ParticleSystem
{
public:
    void OnStart();
    void OnUpdate();

    std::string type = "ParticleSystem";
    std::string key = "???";
    Actor* actor = nullptr;
    bool enabled = true;

    float x = 0.0f;
    float y = 0.0f;
    int frames_between_bursts = 1;
    int burst_quantity = 1;
    float start_scale_min = 1.0f;
    float start_scale_max = 1.0f;
    float rotation_min = 0.0f;
    float rotation_max = 0.0f;
    int start_color_r = 255;
    int start_color_g = 255;
    int start_color_b = 255;
    int start_color_a = 255;
    float emit_radius_min = 0.0f;
    float emit_radius_max = 0.5f;
    float emit_angle_min = 0.0f;
    float emit_angle_max = 360.0f;
    std::string image = "default";
    int sorting_order = 9999;

    int duration_frames = 300;
    float start_speed_min = 0.0f;
    float start_speed_max = 0.0f;
    float rotation_speed_min = 0.0f;
    float rotation_speed_max = 0.0f;
    float gravity_scale_x = 0.0f;
    float gravity_scale_y = 0.0f;
    float drag_factor = 1.0f;
    float angular_drag_factor = 1.0f;
    float end_scale = std::nanf("");
    int end_color_r = -1;
    int end_color_g = -1;
    int end_color_b = -1;
    int end_color_a = -1;

    void Stop() { do_emission = false; }
    void Play() { do_emission = true; }

    void Burst() { GenerateNewParticles(burst_quantity); }
    
private:
    void GenerateNewParticles(int num);
    bool do_emission = true;

    int local_frame_number = 0;

    bool do_end_scale = false;
    bool do_tint_r = false;
    bool do_tint_g = false;
    bool do_tint_b = false;
    bool do_tint_a = false;

    RandomEngine emit_angle_distribution;
    RandomEngine emit_radius_distribution;

    RandomEngine rotation_distribution;
    RandomEngine scale_distribution;

    RandomEngine speed_distribution;
    RandomEngine rotation_speed_distribution;

    int num_particles = 0;
    int num_particle_slots = 0;
    std::vector<float> x_positions;
    std::vector<float> y_positions;
    std::vector<float> x_velocities;
    std::vector<float> y_velocities;
    std::vector<float> rotations;
    std::vector<float> rotation_velocities;
    std::vector<float> start_scales;
    std::vector<int> durations;
    std::vector<bool> is_alive;

    std::queue<int> free_list;
};

#endif
