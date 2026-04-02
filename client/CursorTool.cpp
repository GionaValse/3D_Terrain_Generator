#include "CursorTool.h"

#include <glm/glm.hpp>

CursorTool::CursorTool(float sensitivity)
	: BaseTool("Cursor", "./res/cursor.png"),
	mouseSensitivity(sensitivity)
{}

void CursorTool::use(Eng::Camera * camera, float deltaX, float deltaY)
{
	glm::mat4 matrix = camera->getMatrix();
	glm::vec3 localRight = glm::normalize(glm::vec3(matrix[0]));

	glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(-deltaY * mouseSensitivity), localRight);
	glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(-deltaX * mouseSensitivity), glm::vec3(0.0f, 1.0f, 0.0f));

	camera->setMatrix(rotationY * rotationX * matrix);
}
