#include "../../include/core/game/camera.h"

namespace halogen
{
	Camera::Camera(): m_camera_position(camera_defaults::CAMERA_POSITION)
	{
		m_camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
		m_camera_direction = glm::normalize(m_camera_target - m_camera_position);
		m_camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
		m_camera_right = glm::normalize(glm::cross(m_camera_target, m_camera_up));
	}

	Camera::~Camera()
	{
	}

	//Note : [WIP] Entire camera class is messed up for now.
	void Camera::update_camera(Input& input)
	{
		//All inputs are reversed because the camera doesnt move, but the objects in the world do.
		//So, A doesnt make the camera go left, but makes the game objects move to the right.

		m_camera_right = glm::normalize(glm::cross(m_camera_target, m_camera_up));

		if (input.is_key_pressed(InputMap::KeyboardInput::A))
		{
			m_camera_position += m_camera_right * camera_defaults::DELTA_TIME_MULTIPLIER;
		}
		else if (input.is_key_pressed(InputMap::KeyboardInput::D))
		{
			m_camera_position += -m_camera_right * camera_defaults::DELTA_TIME_MULTIPLIER;
		}

		if (input.is_key_pressed(InputMap::KeyboardInput::W))
		{
			m_camera_position -= m_camera_target * camera_defaults::DELTA_TIME_MULTIPLIER;
		}
		else if (input.is_key_pressed(InputMap::KeyboardInput::S))
		{
			m_camera_position +=m_camera_target * camera_defaults::DELTA_TIME_MULTIPLIER;
		}

		if (input.is_key_pressed(InputMap::KeyboardInput::Q))
		{
			m_camera_position += glm::vec3(0.0f, 0.0f, 1.0f) * camera_defaults::DELTA_TIME_MULTIPLIER;
		}
		else if (input.is_key_pressed(InputMap::KeyboardInput::E))
		{
			m_camera_position += glm::vec3(0.0f, 0.0f, -1.0f) * camera_defaults::DELTA_TIME_MULTIPLIER;
		}

		//Need to add mouse scroll input later (Eular angles : YAW, PITCH, ROLL)
	}
}