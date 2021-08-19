#include "../../include/core/game/camera.h"

namespace halogen
{
	Camera::Camera(): m_camera_position(camera_defaults::CAMERA_POSITION)
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::update_camera(Input& input)
	{
		if (input.is_key_pressed(InputMap::KeyboardInput::A))
		{
			m_camera_position += glm::vec3(1.0f, 0.0f, 0.0f) * camera_defaults::DELTA_TIME_MULTIPLIER;
		}
		else if (input.is_key_pressed(InputMap::KeyboardInput::D))
		{
			m_camera_position += glm::vec3(-1.0f, 0.0f, 0.0f) * camera_defaults::DELTA_TIME_MULTIPLIER;
		}

		else if (input.is_key_pressed(InputMap::KeyboardInput::W))
		{
			m_camera_position += glm::vec3(0.0f, 1.0f, 0.0f) * camera_defaults::DELTA_TIME_MULTIPLIER;
		}
		else if (input.is_key_pressed(InputMap::KeyboardInput::S))
		{
			m_camera_position += glm::vec3(0.0f, -1.0f, 0.0f) * camera_defaults::DELTA_TIME_MULTIPLIER;
		}
	}
}