#pragma once

class Camera
{
public:
	Camera(float fov, float aspectRatio);
	virtual ~Camera();

	virtual void Update(float timeSinceLastUpdate) {}

	const SimpleMath::Matrix& GetProjectionMatrix() const { return _projectionMatrix; }
	const SimpleMath::Matrix& GetViewMatrix() const { return _viewMatrix; }
	const SimpleMath::Matrix& GetViewProjectionMatrix() const { return _viewProjectionMatrix; }

	const SimpleMath::Vector3& GetPosition() const { return _position; }
	const SimpleMath::Vector3& GetUp() const { return _up; }
	const SimpleMath::Vector3& GetViewDir() const { return _viewDir; }

protected:
	void UpdateMatrices();

	SimpleMath::Vector3 _position;
	SimpleMath::Vector3 _up;
	SimpleMath::Vector3 _viewDir;

private:
	SimpleMath::Matrix _projectionMatrix;
	SimpleMath::Matrix _viewMatrix;

	SimpleMath::Matrix _viewProjectionMatrix;
};