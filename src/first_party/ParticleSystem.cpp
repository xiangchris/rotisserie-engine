#include "ParticleSystem.h"

#include "ImageDB.h"
#include "Renderer.h"

#include "glm/glm.hpp"

void ParticleSystem::OnStart()
{
    ImageDB::CreateDefaultParticleTextureWithName(image);
    if (frames_between_bursts < 1)
        frames_between_bursts = 1;
    if (burst_quantity < 1)
        burst_quantity = 1;
    if (duration_frames < 1)
        duration_frames = 1;

    do_end_scale = end_scale > -1;
    do_tint_r = end_color_r > -1;
    do_tint_g = end_color_g > -1;
    do_tint_b = end_color_b > -1;
    do_tint_a = end_color_a > -1;

    emit_angle_distribution.Configure(emit_angle_min, emit_angle_max, 298);
    emit_radius_distribution.Configure(emit_radius_min, emit_radius_max, 404);
    rotation_distribution.Configure(rotation_min, rotation_max, 440);
    scale_distribution.Configure(start_scale_min, start_scale_max, 494);
    speed_distribution.Configure(start_speed_min, start_speed_max, 498);
    rotation_speed_distribution.Configure(rotation_speed_min, rotation_speed_max, 305);
}

void ParticleSystem::OnUpdate()
{
    if (do_emission && local_frame_number % frames_between_bursts == 0)
        GenerateNewParticles(burst_quantity);

    for (int i = 0; i < num_particle_slots; i++)
    {
        if (!is_alive[i])
            continue;

        if (durations[i] < duration_frames)
        {
            float lifetime_progress = static_cast<float>(durations[i]++) / duration_frames;

            x_velocities[i] += gravity_scale_x;
            y_velocities[i] += gravity_scale_y;
            x_velocities[i] *= drag_factor;
            y_velocities[i] *= drag_factor;
            rotation_velocities[i] *= angular_drag_factor;
            x_positions[i] += x_velocities[i];
            y_positions[i] += y_velocities[i];
            rotations[i] += rotation_velocities[i];
            float scale = do_end_scale ? glm::mix(start_scales[i], end_scale, lifetime_progress) : start_scales[i];
            int tint_r = do_tint_r ? glm::mix(start_color_r, end_color_r, lifetime_progress) : start_color_r;
            int tint_g = do_tint_g ? glm::mix(start_color_g, end_color_g, lifetime_progress) : start_color_g;
            int tint_b = do_tint_b ? glm::mix(start_color_b, end_color_b, lifetime_progress) : start_color_b;
            int tint_a = do_tint_a ? glm::mix(start_color_a, end_color_a, lifetime_progress) : start_color_a;

            Renderer::DrawImage(image, x_positions[i], y_positions[i], rotations[i], scale, scale, 0.5f, 0.5f, tint_r, tint_g, tint_b, tint_a, sorting_order);
        }
        else
        {
            is_alive[i] = false;
            free_list.push(i);
            num_particles--;
        }
    }

    local_frame_number++;
}

void ParticleSystem::GenerateNewParticles(int num)
{
    num_particles += burst_quantity;

    for (int i = 0; i < num; i++)
    {
        float angle_radians = glm::radians(emit_angle_distribution.Sample());
        float radius = emit_radius_distribution.Sample();

        float cos_angle = glm::cos(angle_radians);
        float sin_angle = glm::sin(angle_radians);

        float speed = speed_distribution.Sample();

        if (free_list.empty())
        {
            num_particle_slots++;

            x_positions.push_back(x + cos_angle * radius);
            y_positions.push_back(y + sin_angle * radius);
            x_velocities.push_back(cos_angle * speed);
            y_velocities.push_back(sin_angle * speed);
            rotations.push_back(rotation_distribution.Sample());
            rotation_velocities.push_back(rotation_speed_distribution.Sample());
            start_scales.push_back(scale_distribution.Sample());
            durations.push_back(0);
            is_alive.push_back(true);

        }
        else
        {
            int i = free_list.front();
            
            x_positions[i] = x + cos_angle * radius;
            y_positions[i] = y + sin_angle * radius;
            x_velocities[i] = cos_angle * speed;
            y_velocities[i] = sin_angle * speed;
            rotations[i] = rotation_distribution.Sample();
            rotation_velocities[i] = rotation_speed_distribution.Sample();
            start_scales[i] = scale_distribution.Sample();
            durations[i] = 0;
            is_alive[i] = true;

            free_list.pop();
        }
    }
}
