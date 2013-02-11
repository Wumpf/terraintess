#pragma once


class Camera
{
public:
	Camera(float fov, float aspectRatio);
	~Camera();

	const SimpleMath::Matrix& GetProjectionMatrix() const { return _projectionMatrix; }
	const SimpleMath::Matrix& GetViewMatrix() const { return _viewMatrix; }
	const SimpleMath::Matrix& GetViewProjectionMatrix() const { return _viewProjectionMatrix; }

private:
	SimpleMath::Matrix _projectionMatrix;
	SimpleMath::Matrix _viewMatrix;

	SimpleMath::Matrix _viewProjectionMatrix;
};

