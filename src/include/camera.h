#pragma once

#include "custom_math.h"

namespace halo
{
	static constexpr float CAMERA_SPEED = 0.01f;
		
	// base camera class. Engine currently has only one viewport and one 'free' camera.
	struct Camera
	{
		// camera's coordinate system
		math::V3 m_up{ 0.0f, 1.0f, 0.0f };
		math::V3 m_right{ 1.0f, 0.0f, 0.0f };
		math::V3 m_front{ 0.0f, 0.0f, -1.0f };

		math::V3 m_position{ 0.0f, 0.0f, 10.0f };
		math::V3 m_world_up{ 0.0f, 1.0f, 0.0f };

		math::V3 m_target{ 0.0f, 0.0f, 0.0f };

		float m_pitch{ 0.0f };
		float m_yaw{ -90.0f };

		float m_last_x{ 0.0f };
		float m_last_y{ 0.0f };

		float m_sensitivity{ 0.02f };

		inline static Camera& get_instance()
		{
			static Camera camera;
			return camera;
		}

		inline math::M4 get_look_at()
		{
			return math::look_at(m_right.normalize(), m_up.normalize(), m_front.normalize(), m_position);
		}

		inline void update_angles()
		{
			m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

			math::V3 direction{ cos(radians(m_yaw)) * cos(radians(m_pitch)), sin(radians(m_pitch)), sin(radians(m_yaw)) * cos(radians(m_pitch)) };

			m_front = (direction).normalize();
			m_right = cross(m_front, m_world_up).normalize();
			m_up = (cross(m_right, m_front)).normalize();
		}

		inline void update_position(bool front, bool back, bool left, bool right, float delta_time)
		{
			if (front)
			{
				m_position += m_front * CAMERA_SPEED * delta_time;
			}
			else if (back)
			{
				m_position -= m_front * CAMERA_SPEED * (float)delta_time;
			}

			m_right = (cross(m_front, m_up)).normalize();

			if (left)
			{
				m_position -= m_right * CAMERA_SPEED * delta_time;
			}
			else if (right)
			{
				m_position += m_right * CAMERA_SPEED * delta_time;
			}

		}
	};
}